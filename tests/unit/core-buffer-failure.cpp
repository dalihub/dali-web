// CPU failure-injection tests. These mocks are never part of production links.
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <limits>
#include <dali/public-api/dali-core.h>
#include <dali/devel-api/signals/vertex-buffer-update-callback.h>
#include <dali/internal/render/renderers/gpu-buffer.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/render/renderers/render-vertex-buffer.h>
#include <dali/internal/render/renderers/uniform-buffer.h>
#include <dali/internal/render/renderers/uniform-buffer-view.h>
#include <test-application.h>
#include <test-graphics-buffer.h>
#include <test-graphics-command-buffer.h>

extern "C" void tet_infoline(const char* text)
{
  puts(text);
}
extern "C" void tet_printf(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}

using namespace Dali;
namespace R = Dali::Internal::Render;

class FailedLock : public Graphics::Memory
{
public:
  void* LockRegion(uint32_t, uint32_t) override
  {
    return nullptr;
  }
  void Unlock(bool) override
  {
  }
  void Flush() override
  {
  }
};

class FaultController : public TestGraphicsController
{
public:
  enum class Failure
  {
    NONE,
    CREATE,
    MAP,
    LOCK,
    RANGE
  };
  Failure failure{Failure::NONE};
  bool uniformsOnly{false};
  unsigned maps{0};
  unsigned unmaps{0};
  unsigned submissions{0};
  unsigned syncCreations{0};

  bool Affects(Graphics::BufferUsageFlags usage) const
  {
    return !uniformsOnly || (usage & (0u | Graphics::BufferUsage::UNIFORM_BUFFER));
  }
  Graphics::UniquePtr<Graphics::Buffer> CreateBuffer(const Graphics::BufferCreateInfo& info, Graphics::UniquePtr<Graphics::Buffer>&& old) override
  {
    if(failure == Failure::CREATE && Affects(info.usage))
    {
      old.reset();
      return nullptr;
    }
    return TestGraphicsController::CreateBuffer(info, std::move(old));
  }
  Graphics::UniquePtr<Graphics::Memory> MapBufferRange(const Graphics::MapBufferInfo& info) override
  {
    ++maps;
    assert(info.buffer);
    if(Affects(static_cast<TestGraphicsBuffer*>(info.buffer)->mUsage))
    {
      if(failure == Failure::MAP)
      {
        return nullptr;
      }
      if(failure == Failure::LOCK)
      {
        return std::make_unique<FailedLock>();
      }
    }
    return TestGraphicsController::MapBufferRange(info);
  }
  void UnmapMemory(Graphics::UniquePtr<Graphics::Memory> memory) override
  {
    ++unmaps;
    TestGraphicsController::UnmapMemory(std::move(memory));
  }
  const Graphics::Reflection& GetProgramReflection(const Graphics::Program& program) override
  {
    const auto& reflection = TestGraphicsController::GetProgramReflection(program);
    if(failure == Failure::RANGE)
    {
      // Map succeeds, but a matrix write exceeds its declared view during recording.
      auto& test = const_cast<TestGraphicsReflection&>(static_cast<const TestGraphicsReflection&>(reflection));
      test.mUniformBlocks[0].size = 4;
    }
    return reflection;
  }
  Graphics::UniquePtr<Graphics::SyncObject> CreateSyncObject(const Graphics::SyncObjectCreateInfo& info, Graphics::UniquePtr<Graphics::SyncObject>&& old) override
  {
    ++syncCreations;
    return TestGraphicsController::CreateSyncObject(info, std::move(old));
  }
  void SubmitCommandBuffers(const Graphics::SubmitInfo& info) override
  {
    submissions += !info.cmdBuffer.empty();
    TestGraphicsController::SubmitCommandBuffers(info);
  }
};

struct Writer
{
  unsigned calls{0};
  uint32_t count{0};
  uint32_t Write(void* data, size_t size)
  {
    assert(data && size == 16);
    ++calls;
    memset(data, 0x55, 4);
    return count;
  }
};

