#pragma once
// Only compiled by the independent C test target. No production/runtime ABI.
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/public-api/signals/connection-tracker.h>

namespace
{
enum ProbeField
{
  Mode, EventThread, RenderThread, TlsCreated, TlsRemoved, TimerCalled,
  TriggerCalled, EventTicks, RenderTicks, Spinning, SpinTicks, ContextClosed,
  EventExited, StopRequest, TriggerRequest, Timestamp, TickPending, KeyRequest, KeyObserved, MainThread, FieldCount
};
std::atomic<int> eventProbe[FieldCount]{};
pthread_t eventProbeOwner{};
pthread_t eventProbeThread{};
bool eventProbeStarted = false;

void ProbeSpin()
{
  eventProbe[Spinning] = 1;
  for(;;) eventProbe[SpinTicks].fetch_add(1, std::memory_order_relaxed);
}
}

EM_JS(void, NotifyStatus, (const char* state, const char* reason), {
  postMessage({daliProbe: true, kind: 'status', value: {
    state: UTF8ToString(state), reason: reason ? UTF8ToString(reason) : null}});
});
// Only window may dispose the pool, after joining the event thread.
void DisposeWorkers() {}
