// Local P3 fixture. No compiler service or external resources.
window.productionResult = {events: [], pixels: [], memory: [], errors: [], done: false};
const result = window.productionResult;
addEventListener('error', event => result.errors.push(event.message));
addEventListener('unhandledrejection', event => result.errors.push(String(event.reason)));
const isolationKey = 'dali-sw:' + new URL('./production-sw.js', location.href).href;
const reloadKey = isolationKey + ':reloaded';
const firstKey = isolationKey + ':firstIsolation';

async function prepareIsolation() {
  const sw = new URLSearchParams(location.search).get('sw') === '1';
  if (!sw) return true;
  if (crossOriginIsolated) {
    sessionStorage.removeItem(reloadKey);
    return true;
  }
  sessionStorage.setItem(firstKey, String(crossOriginIsolated));
  if (sessionStorage.getItem(reloadKey)) throw new Error('SW reload did not isolate');
  let timer;
  let onControllerChange;
  let finished = false;
  try {
    await Promise.race([
      (async () => {
        await navigator.serviceWorker.register('./production-sw.js');
        await navigator.serviceWorker.ready;
        if (finished) return;
        if (!navigator.serviceWorker.controller) {
          await new Promise(resolve => {
            onControllerChange = () => {
              if (navigator.serviceWorker.controller) resolve();
            };
            navigator.serviceWorker.addEventListener('controllerchange', onControllerChange);
            onControllerChange();
          });
        }
      })(),
      new Promise((resolve, reject) => {
        timer = setTimeout(() => reject(new Error('SW registration/control timed out')), 5000);
      })
    ]);
  } finally {
    finished = true;
    clearTimeout(timer);
    if (onControllerChange) navigator.serviceWorker.removeEventListener('controllerchange', onControllerChange);
  }
  sessionStorage.setItem(reloadKey, '1');
  location.reload();
  return false;
}

(async () => {
  try {
    try {
      if (!await prepareIsolation()) return;
    } catch (error) {
      const message = document.querySelector('#isolation-error');
      const retry = document.querySelector('#isolation-retry');
      message.textContent = '실행 환경을 준비하지 못했습니다. ' + String(error);
      message.hidden = false;
      retry.hidden = false;
      retry.onclick = () => {
        retry.disabled = true;
        sessionStorage.removeItem(reloadKey);
        sessionStorage.removeItem(firstKey);
        location.reload();
      };
      throw error;
    }
    result.isolation = crossOriginIsolated;
    result.firstIsolation = sessionStorage.getItem(firstKey);
    result.controlled = !!navigator.serviceWorker.controller;
    if (new URLSearchParams(location.search).get('trace') === '1') {
      const OriginalWorker = Worker;
      window.Worker = class extends OriginalWorker {
        constructor(url, options) {
          const source = `
            let lastCall;
            const report = text => postMessage({cmd: 'printErr', text});
            addEventListener('error', event => report(String(event.error?.stack || event.message)));
            const prototype = WebGL2RenderingContext.prototype;
            for (const name of Object.getOwnPropertyNames(prototype)) {
              const descriptor = Object.getOwnPropertyDescriptor(prototype, name);
              if (name === 'constructor' || typeof descriptor.value !== 'function') continue;
              const original = descriptor.value;
              prototype[name] = function(...args) {
                const value = original.apply(this, args);
                if (name === 'getError') {
                  if (value) report('GL error ' + value + ' after ' + lastCall + '\\n' + new Error().stack);
                } else if (name !== 'isContextLost') lastCall = name + '(' + args.map(String).join(',') + ')';
                return value;
              };
            }
            importScripts(${JSON.stringify(new URL(url, location.href).href)});
          `;
          super(URL.createObjectURL(new Blob([source], {type: 'text/javascript'})), options);
        }
      };
    }
    const script = document.createElement('script');
    script.src = './runtime/dali-web.js';
    await new Promise((resolve, reject) => {
      script.onload = resolve;
      script.onerror = reject;
      document.head.append(script);
    });
    const module = await createDaliWeb({
      daliProbeMode: Number(new URLSearchParams(location.search).get('failure') || 0),
      canvas: document.querySelector('#canvas'),
      locateFile: name => new URL('./runtime/' + name, location.href).href,
      printErr: line => result.errors.push(String(line)),
      onDaliPixels: frame => result.pixels.push(frame),
      onDaliMemory: check => result.memory.push(check),
      onAbort: reason => { result.errors.push(String(reason)); result.done = true; },
      onDaliStatus: event => {
        result.events.push(event);
        if (event.state === 'stopped') result.done = true;
      }
    });
    result.sharedMemory = module.HEAPU8.buffer instanceof SharedArrayBuffer;
    window.productionModule = module;
    module._dali_web_start();
    // Let real frames run, then exercise the asynchronous stop handshake.
    setTimeout(() => module._dali_web_stop(), 3000);
  } catch (error) {
    result.errors.push(String(error));
    result.done = true;
  }
})();
