// Production Web wrapper with injected WebGL responses; no browser/GPU claim.
#include <dali/internal/graphics/web/web-gl-implementation.h>
#include <dali/internal/graphics/web/web-gles-failure.h>
#include <dali/internal/graphics/gles-impl/gles-program-binary-cache.h>
#include <GLES3/gl3.h>
#include <emscripten/html5_webgl.h>
#include <cassert>
#include <string>
#include <cstdio>
namespace
{
unsigned draws = 0;
unsigned blends = 0;
GLenum error = GL_NO_ERROR;
}
extern "C"
{
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE __wrap_emscripten_webgl_get_current_context()
{
  return 7;
}
bool __wrap_emscripten_is_webgl_context_lost(EMSCRIPTEN_WEBGL_CONTEXT_HANDLE)
{
  return false;
}
EMSCRIPTEN_RESULT __wrap_emscripten_webgl_get_context_attributes(EMSCRIPTEN_WEBGL_CONTEXT_HANDLE, EmscriptenWebGLContextAttributes* attributes)
{
  attributes->majorVersion = 2; return EMSCRIPTEN_RESULT_SUCCESS;
}
GLenum __wrap_glGetError()
{
  const auto result = error; error = GL_NO_ERROR; return result;
}
void __wrap_glGetIntegerv(GLenum name, GLint* value)
{
  if(name == GL_MAJOR_VERSION) *value = 3;
  else if(name == GL_MINOR_VERSION) *value = 0;
  else if(name == GL_MAX_TEXTURE_SIZE) *value = 4096;
  else { assert(name == GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS); *value = 32; }
}
void __wrap_glBlendEquation(GLenum mode)
{
  assert(mode == GL_FUNC_ADD); ++blends;
}
void __wrap_glDrawArrays(GLenum, GLint, GLsizei)
{
  ++draws;
}

}
int main(int argc, char** argv)
{
  using namespace Dali::Graphics::GLES;
  WebGlImplementation gl;
  assert(gl.ContextCreated());
  assert(gl.GetShaderLanguageVersion() == 300 && !gl.IsAdvancedBlendEquationSupported());
  assert(!ProgramBinaryCache::IsSupported());
  gl.BlendEquation(GL_FUNC_ADD);
  gl.DrawArrays(GL_TRIANGLES, 0, 3);
  assert(draws == 1 && blends == 1);
  const std::string mode = argc > 1 ? argv[1] : "blend";
  if(mode == "blend") gl.BlendEquation(0x9294); // KHR advanced blend.
  else if(mode == "map") assert(!gl.MapBufferRange(GL_ARRAY_BUFFER, 0, 16, GL_MAP_WRITE_BIT));
  else if(mode == "binary") gl.ProgramBinary(1, 0, nullptr, 0);
  else if(mode == "cache") assert(!ProgramBinaryCache::LoadProgramBinary({}, "test", 1, &gl));
  else if(mode == "error")
  {
    error = GL_OUT_OF_MEMORY; gl.DrawArrays(GL_TRIANGLES, 0, 3);
  }
  assert(WebFailure::HasFailure());
  gl.DrawArrays(GL_TRIANGLES, 0, 3);
  assert(draws == 1 && blends == 1);
  char log[8] = "dirty";
  Dali::GLsizei size = 9;
  gl.GetShaderInfoLog(1, 8, &size, log);
  assert(size == 0 && log[0] == 0);
  printf("PASS WebGL2 capability/fail-stop %s (CPU injection)\n", mode.c_str());
}
