// DOM ownership stays here. The runtime gets one Canvas and explicit lifecycle
// calls; Core and the render pthread do not depend on a hosting URL or SW.
const start = document.querySelector('#start');
const stop = document.querySelector('#stop');
const status = document.querySelector('#status');
const base = new URL(new URL(location.href).searchParams.get('runtime') || './', import.meta.url);
let runtime;
let loading;

function loadRuntime() {
  if (!loading) {
    loading = new Promise((resolve, reject) => {
      const script = document.createElement('script');
      script.src = new URL('dali-web.js', base).href;
      script.onload = resolve;
      script.onerror = () => reject(new Error('Cannot load the DALi runtime'));
      document.head.append(script);
    });
  }
  return loading;
}

start.onclick = async () => {
  if (!crossOriginIsolated || typeof SharedArrayBuffer === 'undefined' || typeof OffscreenCanvas === 'undefined') {
    status.textContent = 'Isolation, shared memory and OffscreenCanvas are required. No fallback or automatic retry.';
    return;
  }
  start.disabled = true;
  status.textContent = 'Starting';
  try {
    await loadRuntime();
    runtime = await globalThis.createDaliWeb({
      canvas: document.querySelector('#canvas'),
      locateFile: name => new URL(name, base).href,
      onDaliStatus: event => {
        status.textContent = event.reason ? `${event.state}: ${event.reason}` : event.state;
        if (event.state === 'ready') stop.disabled = false;
        if (event.state === 'failure') stop.disabled = true;
        if (event.state === 'stopped') {
          // A transferred Canvas and failed Wasm instance are never reused.
          const old = document.querySelector('#canvas');
          const fresh = document.createElement('canvas');
          fresh.id = 'canvas';
          fresh.width = 320;
          fresh.height = 240;
          old.replaceWith(fresh);
          runtime = null;
          start.disabled = false;
          stop.disabled = true;
        }
      }
    });
    runtime._dali_web_start();
    stop.disabled = !runtime;
  } catch (error) {
    status.textContent = `Startup failed: ${error.message}. Reload before trying again.`;
    // Arbitrary module/runtime aborts are outside the explicit backend OOM
    // contract; don't pretend that an orderly stopped handshake occurred.
    stop.disabled = true;
  }
};
stop.onclick = () => {
  stop.disabled = true;
  status.textContent = 'Stopping';
  runtime?._dali_web_stop();
};