void CheckBuffers(FaultController& controller)
{
  uint8_t pattern[16];
  for(unsigned i = 0; i < 16; ++i)
  {
    pattern[i] = i;
  }
  for(auto failure : {FaultController::Failure::CREATE, FaultController::Failure::MAP, FaultController::Failure::LOCK})
  {
    Internal::GpuBuffer buffer(controller, 0u | Graphics::BufferUsage::VERTEX_BUFFER, Internal::GpuBuffer::WritePolicy::RETAIN);
    controller.failure = failure;
    const auto unmaps = controller.unmaps;
    assert(!buffer.UpdateDataBuffer(controller, 16, pattern));
    assert(!buffer.GetGraphicsObject() && buffer.GetBufferSize() == 0);
    assert(controller.unmaps == unmaps + (failure == FaultController::Failure::LOCK));
    controller.failure = FaultController::Failure::NONE;
    assert(buffer.UpdateDataBuffer(controller, 16, pattern));
    assert(buffer.GetGraphicsObject() && buffer.GetBufferSize() == 16);
    Writer writer;
    auto callback = VertexBufferUpdateCallback::New(&writer, &Writer::Write);
    uint32_t count = 123;
    controller.failure = FaultController::Failure::MAP;
    assert(!buffer.UpdateDataBufferWithCallback(controller, callback.Get(), count));
    assert(writer.calls == 0 && count == 0 && !buffer.GetGraphicsObject());
    controller.failure = FaultController::Failure::LOCK;
    assert(!buffer.UpdateDataBufferWithCallback(controller, callback.Get(), count));
    assert(writer.calls == 0);
    controller.failure = FaultController::Failure::NONE;
    for(auto bytes : {0u, 4u, 16u})
    {
      writer.count = bytes;
      assert(buffer.UpdateDataBufferWithCallback(controller, callback.Get(), count));
      assert(count == bytes && buffer.GetGraphicsObject());
    }
    writer.count = 17;
    assert(!buffer.UpdateDataBufferWithCallback(controller, callback.Get(), count));
    assert(count == 0 && !buffer.GetGraphicsObject());
  }
  puts("PASS buffer create/map/lock failure and callback zero/partial/full/invalid count");
}

void CheckGeometry(FaultController& controller)
{
  for(auto failure : {FaultController::Failure::CREATE, FaultController::Failure::MAP, FaultController::Failure::LOCK})
  {
    R::VertexBuffer vertex;
    Internal::OwnerPointer<R::VertexBuffer::Format> format(new R::VertexBuffer::Format);
    format->size = 4;
    vertex.SetFormat(format);
    Internal::OwnerPointer<Dali::Vector<uint8_t>> data(new Dali::Vector<uint8_t>);
    data->Resize(16);
    vertex.SetData(data, 4);
    controller.failure = failure;
    assert(!vertex.Update(controller) && vertex.IsDataChanged());
    controller.failure = FaultController::Failure::NONE;
    assert(vertex.Update(controller) && !vertex.IsDataChanged());
    Writer writer;
    auto callback = VertexBufferUpdateCallback::New(&writer, &Writer::Write);
    vertex.SetVertexBufferUpdateCallback(callback.Release());
    controller.failure = failure;
    assert(!vertex.Update(controller) && writer.calls == 0);
    // Failure must release the callback lock: this replacement must return.
    vertex.SetVertexBufferUpdateCallback(nullptr);
    controller.failure = FaultController::Failure::NONE;
    R::Geometry geometry;
    geometry.AddVertexBuffer(&vertex);
    R::Geometry::Uint16ContainerType indices;
    indices.PushBack(0);
    indices.PushBack(1);
    indices.PushBack(2);
    geometry.SetIndexBuffer(indices);
    auto commands = controller.CreateCommandBuffer(Graphics::CommandBufferCreateInfo{}, nullptr);
    controller.failure = failure;
    geometry.Upload(controller);
    assert(!geometry.BindVertexAttributes(*commands));
    assert(!geometry.Draw(controller, *commands, 0, 0, 1));
    assert(static_cast<TestGraphicsCommandBuffer*>(commands.get())->GetCommands().empty());
    // The failed index upload must remain dirty and be attempted again.
    controller.failure = FaultController::Failure::NONE;
    // Restore valid vertex data after its previous failed callback.
    Internal::OwnerPointer<Dali::Vector<uint8_t>> replacement(new Dali::Vector<uint8_t>);
    replacement->Resize(16);
    vertex.SetData(replacement, 4);
    geometry.Upload(controller);
    assert(geometry.BindVertexAttributes(*commands));
    assert(geometry.Draw(controller, *commands, 0, 0, 1));
  }
  puts("PASS vertex dirty/lock release and index failure blocks bind/draw including cached path");
}

