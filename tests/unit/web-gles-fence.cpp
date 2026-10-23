// Deterministic lifetime tests with injected GL responses. This is not GPU or
// browser evidence and must never be linked into the production renderer.
#include <dali/internal/graphics/web/web-gles-fence.h>
#include <GLES3/gl3.h>
#include <emscripten/html5_webgl.h>
#include <cassert>
#include <cstdio>
#include <thread>

namespace
{
GLenum waitResult = GL_TIMEOUT_EXPIRED;
bool failCreation = false;
bool contextLost = false;
int created = 0;
int deleted = 0;
int polled = 0;
int flushed = 0;
}

extern "C"
{
GLsync __wrap_glFenceSync(GLenum condition, GLbitfield flags)
{
  assert(condition == GL_SYNC_GPU_COMMANDS_COMPLETE && flags == 0);
  if(failCreation)
  {
    return nullptr;
  }
  return reinterpret_cast<GLsync>(++created);
}
void __wrap_glDeleteSync(GLsync sync)
{
  assert(sync);
  ++deleted;
}
GLenum __wrap_glClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
{
  assert(sync && flags == 0 && timeout == 0);
  ++polled;
  return waitResult;
}
void __wrap_glFlush()
{
  ++flushed;
}
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE __wrap_emscripten_webgl_get_current_context()
{
  return 7;
}
bool __wrap_emscripten_is_webgl_context_lost(EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context)
{
  assert(context == 7);
  return contextLost;
}
}

int main()
{
  using Fence = Dali::Graphics::GLES::WebFence;
  using Status = Fence::Status;
  assert(!Fence::HasFailure());
  Fence empty;
  assert(!empty.Poll() && empty.GetStatus() == Status::Empty);
  empty.Release();
  empty.Release();
  {
    Fence fence;
    assert(fence.Insert() && flushed == 1);
    for(int i = 0; i < 10; ++i)
    {
      Fence::Collect();
      assert(!fence.Poll() && fence.GetStatus() == Status::Pending);
    }
    assert(deleted == 0);
    waitResult = GL_ALREADY_SIGNALED;
    assert(fence.Poll() && fence.GetStatus() == Status::Complete);
    const int polls = polled;
    assert(fence.Poll() && polled == polls && deleted == 1);
    Fence::Collect();
  }
  assert(deleted == 1);
  {
    Fence fence;
    assert(fence.Insert());
    waitResult = GL_CONDITION_SATISFIED;
    Fence::Collect();
    assert(fence.Poll() && deleted == 2);
  }
  {
    Fence fence;
    assert(fence.Insert());
    waitResult = GL_WAIT_FAILED;
    assert(!fence.Poll() && fence.GetStatus() == Status::Failed);
    assert(Fence::HasFailure());
    const int polls = polled;
    assert(!fence.Poll() && polled == polls && deleted == 3);
  }
  {
    failCreation = true;
    Fence fence;
    assert(!fence.Insert() && fence.GetStatus() == Status::Failed);
    assert(!fence.Poll() && deleted == 3);
    failCreation = false;
  }
  {
    waitResult = GL_TIMEOUT_EXPIRED;
    auto* fence = new Fence;
    assert(fence->Insert());
    const int polls = polled;
    std::thread consumer([&]()
    {
      assert(!fence->Poll());
      delete fence;
    });
    consumer.join();
    assert(deleted == 3 && polls == polled);
    Fence::Collect();
    assert(deleted == 4);
  }
  {
    Fence fence;
    assert(fence.Insert());
    Fence::Shutdown();
    assert(!fence.Poll() && fence.GetStatus() == Status::Cancelled && deleted == 5);
    Fence::Shutdown();
  }
  assert(deleted == 5);
  {
    Fence fence;
    assert(fence.Insert());
    assert(fence.Insert()); // Replace an unfinished fence without leaking it.
    assert(deleted == 6);
    fence.Release();
    fence.Release();
    assert(deleted == 7 && !fence.Poll());
    Fence::Collect();
  }
  {
    Fence fence;
    assert(fence.Insert());
    contextLost = true;
    assert(!fence.Poll() && fence.GetStatus() == Status::Failed);
    assert(Fence::HasFailure());
    Fence::Shutdown();
    assert(deleted == 7); // The lost context already owns disposal.
    contextLost = false;
  }
  std::puts("PASS: pending, completion, failure, deferred deletion, shutdown, replacement, context loss (injected GL; no GPU validation)");
}
