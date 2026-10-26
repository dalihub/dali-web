// Local P3 fixture. No compiler service or external resources.
window.productionResult = {events: [], pixels: [], errors: [], done: false};
const result = window.productionResult;
addEventListener('error', event => result.errors.push(event.message));
addEventListener('unhandledrejection', event => result.errors.push(String(event.reason)));
(async () => {
  try {
    const sw = new URLSearchParams(location.search).get('sw') === '1';
    if (sw && !crossOriginIsolated) {
      sessionStorage.setItem('firstIsolation', String(crossOriginIsolated));
      if (sessionStorage.getItem('reloaded')) throw new Error('SW reload did not isolate');
      await navigator.serviceWorker.register('./production-sw.js');
      await navigator.serviceWorker.ready;
      sessionStorage.setItem('reloaded', '1');
      location.reload();
      return;
    }
    result.isolation = crossOriginIsolated;
    result.firstIsolation = sessionStorage.getItem('firstIsolation');
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
      canvas: document.querySelector('#canvas'),
      locateFile: name => new URL('./runtime/' + name, location.href).href,
      printErr: line => result.errors.push(String(line)),
      onDaliPixels: frame => result.pixels.push(frame),
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
