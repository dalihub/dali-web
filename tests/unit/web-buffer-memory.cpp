// Production memory/storage with injected GL responses. CPU contract evidence
// only: this array is not a GPU, readback, browser or production Controller.
#include <dali/internal/graphics/web/web-buffer-memory.h>
#include <dali/internal/graphics/web/web-gles-failure.h>
#include <dali/internal/graphics/web/web-gles-fence.h>
#include <GLES3/gl3.h>
#include <emscripten/html5_webgl.h>
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <memory>

namespace
{
std::array<unsigned char, 64> uploaded{};
int uploads = 0;
size_t uploadBytes = 0;
int frees = 0;
GLenum error = GL_NO_ERROR;
bool lost = false;
bool failUpload = false;
void* Allocate(size_t size, size_t, void*) { return std::malloc(size); }
void Free(void* data, void*) { ++frees; std::free(data); }
void* FailAllocate(size_t, size_t, void*) { return nullptr; }
}
extern "C"
{
void __wrap_glBindBuffer(GLenum target, GLuint) { assert(target == GL_COPY_WRITE_BUFFER); }
void __wrap_glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
{
  assert(target == GL_COPY_WRITE_BUFFER && offset >= 0 && size >= 0 && offset + size <= 64);
  ++uploads;
  uploadBytes += size;
  if(failUpload) { error = GL_OUT_OF_MEMORY; return; }
  std::memcpy(uploaded.data() + offset, data, size);
}
GLenum __wrap_glGetError() { auto result = error; error = GL_NO_ERROR; return result; }
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE __wrap_emscripten_webgl_get_current_context() { return 7; }
bool __wrap_emscripten_is_webgl_context_lost(EMSCRIPTEN_WEBGL_CONTEXT_HANDLE) { return lost; }
GLsync __wrap_glFenceSync(GLenum, GLbitfield) { return nullptr; }
void __wrap_glDeleteSync(GLsync) {}
GLenum __wrap_glClientWaitSync(GLsync, GLbitfield, GLuint64) { return GL_WAIT_FAILED; }
void __wrap_glFlush() {}
}
int main(int argc, char** argv)
{
  using namespace Dali::Graphics;
  using namespace Dali::Graphics::GLES;
  const std::string mode = argc > 1 ? argv[1] : "preserve";
  AllocationCallbacks allocator{};
  allocator.allocCallback = Allocate;
  allocator.freeCallback = Free;
  if(mode == "allocation") allocator.allocCallback = FailAllocate;
  auto storage = std::unique_ptr<WebBufferStorage, void(*)(WebBufferStorage*)>(
    new WebBufferStorage(16, mode == "cpu", &allocator), [](WebBufferStorage* value) { value->Unreference(); });
  storage->SetBufferId(1);
  if(mode == "allocation")
  {
    assert(!storage->Allocate() && WebFailure::HasFailure());
  }
  else
  {
    assert(storage->Allocate());
    if(mode == "bounds" || mode == "overflow")
    {
      WebBufferMemory memory(storage.get(), 4, 8);
      assert(!memory.LockRegion(mode == "overflow" ? UINT32_MAX : 7, 2));
      assert(WebFailure::HasFailure());
    }
    else if(mode == "map-bounds")
    {
      assert(!WebBufferMemory::Create(storage.get(), UINT32_MAX, 8) && WebFailure::HasFailure());
    }
    else if(mode == "duplicate-map")
    {
      std::unique_ptr<WebBufferMemory> memory(WebBufferMemory::Create(storage.get(), 0, 16));
      assert(memory);
      assert(!WebBufferMemory::Create(storage.get(), 0, 16) && WebFailure::HasFailure());
    }
    else if(mode == "generations")
    {
      std::unique_ptr<WebBufferMemory> memory(WebBufferMemory::Create(storage.get(), 0, 16));
      auto* old = static_cast<unsigned char*>(memory->LockRegion(0, 16));
      std::memset(old, 0x41, 16);
      storage->Detach();
      storage.reset(new WebBufferStorage(32, false, &allocator));
      storage->SetBufferId(2);
      assert(storage->Allocate());
      for(unsigned i = 0; i < 32; ++i) assert(storage->Data()[i] == 0);
      assert(old[0] == 0x41); // Old mapping pinned; new generation zeroed.
      const auto before = uploads;
      memory.reset();
      assert(uploads == before && frees == 1);
      storage->Detach();
      storage.reset(new WebBufferStorage(32, false, &allocator));
      storage->SetBufferId(2);
      assert(storage->Allocate()); // Same-size recycled storage starts zeroed.
      for(unsigned i = 0; i < 32; ++i) assert(storage->Data()[i] == 0);
    }
    else if(mode == "context")
    {
      lost = true;
      assert(!WebFailure::CheckContext() && WebFailure::HasFailure());
      lost = false;
      assert(!WebFailure::CheckContext()); // No automatic recovery.
    }
    else if(mode == "fence")
    {
      WebFence fence;
      assert(!fence.Insert());
      assert(WebFailure::HasFailure() && !WebFailure::CheckContext());
    }
    else
    {
      {
        WebBufferMemory memory(storage.get(), 0, 16);
        auto* data = static_cast<unsigned char*>(memory.LockRegion(0, 16));
        assert(data);
        for(unsigned i = 0; i < 16; ++i) data[i] = i + 1;
        memory.Unlock(true);
        const auto before = uploads;
        memory.Unlock(true);
        assert(uploads == before); // Double unlock has no second upload.
      }
      {
        WebBufferMemory memory(storage.get(), 4, 8);
        auto* data = static_cast<unsigned char*>(memory.LockRegion(2, 4));
        assert(data == storage->Data() + 6);
        std::memset(data, 0xa5, 4);
        memory.Unlock(false); // Destructor must flush the pending write.
      }
      for(unsigned i = 0; i < 16; ++i)
      {
        const unsigned expected = i >= 6 && i < 10 ? 0xa5 : i + 1;
        assert(storage->Data()[i] == expected);
        if(mode != "cpu") assert(uploaded[i] == expected);
      }
      // Callback return counts do not describe dirty locations. Reproduce
      // zero, partial and full writes through the same public Memory contract.
      for(unsigned count : {0u, 4u, 16u})
      {
        WebBufferMemory memory(storage.get(), 0, 16);
        auto* data = static_cast<unsigned char*>(memory.LockRegion(0, 16));
        auto previous = uploaded;
        if(count) std::memset(data + (count == 4 ? 8 : 0), 0x55, count);
        memory.Unlock(true);
        if(mode != "cpu" && count == 0) assert(uploaded == previous);
        if(mode != "cpu") assert(std::memcmp(uploaded.data(), storage->Data(), 16) == 0);
      }
      {
        WebBufferMemory memory(storage.get(), 0, 16);
        auto* data = static_cast<unsigned char*>(memory.LockRegion(0, 16));
        memory.Flush();
        data[3] = 0x72;
        memory.Unlock(true);
        if(mode != "cpu") assert(uploaded[3] == 0x72);
      }
      if(mode == "upload")
      {
        WebBufferMemory memory(storage.get(), 0, 16);
        assert(memory.LockRegion(0, 4));
        failUpload = true;
        memory.Unlock(true);
        assert(WebFailure::HasFailure());
        const auto before = uploads;
        assert(!memory.LockRegion(0, 4));
        assert(uploads == before);
      }
      else
      {
        auto memory = std::make_unique<WebBufferMemory>(storage.get(), 0, 16);
        assert(memory->LockRegion(0, 4));
        const auto before = uploads;
        if(mode == "stop") WebFailure::Stop();
        else storage->Detach();
        storage.reset();
        assert(frees == 0); // Mapping pins CPU allocation through detach.
        memory.reset();
        assert(frees == 1 && uploads == before);
      }
      if(mode == "cpu") assert(uploads == 0);
    }
  }
  std::printf("Web memory CPU contract: %s PASS (not GPU evidence)\n", mode.c_str());
  std::printf("injected upload calls=%d bytes=%zu (CPU accounting only)\n", uploads, uploadBytes);
}
