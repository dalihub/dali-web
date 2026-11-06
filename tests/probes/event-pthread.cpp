// Separate C candidate. Reuse the production host/renderer, change only ownership.
#define DALI_WEB_TEST_EVENT_PTHREAD
#define DALI_WEB_TEST_READBACK
#include "../../web/host/web-host.cpp"

namespace
{
void* EventEntry(void*)
{
  eventProbeOwner = pthread_self();
  eventProbe[EventThread] = static_cast<int>(pthread_self());
  DALI_ASSERT_ALWAYS(!emscripten_is_main_runtime_thread());
  DALI_ASSERT_ALWAYS(!emscripten_is_main_browser_thread());
  DALI_ASSERT_ALWAYS(!Dali::Internal::ThreadLocalStorage::Created());
  dali_web_start();
  emscripten_exit_with_live_runtime();
}
}
extern "C" EMSCRIPTEN_KEEPALIVE int probe_start(int mode)
{
  DALI_ASSERT_ALWAYS(emscripten_is_main_browser_thread());
  DALI_ASSERT_ALWAYS(!Dali::Internal::ThreadLocalStorage::Created());
  eventProbe[MainThread] = static_cast<int>(pthread_self());
  if(eventProbeStarted) return EALREADY;
  eventProbeStarted = true;
  eventProbe[Mode] = mode;
  pthread_attr_t attributes;
  pthread_attr_init(&attributes);
  int result = emscripten_pthread_attr_settransferredcanvases(&attributes, "#canvas");
  if(!result) result = pthread_create(&eventProbeThread, &attributes, EventEntry, nullptr);
  pthread_attr_destroy(&attributes);
  return result;
}
extern "C" EMSCRIPTEN_KEEPALIVE int* probe_data()
{
  static_assert(sizeof(std::atomic<int>) == sizeof(int));
  return reinterpret_cast<int*>(eventProbe);
}
extern "C" EMSCRIPTEN_KEEPALIVE int probe_join()
{
  DALI_ASSERT_ALWAYS(emscripten_is_main_browser_thread());
  if(!eventProbe[EventExited]) return EBUSY;
  return pthread_tryjoin_np(eventProbeThread, nullptr);
}
EM_JS_DEPS(probe_dependencies, "$PThread");
EM_JS(int, ProbeWorkerCount, (), {
  return new Set([...PThread.runningWorkers, ...PThread.unusedWorkers]).size;
});
EM_JS(int, ProbeDispose, (int force), {
  const workers = new Set([...PThread.runningWorkers, ...PThread.unusedWorkers]);
  if (force) {
    // Do not free pthread data or acquire allocator locks in a hung runtime.
    // The supervisor discards this entire Module after terminating every Worker.
    for (const worker of workers) worker.terminate();
  } else {
    PThread.terminateAllThreads();
  }
  return workers.size;
});
EM_JS(void, ProbeListen, (), {
  for (const worker of new Set([...PThread.runningWorkers, ...PThread.unusedWorkers])) {
    worker.addEventListener('message', event => {
      if (event.data.daliProbe === true) Module['onProbe'](event.data);
    });
  }
});

extern "C" EMSCRIPTEN_KEEPALIVE int probe_worker_count()
{
  return ProbeWorkerCount();
}
extern "C" EMSCRIPTEN_KEEPALIVE int probe_dispose(int force)
{
  return ProbeDispose(force);
}
extern "C" EMSCRIPTEN_KEEPALIVE void probe_listen()
{
  ProbeListen();
}
