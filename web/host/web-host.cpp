// Minimal production D3 host: browser main owns Core events; one pthread owns
// update/upload/render. Mailboxes use release/acquire and never block main.
#include "web-graphics.h"
#include <dali/integration-api/core.h>
#include <dali/integration-api/scene-pre-render-status.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/render-controller.h>
#include <dali/public-api/dali-core.h>
#include <dali/internal/graphics/web/web-gles-fence.h>
#include <emscripten/emscripten.h>
#include <emscripten/threading.h>
#include <atomic>
#include <cerrno>
#include <map>
#include <memory>
#include <new>

EM_JS(void, NotifyStatus, (const char* state, const char* reason), {
  const event = {state: UTF8ToString(state), reason: reason ? UTF8ToString(reason) : null};
  try { if (Module['onDaliStatus']) Module['onDaliStatus'](event); }
  catch (error)
  {
    console.error(error);
  }
});

EM_JS_DEPS(dali_web_worker_dependencies, "$PThread");
EM_JS(void, DisposeWorkers, (), { PThread.terminateAllThreads(); });


namespace Dali::Web
{
class Host;
Host* instance = nullptr;
bool executionStarted = false;
using Graphics::GLES::WebFailure;
using Graphics::GLES::WebFence;

class Platform : public Integration::PlatformAbstraction
{
  struct Timer
  {
    Platform* owner;
    uint32_t id;
    long timeout;
    std::unique_ptr<CallbackBase> callback;
  };
public:
  uint32_t StartTimer(uint32_t milliseconds, CallbackBase* callback) override
  {
    DALI_ASSERT_ALWAYS(emscripten_is_main_runtime_thread());
    auto timer = std::make_unique<Timer>();
    timer->owner = this;
    timer->id = ++mNextId;
    timer->callback.reset(callback);
    const auto id = timer->id;
    timer->timeout = emscripten_set_timeout([](void* ptr)
    {
      auto* value = static_cast<Timer*>(ptr);
      auto* owner = value->owner;
      auto timer = std::move(owner->mTimers.at(value->id));
      owner->mTimers.erase(value->id);
      CallbackBase::Execute(*timer->callback);
    }, milliseconds, timer.get());
    mTimers.emplace(id, std::move(timer));
    return id;
  }
  void CancelTimer(uint32_t id) override
  {
    DALI_ASSERT_ALWAYS(emscripten_is_main_runtime_thread());
    auto iter = mTimers.find(id);
    if(iter != mTimers.end())
    {
      emscripten_clear_timeout(iter->second->timeout);
      mTimers.erase(iter);
    }
  }
  void CancelAll()
  {
    while(!mTimers.empty()) CancelTimer(mTimers.begin()->first);
  }
  ~Platform() override
  {
    CancelAll();
  }
private:
  uint32_t mNextId{0};
  std::map<uint32_t, std::unique_ptr<Timer>> mTimers;
};

class Sync : public Integration::GraphicsSyncAbstraction
{
  class Fence : public SyncObject
  {
  public:
    bool IsSynced() override
    {
      return fence.Poll();
    }
    void Wait() override
    {
      WebFailure::Latch("cross-context GPU wait unsupported");
    }
    void ClientWait() override
    {
      WebFailure::Latch("blocking client wait unsupported; poll the fence");
    }
    WebFence fence;
  };
public:
  SyncObject* CreateSyncObject(SyncObject::SyncType type) override
  {
    if(type != SyncObject::SyncType::FENCE_SYNC)
    {
      WebFailure::Latch("native fence unsupported");
      return nullptr;
    }
    auto* object = new(std::nothrow) Fence;
    if(!object || !object->fence.Insert())
    {
      delete object;
      WebFailure::Latch("host fence creation failed");
      return nullptr;
    }
    return object;
  }
  void DestroySyncObject(SyncObject* object) override
  {
    delete static_cast<Fence*>(object);
  }
};

class Host : public Integration::RenderController
{
  enum class Command { None, Frame, Shutdown, Exit };
  enum class Reply { None, Ready, FrameDone, ResourcesStopped, Exited };
public:
  Host() : mSurface(mController), mGraphics(mController, mGl, mSurface) {}
  void RequestUpdate() override
  {
    mNeedsEvents = true;
  }
  void RequestProcessEventsOnIdle() override
  {
    mNeedsEvents = true;
  }
  bool Start()
  {
    DALI_ASSERT_ALWAYS(emscripten_is_main_runtime_thread());
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    const int transfer = emscripten_pthread_attr_settransferredcanvases(&attributes, "#canvas");
    const int result = transfer ? transfer : pthread_create(&mWorker, &attributes, WorkerEntry, this);
    pthread_attr_destroy(&attributes);
    if(result)
    {
      NotifyStatus("failure", "render pthread/Canvas transfer failed");
      return false;
    }
    // This is a nonblocking mailbox pump, not a readiness wait or join loop.
    // It remains scheduled when rAF is suspended, so stop can still complete.
    mMainTimer = emscripten_set_interval(MainPump, 8, this);
    return true;
  }
  void Stop()
  {
    DALI_ASSERT_ALWAYS(emscripten_is_main_runtime_thread());
    mStopRequested = true;
    if(mRaf)
    {
      emscripten_cancel_animation_frame(mRaf);
      mRaf = 0;
    }
    if(mReady && !mInFlight && !mStopping) BeginStop();
  }
private:
  static void* WorkerEntry(void* data)
  {
    auto& self = *static_cast<Host*>(data);
    if(self.mSurface.Open() && self.mGl.ContextCreated())
    {
      self.mController.InitializeGLES(self.mGl);
      self.mController.Initialize(self.mSync, self.mGraphics);
      self.mController.SetGLESVersion(Graphics::GLES::GLESVersion::GLES_30);
      self.mSurface.Resize({320, 240});
      self.mGraphics.SetInitialized(true);
      self.mGraphics.ActivateResourceContext();
      self.mGraphicsInitialized = true;
    }
    self.PublishFailure();
    self.mWorkerTimer = emscripten_set_interval(WorkerPump, 4, &self);
    self.mReply.store(Reply::Ready, std::memory_order_release);
    emscripten_exit_with_live_runtime();
  }
  void PublishFailure()
  {
    if(const char* reason = WebFailure::Reason())
    {
      const char* expected = nullptr;
      mFailure.compare_exchange_strong(expected, reason);
    }
  }
  static void WorkerPump(void* data)
  {
    auto& self = *static_cast<Host*>(data);
    const auto command = self.mCommand.exchange(Command::None, std::memory_order_acquire);
    if(command == Command::Frame)
    {
      if(!WebFailure::HasFailure() && !WebFailure::IsStopped()) self.RenderFrame();
      self.PublishFailure();
      self.mReply.store(Reply::FrameDone, std::memory_order_release);
    }
    else if(command == Command::Shutdown)
    {
      if(self.mCore)
      {
        // Consume event-side removals and both update buffers. No new rendering.
        Integration::UpdateStatus update;
        self.mCore->Update(0, 0, 0, update, false, false, false);
        self.mCore->Update(0, 0, 0, update, false, false, false);
        self.mCore->ContextDestroyed();
      }
      if(self.mGraphicsInitialized) self.mGraphics.Shutdown();
      WebFence::Shutdown();
      WebFailure::Stop();
      self.PublishFailure();
      self.mReply.store(Reply::ResourcesStopped, std::memory_order_release);
    }
    else if(command == Command::Exit)
    {
      // Main has now destroyed Core/TLS and its late resource handles. The
      // render owner cancels remaining fences before disposing of the context.
      WebFence::Shutdown();
      self.mSurface.Close();
      emscripten_clear_interval(self.mWorkerTimer);
      self.mReply.store(Reply::Exited, std::memory_order_release);
      pthread_exit(nullptr);
    }
  }
  void CreateScene()
  {
    mCore = Integration::Core::New(*this, mPlatform, mController, Integration::CorePolicyFlags::DEFAULT);
    mCore->Initialize();
    Graphics::RenderTargetCreateInfo target{};
    target.SetSurface(&mSurface).SetExtent({320, 240}).SetPreTransform(0 | Graphics::RenderTargetTransformFlagBits::TRANSFORM_IDENTITY_BIT);
    mScene = Integration::Scene::New(target, Size(320, 240));
    mScene.SetDpi(Vector2(96, 96));
    mScene.SetBackgroundColor(Vector4(0.05f, 0.1f, 0.2f, 1.0f));
    Property::Map format;
    format["aPosition"] = Property::VECTOR2;
    auto vertices = VertexBuffer::New(format);
    Vector2 positions[] = {Vector2(-0.5f, -0.5f), Vector2(0.5f, -0.5f), Vector2(-0.5f, 0.5f), Vector2(0.5f, 0.5f)};
    vertices.SetData(positions, 4);
    auto geometry = Geometry::New();
    geometry.AddVertexBuffer(vertices);
    geometry.SetType(Geometry::TRIANGLE_STRIP);
    auto shader = Shader::New("attribute vec2 aPosition; uniform mat4 uMvpMatrix; uniform vec3 uSize; void main(){gl_Position=uMvpMatrix*vec4(aPosition*uSize.xy,0.0,1.0);}",
      "precision mediump float; void main(){gl_FragColor=vec4(0.9,0.3,0.1,1.0);}");
    auto actor = Actor::New();
    actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
    actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    actor.SetProperty(Actor::Property::PIVOT, Vector3(0.5f, 0.5f, 0.5f));
    auto renderer = Renderer::New(geometry, shader);
    actor.AddRenderer(renderer);
    mScene.Add(actor);
    mCore->SceneCreated();
    mCore->ProcessEvents();
  }
  void RenderFrame()
  {
    if(!mContextAnnounced)
    {
      mCore->ContextCreated();
      mContextAnnounced = true;
    }
    const double delta = mLastTimestamp ? (mTimestamp - mLastTimestamp) * 0.001 : 0;
    mLastTimestamp = mTimestamp;
    Integration::UpdateStatus update;
    Integration::RenderStatus render;
    Integration::ScenePreRenderStatus sceneStatus;
    mGraphics.FrameStart();
    mCore->Update(static_cast<float>(delta), static_cast<uint32_t>(mTimestamp), static_cast<uint32_t>(mTimestamp + 16), update, false, false, false);
    mGraphics.ActivateResourceContext();
    mCore->PreRender(render, false);
    mGraphics.RenderStart();
    std::vector<BoundsInteger> damage;
    mCore->PreRenderScene(mScene, sceneStatus, damage);
    BoundsInteger clip;
    if(mSurface.PreRender(false, damage, clip) && mController.CanSubmit())
    {
      mCore->RenderScene(render, mScene, true);
      if(mController.CanSubmit()) mCore->RenderScene(render, mScene, false);
    }
    mGraphics.PostRender();
    mCore->PostRender();
    WebFence::Collect();
    WebFailure::CheckGl();
  }
  static EM_BOOL AnimationFrame(double timestamp, void* data)
  {
    auto& self = *static_cast<Host*>(data);
    self.mRaf = 0;
    if(self.mStopRequested || self.mStopping) return EM_FALSE;
    self.mCore->ProcessEvents();
    self.mNeedsEvents = false;
    self.mTimestamp = timestamp;
    self.mInFlight = true;
    self.mCommand.store(Command::Frame, std::memory_order_release);
    return EM_FALSE;
  }
  void ScheduleFrame()
  {
    if(!mRaf && !mInFlight && !mStopRequested)
      mRaf = emscripten_request_animation_frame(AnimationFrame, this);
  }
  void BeginStop()
  {
    mStopping = true;
    mPlatform.CancelAll();
    if(mCore)
    {
      mCore->SceneDestroyed();
      mScene.RemoveSceneObject();
      mScene.Reset();
      mCore->ProcessEvents();
    }
    mPlatform.CancelAll();
    mCommand.store(Command::Shutdown, std::memory_order_release);
  }
  static void MainPump(void* data)
  {
    auto& self = *static_cast<Host*>(data);
    if(const char* reason = self.mFailure.load(std::memory_order_acquire); reason && !self.mFailureNotified)
    {
      self.mFailureNotified = true;
      NotifyStatus("failure", reason);
      self.Stop();
    }
    auto reply = self.mReply.exchange(Reply::None, std::memory_order_acquire);
    if(reply == Reply::Ready)
    {
      self.mReady = true;
      if(self.mFailure.load(std::memory_order_acquire)) self.mStopRequested = true;
      if(!self.mStopRequested)
      {
        self.CreateScene();
        NotifyStatus("ready", nullptr);
        self.ScheduleFrame();
      }
      else self.BeginStop();
    }
    else if(reply == Reply::FrameDone)
    {
      self.mInFlight = false;
      if(self.mStopRequested) self.BeginStop();
      else self.ScheduleFrame();
    }
    else if(reply == Reply::ResourcesStopped)
    {
      delete self.mCore;
      self.mCore = nullptr;
      self.mCommand.store(Command::Exit, std::memory_order_release);
    }
    else if(reply == Reply::Exited) self.mExited = true;
    if(self.mReady && self.mCore && self.mNeedsEvents && !self.mInFlight && !self.mStopping)
    {
      self.mNeedsEvents = false;
      self.mCore->ProcessEvents();
    }
    if(self.mExited)
    {
      const int result = pthread_tryjoin_np(self.mWorker, nullptr);
      if(result == EBUSY) return;
      if(result != 0)
      {
        if(!self.mFailureNotified) NotifyStatus("failure", "pthread reap failed");
        self.mFailureNotified = true;
        return;
      }
      emscripten_clear_interval(self.mMainTimer);
      instance = nullptr;
      delete &self;
      DisposeWorkers();
      NotifyStatus("stopped", nullptr);
    }
  }
  Graphics::GlesGraphicsController mController;
  Graphics::GLES::WebGlImplementation mGl;
  Surface mSurface;
  GraphicsHost mGraphics;
  Sync mSync;
  Platform mPlatform;
  Integration::Core* mCore{nullptr};
  Integration::Scene mScene;
  pthread_t mWorker{};
  std::atomic<Command> mCommand{Command::None};
  std::atomic<Reply> mReply{Reply::None};
  std::atomic<const char*> mFailure{nullptr};
  double mTimestamp{0};
  double mLastTimestamp{0};
  long mMainTimer{0};
  long mWorkerTimer{0};
  long mRaf{0};
  bool mGraphicsInitialized{false}; // Worker only.
  bool mContextAnnounced{false}; // Worker only.
  bool mReady{false}; // Remaining flags are browser-main only.
  bool mNeedsEvents{false};
  bool mInFlight{false};
  bool mStopRequested{false};
  bool mStopping{false};
  bool mFailureNotified{false};
  bool mExited{false};
};
}
extern "C" EMSCRIPTEN_KEEPALIVE void dali_web_start()
{
  DALI_ASSERT_ALWAYS(emscripten_is_main_runtime_thread());
  if(Dali::Web::executionStarted)
  {
    NotifyStatus("failure", "new execution instance required"); return;
  }
  Dali::Web::executionStarted = true;
  Dali::Web::instance = new(std::nothrow) Dali::Web::Host;
  if(!Dali::Web::instance)
  {
    NotifyStatus("failure", "host allocation failed"); DisposeWorkers(); NotifyStatus("stopped", nullptr); return;
  }
  if(!Dali::Web::instance->Start())
  {
    delete Dali::Web::instance;
    Dali::Web::instance = nullptr;
    DisposeWorkers();
    NotifyStatus("stopped", "new execution instance required");
  }
}
extern "C" EMSCRIPTEN_KEEPALIVE void dali_web_stop()
{
  if(Dali::Web::instance) Dali::Web::instance->Stop();
}
