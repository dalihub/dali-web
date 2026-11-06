// Local startup gate only. Cleanup requests are not proof of full reclamation.
window.productionResult = {events: [], errors: [], done: false, heartbeat: 0};
const result = window.productionResult;
(async () => {
  let worker;
  let heartbeat;
  let watchdog;
  let stopTimer;
  const finish = () => {
    clearInterval(heartbeat);
    clearTimeout(watchdog);
    clearTimeout(stopTimer);
    worker?.terminate();
    document.querySelector('#canvas')?.remove();
    result.cleanupRequested = true;
    result.fullTeardownVerified = false;
    result.done = true;
  };
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
    heartbeat = setInterval(() => ++result.heartbeat, 20);
    watchdog = setTimeout(() => { result.errors.push('startup/stop timeout'); finish(); }, 15000);
    worker = new Worker('./event-worker-runtime.js');
    worker.onerror = event => { result.errors.push(event.message); finish(); };
    worker.onmessage = event => {
      const {kind, value} = event.data;
      if (kind === 'environment') result.workerEnvironment = value;
      else if (kind === 'sharedMemory') result.sharedMemory = value;
      else if (kind === 'status') {
        result.events.push(value);
        if (value.state === 'ready') {
          stopTimer = setTimeout(() => worker.postMessage({command: 'stop'}), 1000);
        }
        if (value.state === 'stopped') finish();
      } else {
        result.errors.push(value);
        if (kind === 'exception') finish();
      }
    };
    const canvas = document.querySelector('#canvas').transferControlToOffscreen();
    worker.postMessage({command: 'start', canvas}, [canvas]);
  } catch (error) {
    result.errors.push(String(error));
    finish();
  }
})();
