#pragma once
// Test-only: real Controller/Core uploads, WebGL2 readback on the GL owner.
#include <dali/internal/render/renderers/gpu-buffer.h>
#include <dali/internal/render/renderers/uniform-buffer.h>
#include <dali/internal/graphics/gles-impl/gles-graphics-program.h>
#include <dali/internal/graphics/gles-impl/gles-graphics-reflection.h>
#include <dali/devel-api/signals/vertex-buffer-update-callback.h>
#include <GLES3/gl3.h>
#include <array>
#include <cstring>
#include <vector>

EM_JS(void, ReadGpuBytes, (unsigned id, unsigned char* output, unsigned size), {
  const gl = GL.currentContext.GLctx;
  const previous = gl.getParameter(gl.COPY_READ_BUFFER_BINDING);
  gl.bindBuffer(gl.COPY_READ_BUFFER, GL.buffers[id]);
  gl.getBufferSubData(gl.COPY_READ_BUFFER, 0, HEAPU8.subarray(output, output + size));
  gl.bindBuffer(gl.COPY_READ_BUFFER, previous);
});
EM_JS(void, ReportMemoryCheck, (unsigned frame, const char* name, const unsigned char* actual, const unsigned char* expected, unsigned size, unsigned error, int passed), {
  Module['onDaliMemory']?.({frame, name: UTF8ToString(name), actual: Array.from(HEAPU8.subarray(actual, actual + size)), expected: Array.from(HEAPU8.subarray(expected, expected + size)), error, passed: !!passed});
});

