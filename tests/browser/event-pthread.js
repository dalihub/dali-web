// Independent C gate. Each generation owns one Module and all its pthreads.
window.productionResult = {done: false, errors: [], runs: []};
const result = window.productionResult;
const F = {mode: 0, event: 1, render: 2, tls: 3, removed: 4, timer: 5,
  trigger: 6, eventTicks: 7, renderTicks: 8, spinning: 9, spinTicks: 10,
  closed: 11, exited: 12, stop: 13, request: 14, time: 15, tick: 16, key: 17, keyObserved: 18, main: 19};
const delay = ms => new Promise(resolve => setTimeout(resolve, ms));
async function until(test, label, timeout = 6000) {
  const deadline = performance.now() + timeout;
  while (!test()) {
    if (performance.now() > deadline) throw new Error(label + ' timeout');
    await delay(20);
  }
}
function require(value, label) { if (!value) throw new Error(label); }
let generation = 0;
let active = null;
let beats = 0;
const heartbeat = setInterval(() => ++beats, 20);
addEventListener('error', event => result.errors.push(event.message));
addEventListener('unhandledrejection', event => result.errors.push(String(event.reason)));
addEventListener('keydown', event => {
  if (active) Atomics.store(active.data, F.key, event.keyCode);
});
document.querySelector('#stop').onclick = () => {
  if (!active) return;
  active.clicks++;
  if (active.mode) active.kill();
  else Atomics.store(active.data, F.stop, 1);
};
(async () => {
  let previousMemory;
  let previousCanvas;
  result.freshMemory = true;
  try {
    const sw = new URLSearchParams(location.search).get('sw') === '1';
    if (sw && !crossOriginIsolated) {
      sessionStorage.setItem('firstIsolation', String(crossOriginIsolated));
      if (sessionStorage.getItem('reloaded')) throw new Error('SW reload did not isolate');
      await navigator.serviceWorker.register('./production-sw.js');
      await navigator.serviceWorker.ready;
      sessionStorage.setItem('reloaded', '1');
      result.reloading = true;
      location.reload();
      return;
    }
    result.isolation = crossOriginIsolated;
    result.controlled = !!navigator.serviceWorker.controller;
    result.firstIsolation = sessionStorage.getItem('firstIsolation');
    for (const mode of [0, 1, 2, 0]) {
      const id = ++generation;
      const run = {id, mode, events: [], pixels: [], errors: [], clicks: 0, lateMessages: 0};
      result.runs.push(run);
      const canvas = document.createElement('canvas');
      canvas.id = 'canvas'; canvas.width = 320; canvas.height = 240;
      document.querySelector('#surface').append(canvas);
      let live = true;
      const receive = message => {
        if (!live || generation !== id) { run.lateMessages++; return; }
        if (message.kind === 'status') run.events.push(message.value);
        if (message.kind === 'pixels') run.pixels.push(message.value);
      };
      const module = await createDaliWeb({canvas,
        locateFile: name => new URL('./runtime/' + name, location.href).href,
        mainScriptUrlOrBlob: new URL('./runtime/dali-web.js', location.href).href,
        printErr: line => run.errors.push(String(line)),
        onAbort: reason => run.errors.push(String(reason)), onProbe: receive});
      require(module.HEAPU8.buffer instanceof SharedArrayBuffer, 'shared memory');
      result.freshMemory &&= previousMemory !== module.HEAPU8.buffer && previousCanvas !== canvas;
      previousMemory = module.HEAPU8.buffer; previousCanvas = canvas;
      const data = new Int32Array(module.HEAPU8.buffer, module._probe_data(), 20);
      const read = field => Atomics.load(data, F[field]);
      module._probe_listen();
      require(module._probe_worker_count() === 2, 'two preloaded pthread Workers');
      let raf;
      const tick = timestamp => {
        if (!live) return;
        Atomics.store(data, F.time, Math.floor(timestamp));
        Atomics.store(data, F.tick, 1);
        raf = requestAnimationFrame(tick);
      };
      const kill = (force = mode !== 0) => {
        if (!live) return;
        live = false; cancelAnimationFrame(raf);
        run.workersBefore = module._probe_worker_count();
        run.terminatedWorkers = module._probe_dispose(force ? 1 : 0);
        canvas.remove();
        run.canvasRemoved = !canvas.isConnected;
      };
      active = {mode, data, kill, get clicks() { return run.clicks; }, set clicks(v) { run.clicks = v; }};
      raf = requestAnimationFrame(tick);
      require(module._probe_start(mode) === 0, 'event pthread start');
      if (mode !== 1) {
        await until(() => run.pixels.length === 2 && read('timer'), 'render/timer');
        dispatchEvent(new KeyboardEvent('keydown', {key: 'a', keyCode: 65}));
        await until(() => read('keyObserved') === 65, 'DALi input signal');
        Atomics.store(data, F.request, 1);
        await until(() => read('trigger'), 'event trigger');
      }
      if (mode) {
        await until(() => read('spinning'), 'infinite loop entered');
        const before = {beats, spin: read('spinTicks'), render: read('renderTicks')};
        await delay(600);
        run.hang = {heartbeatDelta: beats - before.beats,
          spinChanged: read('spinTicks') !== before.spin,
          renderChanged: read('renderTicks') !== before.render};
        require(run.hang.heartbeatDelta >= 10 && run.hang.spinChanged && run.hang.renderChanged, 'main/render alive during event infinite loop');
      }
      document.querySelector('#stop').click();
      if (!mode) {
        await until(() => read('exited'), 'event exit');
        await until(() => module._probe_join() === 0, 'event pthread reap');
        kill();
      }
      // Counter quiescence is separate from terminate requests and stopped events.
      await delay(150);
      const frozen = Array.from(data);
      await delay(350);
      run.countersStable = frozen.every((value, index) => value === Atomics.load(data, index));
      run.state = Array.from(data);
      run.eventOwner = read('event'); run.renderOwner = read('render');
      require(read('tls') === 1 && run.eventOwner !== 0 && run.renderOwner !== 0
        && run.eventOwner !== run.renderOwner && run.eventOwner !== read('main')
        && run.renderOwner !== read('main'), 'Core TLS and separate owners');
      require(run.countersStable && run.canvasRemoved && run.terminatedWorkers === 2
        && run.workersBefore === 2 && run.clicks === 1, 'whole instance stop');
      if (!mode) {
        require(read('removed') && read('closed') && read('exited'), 'TLS/context shutdown');
        require(run.events.map(e => e.state).join(',') === 'ready,stopped', 'normal status order');
      }
      if (mode !== 1) {
        const expected = [13, 26, 51, 255, 230, 77, 26, 255];
        require(run.pixels.length === 2 && run.pixels.every((frame, i) => frame.frame === i + 1
          && frame.error === 0 && frame.pixels.length === 8
          && frame.pixels.every((value, index) => Math.abs(value - expected[index]) <= 1)), 'actual DALi pixels');
      }
      // Inject a delayed old-generation message into the same receiver guard.
      const eventCount = run.events.length;
      receive({kind: 'status', value: {state: 'stale'}});
      result.staleMessageRejected = run.events.length === eventCount && run.lateMessages >= 1;
      require(run.errors.length === 0, run.errors.join('\n'));
      run.passed = true;
      active = null;
    }
  } catch (error) {
    result.errors.push(String(error) + "\n" + (error.stack || ""));
    active?.kill(true);
  } finally {
    clearInterval(heartbeat);
    if (!result.reloading) result.done = true;
  }
})();
