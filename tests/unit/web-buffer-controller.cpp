// CPU integration: real Controller, Buffer factory and Core uniforms, injected GL.
// This executable does not establish browser/GPU or production-link success.
#include <dali/internal/graphics/gles-impl/gles-graphics-controller.h>
#include <dali/internal/graphics/web/web-buffer-memory.h>
#include <dali/internal/graphics/web/web-gles-failure.h>
#include <dali/internal/render/renderers/uniform-buffer.h>
#include "web-memory-test-gl.h"
#include "web-memory-test-host.h"
#include <GLES3/gl3.h>
#include <emscripten/html5_webgl.h>
#include <cassert>
#include <cstring>
#include <map>
#include <vector>
#include <new>

namespace
{
size_t failObjectSize = 0;
bool failGen = false;
bool failData = false;
unsigned generations = 0;
unsigned deletions = 0;
unsigned uploads = 0;
unsigned objectFrees = 0;
GLuint bound = 0;
GLenum error = GL_NO_ERROR;
std::map<GLuint, std::vector<unsigned char>> bytes;
void* FailAllocate(size_t, size_t, void*)
{
  return nullptr;
}
void* Allocate(size_t size, size_t, void*)
{
  return std::malloc(size);
}
void Free(void* ptr, void*)
{
  ++objectFrees; std::free(ptr);
}
class SyncBoundary : public Dali::Integration::GraphicsSyncAbstraction
{
public:
  SyncObject* CreateSyncObject(SyncObject::SyncType) override
  {
    std::abort();
  }
  void DestroySyncObject(SyncObject*) override
  {
    std::abort();
  }
};
class GlResponses : public Dali::MemoryTestGl
{
public:
  unsigned viewports = 0;
  void GetIntegerv(Dali::GLenum name, Dali::GLint* value) override
  {
    assert(name == GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    *value = 16;
  }
  void Viewport(Dali::GLint, Dali::GLint, Dali::GLsizei, Dali::GLsizei) override
  {
    ++viewports;
  }
};
}
void* operator new(size_t size, const std::nothrow_t&) noexcept
{
  if(size == failObjectSize)
  {
    failObjectSize = 0; return nullptr;
  }
  return std::malloc(size);
}
extern "C"
{
void __wrap_glGenBuffers(GLsizei count, GLuint* ids)
{
  assert(count == 1);
  *ids = failGen ? 0 : ++generations;
}
void __wrap_glDeleteBuffers(GLsizei count, const GLuint* ids)
{
  assert(count == 1); ++deletions; bytes.erase(*ids);
}
void __wrap_glBindBuffer(GLenum target, GLuint id)
{
  assert(target == GL_COPY_WRITE_BUFFER); bound = id;
}
void __wrap_glBufferData(GLenum target, GLsizeiptr size, const void*, GLenum)
{
  assert(target == GL_COPY_WRITE_BUFFER && bound && size > 0);
  if(failData)
  {
    error = GL_OUT_OF_MEMORY; return;
  }
  bytes[bound].assign(size, 0xcc);
}
void __wrap_glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
{
  assert(target == GL_COPY_WRITE_BUFFER && bound && offset >= 0 && size > 0);
  assert(offset + size <= bytes[bound].size());
  ++uploads; std::memcpy(bytes[bound].data() + offset, data, size);
}
GLenum __wrap_glGetError()
{
  auto result = error; error = GL_NO_ERROR; return result;
}
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE __wrap_emscripten_webgl_get_current_context()
{
  return 7;
}
bool __wrap_emscripten_is_webgl_context_lost(EMSCRIPTEN_WEBGL_CONTEXT_HANDLE)
{
  return false;
}
}
int main(int argc, char** argv)
{
  using namespace Dali::Graphics;
  using namespace Dali::Graphics::GLES;
  const std::string mode = argc > 1 ? argv[1] : "factory";
  GlResponses gl;
  GlesGraphicsController controller;
  MemoryTestHost host(controller);
  SyncBoundary sync;
  controller.InitializeGLES(gl);
  controller.Initialize(sync, host);
  controller.ActivateResourceContext();
  BufferCreateInfo info{};
  info.SetSize(16).SetUsage(0 | BufferUsage::VERTEX_BUFFER);
  AllocationCallbacks allocator{};
  allocator.allocCallback = mode == "factory-allocation" ? FailAllocate : Allocate;
  allocator.freeCallback = Free;
  if(mode == "factory-allocation" || mode == "discard" || mode == "shutdown-allocator" || mode == "invalid-allocator") info.SetAllocationCallbacks(allocator);
  if(mode == "invalid-allocator") allocator.freeCallback = nullptr;
  if(mode == "buffer-object") failObjectSize = sizeof(Dali::Graphics::GLES::Buffer);
  auto buffer = controller.CreateBuffer(info, nullptr);
  auto map = [&](Dali::Graphics::Buffer* target, uint32_t size)
  {
    MapBufferInfo mapping{};
    mapping.buffer = target; mapping.offset = 0; mapping.size = size;
    mapping.usage = 0 | MemoryUsageFlagBits::WRITE;
    return controller.MapBufferRange(mapping);
  };
  if(mode == "factory-allocation" || mode == "buffer-object" || mode == "invalid-allocator")
  {
    assert(!buffer && WebFailure::HasFailure());
  }
  else if(mode == "discard")
  {
    buffer.reset();
    controller.ProcessDiscardQueues(); // Destroy pending create before flushing.
    assert(objectFrees == 1);
    controller.WaitIdle();
    controller.WaitIdle();
    assert(generations == 0 && objectFrees == 1);
  }
  else if(mode.find("uniform-") == 0)
  {
    buffer.reset(); controller.ProcessDiscardQueues();
    const bool cpu = mode.find("cpu") != std::string::npos;
    const bool failure = mode.find("failure") != std::string::npos;
    if(failure) failObjectSize = sizeof(WebBufferMemory);
    auto uniform = Dali::Internal::Render::UniformBufferV2::New(&controller, cpu, 16);
    uniform->ReSpecify(32);
    const uint32_t value = 0x12345678;
    if(failure)
    {
      assert(!uniform->IsValid() && !uniform->Write(&value, 4, 4) && !uniform->Flush());
      assert(WebFailure::HasFailure());
      uniform.reset(); controller.WaitIdle(); controller.Shutdown();
      puts("PASS Core uniform failure through real Controller"); return 0;
    }
    assert(uniform->IsValid() && uniform->Write(&value, 4, 4));
    assert(uniform->MemoryCompare(const_cast<uint32_t*>(&value), 4, 4));
    auto* actual = static_cast<Dali::Graphics::GLES::Buffer*>(uniform->GetGraphicsBuffer());
    assert(actual->IsCPUAllocated() == cpu);
    assert(uniform->Flush());
    if(cpu) assert(uploads == 0);
    else assert(std::memcmp(bytes[actual->GetGLBuffer()].data() + 4, &value, 4) == 0);
    if(cpu)
    {
      // CPU uniforms stay mapped across frames, including empty scenes.
      for(uint32_t size : {32u, 16u, 0u, 32u, 64u})
      {
        uniform->ReSpecify(size);
        assert(uniform->IsValid());
        assert(uniform->Write(&value, 4, 4) && uniform->Flush());
        assert(!WebFailure::HasFailure() && uploads == 0);
      }
    }
    uniform.reset(); controller.WaitIdle();
  }
  else
  {
    if(mode == "storage-object") failObjectSize = sizeof(WebBufferStorage);
    if(mode == "memory-object") failObjectSize = sizeof(WebBufferMemory);
    failGen = mode == "gl-allocation";
    failData = mode == "gl-data";
    auto memory = map(buffer.get(), 16);
    if(mode == "storage-object" || mode == "memory-object" || failGen || failData)
    {
      assert(!memory && WebFailure::HasFailure());
      auto before = generations;
      controller.WaitIdle(); controller.WaitIdle();
      assert(generations == before && !controller.CreateBuffer(info, nullptr));
    }
    else
    {
      assert(memory);
      auto* data = static_cast<unsigned char*>(memory->LockRegion(0, 16));
      assert(data); std::memset(data, 0x42, 16);
      if(mode == "shutdown" || mode == "shutdown-allocator")
      {
        controller.Shutdown();
        const auto before = uploads;
        buffer.reset();
        assert(data[0] == 0x42); // Storage pinned after Buffer destruction.
        memory.reset();
        assert(uploads == before && !controller.CanSubmit());
        if(mode == "shutdown-allocator") assert(objectFrees == 2);
        puts("PASS shutdown with live Memory"); return 0;
      }
      if(mode == "mapped-recycle")
      {
        auto* old = buffer.get();
        buffer = controller.CreateBuffer(info, std::move(buffer));
        assert(buffer && buffer.get() != old);
        controller.WaitIdle();
        const auto before = uploads;
        assert(data[0] == 0x42);
        memory.reset();
        assert(uploads == before);
        buffer.reset(); controller.WaitIdle(); controller.Shutdown();
        puts("PASS mapped recycle rejects reuse and pins old storage"); return 0;
      }
      memory->Unlock(true); controller.UnmapMemory(std::move(memory));
      auto* old = buffer.get();
      auto revision = static_cast<Dali::Graphics::GLES::Buffer*>(old)->GetBufferChangedCount();
      buffer = controller.CreateBuffer(info, std::move(buffer));
      assert(buffer.get() == old);
      if(mode == "recycle-failure") failObjectSize = sizeof(WebBufferStorage);
      memory = map(buffer.get(), 16);
      if(mode == "recycle-failure")
      {
        assert(!memory && WebFailure::HasFailure());
        auto before = generations;
        controller.WaitIdle(); controller.WaitIdle();
        assert(generations == before);
        buffer.reset(); controller.Shutdown();
        puts("PASS recycle failure has no retry"); return 0;
      }
      data = static_cast<unsigned char*>(memory->LockRegion(0, 16));
      for(unsigned i = 0; i < 16; ++i) assert(data[i] == 0);
      assert(static_cast<Dali::Graphics::GLES::Buffer*>(buffer.get())->GetBufferChangedCount() > revision);
      controller.UnmapMemory(std::move(memory));
      info.SetSize(32);
      if(mode == "grow-failure") failObjectSize = sizeof(Dali::Graphics::GLES::Buffer);
      buffer = controller.CreateBuffer(info, std::move(buffer));
      if(mode == "grow-failure")
      {
        assert(!buffer && WebFailure::HasFailure());
        controller.WaitIdle(); controller.Shutdown();
        puts("PASS grow factory allocation failure"); return 0;
      }
      assert(buffer && buffer.get() != old);
      memory = map(buffer.get(), 32); assert(memory);
      controller.UnmapMemory(std::move(memory));
      controller.WaitIdle();
      assert(deletions == 1);
    }
  }
  // Real command creation and execution before/after the permanent failure.
  if(!WebFailure::HasFailure())
  {
    auto command = controller.CreateCommandBuffer(CommandBufferCreateInfo{}, nullptr);
    command->Begin(CommandBufferBeginInfo{});
    command->SetViewport({0, 0, 16, 16, 0, 1}); command->End();
    SubmitInfo submit{}; submit.cmdBuffer.push_back(command.get()); submit.flags = 0 | SubmitFlagBits::FLUSH;
    controller.SubmitCommandBuffers(submit);
    assert(gl.viewports == 1);
    submit.flags = 0;
    controller.SubmitCommandBuffers(submit); // Queued before failure, also cancelled.
    WebFailure::Latch("test submission failure");
    controller.SubmitCommandBuffers(submit); controller.WaitIdle();
    assert(gl.viewports == 1);
  }
  buffer.reset(); controller.WaitIdle(); controller.Shutdown();
  printf("PASS production Buffer/Controller %s (CPU GL injection only)\n", mode.c_str());
}