EM_JS(int, ProbeMode, (), { return Module['daliProbeMode'] || 0; });
namespace Dali::Web
{
class ProbeGl : public Graphics::GLES::WebGlImplementation
{
public:
  int mode{0};
  void GetActiveUniformsiv(Dali::GLuint program, Dali::GLsizei count, const Dali::GLuint* indices, Dali::GLenum name, Dali::GLint* params) override
  {
    if(mode == 2 && name == GL_UNIFORM_NAME_LENGTH)
    {
      // Real invalid enum, not a mocked query response. The production wrapper
      // must detect the GL error and reflection must stop using query outputs.
      glEnable(0xdead);
      mode = 0;
    }
    WebGlImplementation::GetActiveUniformsiv(program, count, indices, name, params);
  }
};
class MemoryProbe
{
  struct Result
  {
    const char* name;
    std::vector<unsigned char> actual;
    std::vector<unsigned char> expected;
    unsigned error;
    bool passed;
  };
public:
  explicit MemoryProbe(Graphics::GlesGraphicsController& controller)
  : mController(controller), mBuffer(controller, 0 | Graphics::BufferUsage::VERTEX_BUFFER, Internal::GpuBuffer::WritePolicy::RETAIN)
  {
  }
  uint32_t Callback(void* pointer, size_t size)
  {
    if(size != 16)
    {
      return 17;
    }
    if(mNoWrite)
    {
      return 0;
    }
    // Even count 0 can accompany writes. Count controls drawing, not dirty location.
    if(mCount == 16)
    {
      std::memcpy(pointer, mExpected.data(), 16);
    }
    else
    {
      std::memcpy(static_cast<unsigned char*>(pointer) + 6, mExpected.data() + 6, 4);
    }
    return mCount;
  }
  void Check(const char* name, const Graphics::Buffer* buffer, const unsigned char* expected, unsigned size)
  {
    Result result{name, std::vector<unsigned char>(size, 0xcd), std::vector<unsigned char>(expected, expected + size), 0, false};
    if(buffer)
    {
      const auto* actual = static_cast<const Graphics::GLES::Buffer*>(buffer);
      ReadGpuBytes(actual->GetGLBuffer(), result.actual.data(), size);
      result.error = glGetError();
      result.passed = !result.error && result.actual == result.expected && !Graphics::GLES::WebFailure::HasFailure();
    }
    mResults.push_back(std::move(result));
  }
  void Flag(const char* name, bool passed)
  {
    mResults.push_back({name, {}, {}, glGetError(), passed});
  }
  void Run(unsigned frame)
  {
    mResults.clear();
    if(frame == 1)
    {
      for(unsigned i = 0; i < 16; ++i) mExpected[i] = 16 + i;
      Flag("core-initial-return", mBuffer.UpdateDataBuffer(mController, 16, mExpected.data()));
      Check("core-initial-16", mBuffer.GetGraphicsObject(), mExpected.data(), 16);
    }
    else
    {
      Check("next-frame-preserved", mBuffer.GetGraphicsObject(), mExpected.data(), 16);
    }
    auto callback = VertexBufferUpdateCallback::New(this, &MemoryProbe::Callback);
    mNoWrite = true;
    uint32_t untouchedCount = 99;
    Flag("callback-no-write-return", mBuffer.UpdateDataBufferWithCallback(mController, callback.Get(), untouchedCount) && untouchedCount == 0);
    Check("callback-no-write", mBuffer.GetGraphicsObject(), mExpected.data(), 16);
    mNoWrite = false;
    for(unsigned count : {0u, 4u, 16u})
    {
      mCount = count;
      for(unsigned i = 6; i < 10; ++i) mExpected[i] = 80 + frame * 20 + count + i;
      if(count == 16)
        for(unsigned i = 0; i < 16; ++i) mExpected[i] = frame * 30 + i;
      uint32_t updated = 99;
      Flag("callback-return", mBuffer.UpdateDataBufferWithCallback(mController, callback.Get(), updated) && updated == count);
      Check(count == 0 ? "callback-0" : count == 4 ? "callback-4" : "callback-16", mBuffer.GetGraphicsObject(), mExpected.data(), 16);
    }
    Graphics::MapBufferInfo map{};
    map.buffer = const_cast<Graphics::Buffer*>(mBuffer.GetGraphicsObject());
    map.offset = 3;
    map.size = 10;
    map.usage = 0 | Graphics::MemoryUsageFlagBits::WRITE;
    auto memory = mController.MapBufferRange(map);
    auto* pointer = memory ? static_cast<unsigned char*>(memory->LockRegion(3, 4)) : nullptr;
    Flag("nonzero-map-lock", pointer != nullptr);
    if(pointer)
    {
      std::memset(pointer, 0x70 + frame, 4);
      std::memset(mExpected.data() + 6, 0x70 + frame, 4);
      memory->Unlock(false);
      mController.UnmapMemory(std::move(memory));
      Check("offset-unlock-false-unmap", mBuffer.GetGraphicsObject(), mExpected.data(), 16);
    }
    if(!mUniform)
    {
      mUniform = Internal::Render::UniformBufferV2::New(&mController, false, 16);
    }
    // Exercise both rotating GPU buffers and growth, saving the submitted buffer
    // before Flush changes the Core buffer index.
    for(unsigned size : {16u, 16u, 32u, 32u})
    {
      std::vector<unsigned char> pattern(size);
      for(unsigned i = 0; i < size; ++i) pattern[i] = i + frame * 32;
      mUniform->ReSpecify(size);
      Flag("gpu-uniform-write", mUniform->Write(pattern.data(), size, 0));
      std::memset(pattern.data() + 6, 0x90 + frame, 4);
      Flag("gpu-uniform-partial-write", mUniform->Write(pattern.data() + 6, 4, 6));
      auto* submitted = mUniform->GetGraphicsBuffer();
      Flag("gpu-uniform-flush", mUniform->Flush());
      Check("gpu-uniform-readback", submitted, pattern.data(), size);
    }
    CheckCpuUniform(frame);
    CheckLifetime(frame, false);
    CheckLifetime(frame, true);
  }
  void CheckLifetime(unsigned frame, bool custom)
  {
    Graphics::BufferCreateInfo info{};
    AllocationState allocation;
    Graphics::AllocationCallbacks callbacks{};
    callbacks.userData = &allocation;
    callbacks.allocCallback = Allocate;
    callbacks.freeCallback = Free;
    if(custom)
    {
      info.SetAllocationCallbacks(callbacks);
    }
    info.SetSize(16).SetUsage(0 | Graphics::BufferUsage::VERTEX_BUFFER);
    auto buffer = mController.CreateBuffer(info, nullptr);
    Graphics::MapBufferInfo map{};
    map.usage = 0 | Graphics::MemoryUsageFlagBits::WRITE;
    map.buffer = buffer.get();
    map.offset = 0;
    map.size = 16;
    auto memory = mController.MapBufferRange(map);
    auto* pointer = memory ? static_cast<unsigned char*>(memory->LockRegion(0, 16)) : nullptr;
    if(!pointer)
    {
      Flag("lifetime-map", false);
      return;
    }
    std::memcpy(pointer, mExpected.data(), 16);
    memory->Flush();
    Check("before-detach", buffer.get(), mExpected.data(), 16);
    const auto oldId = static_cast<Graphics::GLES::Buffer*>(buffer.get())->GetGLBuffer();
    auto* oldBuffer = buffer.get();
    buffer = mController.CreateBuffer(info, std::move(buffer));
    Flag("mapped-recycle-new-object", buffer && buffer.get() != oldBuffer);
    mController.WaitIdle();
    Flag("detached-gl-deleted-shadow-pinned", !glIsBuffer(oldId) && std::memcmp(pointer, mExpected.data(), 16) == 0);
    // Release a still-live mapping after its Buffer has detached; it must not
    // upload into either the deleted object or its replacement.
    if(custom)
    {
      Flag("detached-shadow-not-freed", allocation.shadowFrees == 0);
    }
    pointer[0] = 0xee;
    memory.reset();
    if(custom)
    {
      Flag("last-reference-frees-shadow", allocation.shadowFrees == 1);
    }
    map.buffer = buffer.get();
    memory = mController.MapBufferRange(map);
    std::array<unsigned char, 32> zeros{};
    Check("replacement-zero", buffer.get(), zeros.data(), 16);
    mController.UnmapMemory(std::move(memory));
    oldBuffer = buffer.get();
    buffer = mController.CreateBuffer(info, std::move(buffer));
    Flag(custom ? "custom-allocator-new-object" : "unmapped-recycle-reuse",
      custom ? buffer.get() != oldBuffer : buffer.get() == oldBuffer);
    map.buffer = buffer.get();
    memory = mController.MapBufferRange(map);
    Check("recycle-zero", buffer.get(), zeros.data(), 16);
    mController.UnmapMemory(std::move(memory));
    info.SetSize(32);
    buffer = mController.CreateBuffer(info, std::move(buffer));
    map.buffer = buffer.get();
    map.size = 32;
    memory = mController.MapBufferRange(map);
    Check("grow-zero", buffer.get(), zeros.data(), 32);
    pointer = memory ? static_cast<unsigned char*>(memory->LockRegion(0, 32)) : nullptr;
    Flag("grow-lock", pointer != nullptr);
    if(pointer)
    {
      for(unsigned i = 0; i < 32; ++i) zeros[i] = frame + i;
      std::memcpy(pointer, zeros.data(), 32);
    }
    mController.UnmapMemory(std::move(memory));
    Check("grow-upload", buffer.get(), zeros.data(), 32);
    buffer.reset();
    mController.WaitIdle();
    if(custom)
    {
      Flag("allocator-all-references-released", allocation.allocations == allocation.frees);
    }
  }
  void CheckCpuUniform(unsigned frame)
  {
    using namespace Graphics;
    if(!mCpuProgram)
    {
      const char* vertex = "attribute vec2 aPosition; void main(){gl_Position=vec4(aPosition,0.0,1.0);}";
      const char* fragment = "precision mediump float; uniform vec4 uMemory; void main(){gl_FragColor=uMemory;}";
      ShaderCreateInfo info{};
      info.SetSourceMode(ShaderSourceMode::TEXT).SetSourceData(vertex).SetSourceSize(std::strlen(vertex) + 1).SetPipelineStage(PipelineStage::VERTEX_SHADER);
      auto vs = mController.CreateShader(info, nullptr);
      info.SetSourceData(fragment).SetSourceSize(std::strlen(fragment) + 1).SetPipelineStage(PipelineStage::FRAGMENT_SHADER);
      auto fs = mController.CreateShader(info, nullptr);
      std::vector<ShaderState> stages{
        ShaderState().SetShader(*vs).SetPipelineStage(PipelineStage::VERTEX_SHADER),
        ShaderState().SetShader(*fs).SetPipelineStage(PipelineStage::FRAGMENT_SHADER)};
      ProgramCreateInfo programInfo{};
      programInfo.SetShaderState(stages);
      mCpuProgram = mController.CreateProgram(programInfo, nullptr);
      mCpuUniform = Internal::Render::UniformBufferV2::New(&mController, true, 16);
    }
    auto* program = static_cast<GLES::Program*>(mCpuProgram.get())->GetImplementation();
    const auto& reflection = program->GetReflection();
    UniformInfo info{};
    if(!reflection.GetNamedUniform("uMemory", info))
    {
      Flag("cpu-reflection", false);
      return;
    }
    GLint previous = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &previous);
    glUseProgram(program->GetGlProgram());
    for(unsigned size : {16u, 16u, 32u})
    {
      std::array<float, 4> expected{0.125f, 0.25f, 0.5f, 1.0f};
      mCpuUniform->ReSpecify(size);
      Flag("cpu-uniform-write", mCpuUniform->Write(expected.data(), 16, info.offset));
      expected[1] = frame == 1 ? 0.375f : 0.75f;
      Flag("cpu-uniform-middle4", mCpuUniform->Write(&expected[1], 4, info.offset + 4) && mCpuUniform->Flush());
      auto* buffer = static_cast<GLES::Buffer*>(mCpuUniform->GetGraphicsBuffer());
      program->UpdateStandaloneUniformBlock(static_cast<const char*>(buffer->GetCPUAllocatedAddress()));
      std::array<float, 4> actual{};
      glGetUniformfv(program->GetGlProgram(), info.location, actual.data());
      Result result{"cpu-uniform-gpu-query", {}, {}, glGetError(), actual == expected};
      const auto* actualBytes = reinterpret_cast<const unsigned char*>(actual.data());
      const auto* expectedBytes = reinterpret_cast<const unsigned char*>(expected.data());
      result.actual.assign(actualBytes, actualBytes + 16);
      result.expected.assign(expectedBytes, expectedBytes + 16);
      mResults.push_back(std::move(result));
    }
    glUseProgram(previous);
  }
  void Report(unsigned frame)
  {
    for(const auto& result : mResults)
      ReportMemoryCheck(frame, result.name, result.actual.data(), result.expected.data(), result.actual.size(), result.error, result.passed);
  }
  void Clear()
  {
    mCpuUniform.reset();
    mCpuProgram.reset();
    mUniform.reset();
    mBuffer.Destroy();
  }
private:
  Graphics::GlesGraphicsController& mController;
  Internal::GpuBuffer mBuffer;
  Graphics::UniquePtr<Internal::Render::UniformBufferV2> mUniform;
  Graphics::UniquePtr<Internal::Render::UniformBufferV2> mCpuUniform;
  Graphics::UniquePtr<Graphics::Program> mCpuProgram;
  std::array<unsigned char, 16> mExpected{};
  struct AllocationState
  {
    unsigned allocations{0};
    unsigned frees{0};
    unsigned shadowFrees{0};
    void* firstShadow{nullptr};
  };
  static void* Allocate(size_t size, size_t, void* user)
  {
    auto& state = *static_cast<AllocationState*>(user);
    auto* pointer = std::malloc(size);
    if(pointer)
    {
      ++state.allocations;
    }
    if(size == 16 && !state.firstShadow)
    {
      state.firstShadow = pointer;
    }
    return pointer;
  }
  static void Free(void* pointer, void* user)
  {
    auto& state = *static_cast<AllocationState*>(user);
    ++state.frees;
    if(pointer == state.firstShadow)
    {
      ++state.shadowFrees;
    }
    std::free(pointer);
  }
  unsigned mCount{0};
  bool mNoWrite{false};
  std::vector<Result> mResults;
};
}