void CheckUniforms(FaultController& controller)
{
  uint32_t data = 0x12345678;
  for(bool cpu : {true, false})
  {
    for(auto failure : {FaultController::Failure::CREATE, FaultController::Failure::MAP, FaultController::Failure::LOCK})
    {
      auto ubo = R::UniformBufferV2::New(&controller, cpu, 16);
      controller.failure = failure;
      ubo->ReSpecify(32);
      assert(!ubo->IsValid());
      assert(!ubo->Write(&data, 4, 4));
      assert(!ubo->MemoryCompare(&data, 4, 4));
      assert(!ubo->Flush());
      controller.failure = FaultController::Failure::NONE;
      ubo->ReSpecify(32);
      assert(ubo->IsValid());
      assert(ubo->Write(&data, 4, 4));
      assert(ubo->MemoryCompare(&data, 4, 4));
      assert(ubo->IncrementOffsetBy(4) && ubo->GetCurrentOffset() == 16);
      assert(!ubo->IncrementOffsetBy(UINT32_MAX));
      assert(ubo->GetCurrentOffset() == 16 && !ubo->Flush());
      ubo->ReSpecify(32);
      auto* view = R::UniformBufferView::New(*ubo, 8, 8);
      assert(view->Write(&data, 4, 4));
      assert(!view->Write(&data, 4, 8));
      assert(!view->GetBuffer() && !ubo->Flush());
      delete view;
      ubo->ReSpecify(32);
      assert(!ubo->Write(&data, 4, UINT32_MAX));
      assert(!ubo->Flush());
    }
  }
  controller.failure = FaultController::Failure::NONE;
  puts("PASS CPU/GPU uniform failures, explicit recovery, view bounds and offset overflow");
}

class FaultApplication : public TestApplication
{
public:
  explicit FaultApplication(FaultController& controller)
  : TestApplication(480, 800, 220, 217, false)
  {
    controller.Initialize();
    mCore = Integration::Core::New(mRenderController, mPlatformAbstraction, controller, Integration::CorePolicyFlags::DEFAULT);
    mCore->ContextCreated();
    CreateScene();
    InitializeCore();
  }
};

void CheckSceneSubmission()
{
  for(auto failure : {FaultController::Failure::NONE, FaultController::Failure::CREATE, FaultController::Failure::MAP, FaultController::Failure::LOCK, FaultController::Failure::RANGE})
  {
    FaultController controller;
    FaultApplication app(controller);
    controller.uniformsOnly = true;
    controller.failure = failure;
    Property::Map format;
    format["aPosition"] = Property::VECTOR2;
    auto vertex = Dali::VertexBuffer::New(format);
    Vector2 vertices[] = {Vector2(0, 0), Vector2(1, 0), Vector2(0, 1)};
    vertex.SetData(vertices, 3);
    auto geometry = Dali::Geometry::New();
    geometry.AddVertexBuffer(vertex);
    auto shader = Shader::New("attribute vec2 aPosition; uniform mat4 uMvpMatrix; void main(){gl_Position=uMvpMatrix*vec4(aPosition,0.0,1.0);}", "void main(){gl_FragColor=vec4(1.0);}");
    auto actor = Actor::New();
    actor.SetProperty(Actor::Property::SIZE, Vector2(100, 100));
    auto renderer = Renderer::New(geometry, shader);
    actor.AddRenderer(renderer);
    app.GetScene().Add(actor);
    auto texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 16, 16);
    auto framebuffer = FrameBuffer::New(16, 16);
    framebuffer.AttachColorTexture(texture);
    auto tasks = app.GetScene().GetRenderTaskList();
    auto task = tasks.CreateTask();
    task.SetSourceActor(actor);
    task.SetCameraActor(tasks.GetTask(0).GetCameraActor());
    task.SetFrameBuffer(framebuffer);
    task.SetRefreshRate(RenderTask::REFRESH_ONCE);
    task.SetProperty(RenderTask::Property::REQUIRES_SYNC, true);
    task.KeepRenderResult();
    app.SendNotification();
    controller.submissions = 0;
    app.Render();
    assert((controller.submissions > 0) == (failure == FaultController::Failure::NONE));
    assert((controller.syncCreations > 0) == (failure == FaultController::Failure::NONE));
    app.SendNotification();
    if(failure != FaultController::Failure::NONE)
    {
      assert(!task.GetRenderResult());
    }
  }
  puts("PASS real Core scene blocks submission/sync/readback on uniform create/map/lock/range failure (mock graphics)");
}

int main()
{
  {
    TestApplication pools;
    FaultController controller;
    controller.Initialize();
    CheckBuffers(controller);
    CheckGeometry(controller);
    CheckUniforms(controller);
  }
  CheckSceneSubmission();
  puts("PASS Core buffer failure suite; no GPU/runtime compatibility claim");
}
