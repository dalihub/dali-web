// C startup diagnostic: a dedicated Worker is the module's main runtime thread.
// The unmodified host creates its update/render pthread here. No DOM/GL shims.
let module;
const send = (kind, value) => postMessage({kind, value});
addEventListener('unhandledrejection', event => send('error', String(event.reason)));
onmessage = async event => {
  try {
    if (event.data.command === 'stop') {
      module?._dali_web_stop();
      return;
    }
    if (event.data.command !== 'start') return;
    const canvas = event.data.canvas;
    canvas.id = 'canvas';
    send('environment', {
      isolation: crossOriginIsolated,
      document: typeof document,
      offscreenCanvas: canvas instanceof OffscreenCanvas,
      transferControlToOffscreen: typeof canvas.transferControlToOffscreen,
      requestAnimationFrame: typeof requestAnimationFrame
    });
    importScripts('./runtime/dali-web.js');
    module = await createDaliWeb({
      canvas,
      mainScriptUrlOrBlob: new URL('./runtime/dali-web.js', location.href).href,
      locateFile: name => new URL('./runtime/' + name, location.href).href,
      printErr: line => send('error', String(line)),
      onAbort: reason => send('error', String(reason)),
      onDaliStatus: value => send('status', value)
    });
    send('sharedMemory', module.HEAPU8.buffer instanceof SharedArrayBuffer);
    module._dali_web_start();
  } catch (error) {
    send('exception', String(error.stack || error));
  }
};
