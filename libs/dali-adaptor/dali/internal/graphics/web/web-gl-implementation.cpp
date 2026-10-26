/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <dali/internal/graphics/web/web-gl-implementation.h>
#include <dali/internal/graphics/web/web-gles-failure.h>
#include <emscripten/html5_webgl.h>
#include <GLES3/gl3.h>
namespace DALI_NAMESPACE::Graphics::GLES
{
namespace
{
bool SupportedBlend(GLenum mode)
{
  return mode == GL_FUNC_ADD || mode == GL_FUNC_SUBTRACT || mode == GL_FUNC_REVERSE_SUBTRACT || mode == GL_MIN || mode == GL_MAX;
}
}
bool WebGlImplementation::ContextCreated()
{
  if(!WebFailure::CheckContext()) return false;
  EmscriptenWebGLContextAttributes attributes{};
  if(emscripten_webgl_get_context_attributes(emscripten_webgl_get_current_context(), &attributes) != EMSCRIPTEN_RESULT_SUCCESS || attributes.majorVersion != 2)
  {
    WebFailure::Latch("WebGL2 context required");
    return false;
  }
  GLint major = 0;
  GLint minor = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mMaxTextureSize);
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &mMaxTextureUnits);
  if(major != 3 || minor != 0 || mMaxTextureSize <= 0 || mMaxTextureUnits < 16)
  {
    WebFailure::Latch("WebGL2 limits or GLES mapping invalid");
    return false;
  }
  mReady = WebFailure::CheckGl();
  return mReady;
}
void WebGlImplementation::PreRender()
{
  WebFailure::CheckGl();
}
void WebGlImplementation::PostRender()
{
  WebFailure::CheckGl();
}
bool WebGlImplementation::IsSurfacelessContextSupported() const
{
  return false;
}
bool WebGlImplementation::IsAdvancedBlendEquationSupported()
{
  return false;
}
bool WebGlImplementation::IsMultisampledRenderToTextureSupported()
{
  return false;
}
bool WebGlImplementation::IsBlendEquationSupported(Dali::BlendEquation::Type blendEquation)
{
  return blendEquation == Dali::BlendEquation::ADD || blendEquation == Dali::BlendEquation::SUBTRACT || blendEquation == Dali::BlendEquation::REVERSE_SUBTRACT || blendEquation == Dali::BlendEquation::MIN || blendEquation == Dali::BlendEquation::MAX;
}
uint32_t WebGlImplementation::GetShaderLanguageVersion()
{
  return mReady ? 300u : 0u;
}
std::string WebGlImplementation::GetShaderVersionPrefix()
{
  return "#version 300 es\n";
}
std::string WebGlImplementation::GetVertexShaderPrefix()
{
  return GetShaderVersionPrefix() + "#define INPUT in\n#define OUTPUT out\n#define FLAT flat\n";
}
std::string WebGlImplementation::GetFragmentShaderPrefix()
{
  return GetShaderVersionPrefix() + "#define INPUT in\n#define FLAT flat\n#define OUT_COLOR fragColor\n#define TEXTURE texture\nout mediump vec4 fragColor;\n";
}
bool WebGlImplementation::TextureRequiresConverting(const GLenum imageGlFormat, const GLenum textureGlFormat, const bool isSubImage) const
{
  return imageGlFormat == GL_RGB && textureGlFormat == GL_RGBA && !isSubImage;
}
void WebGlImplementation::ActiveTexture(GLenum texture)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glActiveTexture(texture);
}
void WebGlImplementation::AttachShader(GLuint program, GLuint shader)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glAttachShader(program, shader);
}
void WebGlImplementation::BindAttribLocation(GLuint program, GLuint index, const char* name)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBindAttribLocation(program, index, name);
}
void WebGlImplementation::BindBuffer(GLenum target, GLuint buffer)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBindBuffer(target, buffer);
}
void WebGlImplementation::BindFramebuffer(GLenum target, GLuint framebuffer)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBindFramebuffer(target, framebuffer);
}
void WebGlImplementation::BindRenderbuffer(GLenum target, GLuint renderbuffer)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBindRenderbuffer(target, renderbuffer);
}
void WebGlImplementation::BindTexture(GLenum target, GLuint texture)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  if(target == 0x8D65)
  {
    WebFailure::Latch("external texture unsupported"); return;
  }
  glBindTexture(target, texture);
}
void WebGlImplementation::BlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBlendColor(red, green, blue, alpha);
}
void WebGlImplementation::BlendEquation( GLenum mode )
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  if(!SupportedBlend(mode))
  {
    WebFailure::Latch("advanced blend unsupported"); return;
  }
  glBlendEquation(mode);
}
void WebGlImplementation::BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  if(!SupportedBlend(modeRGB) || !SupportedBlend(modeAlpha))
  {
    WebFailure::Latch("advanced blend unsupported"); return;
  }
  glBlendEquationSeparate(modeRGB, modeAlpha);
}
void WebGlImplementation::BlendFunc(GLenum sfactor, GLenum dfactor)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBlendFunc(sfactor, dfactor);
}
void WebGlImplementation::BlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
}
void WebGlImplementation::BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBufferData(target, size, data, usage);
}
void WebGlImplementation::BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBufferSubData(target, offset, size, data);
}
GLenum WebGlImplementation::CheckFramebufferStatus(GLenum target)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glCheckFramebufferStatus(target);
}
void WebGlImplementation::Clear(GLbitfield mask)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glClear(mask);
}
void WebGlImplementation::ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glClearColor(red, green, blue, alpha);
}
void WebGlImplementation::ClearDepthf(GLclampf depth)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glClearDepthf(depth);
}
void WebGlImplementation::ClearStencil(GLint s)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glClearStencil(s);
}
void WebGlImplementation::ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glColorMask(red, green, blue, alpha);
}
void WebGlImplementation::CompileShader(GLuint shader)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glCompileShader(shader);
}
void WebGlImplementation::CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
  WebFailure::Latch("unsupported WebGL2 operation: CompressedTexImage2D");
  return;
}
void WebGlImplementation::CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)
{
  WebFailure::Latch("unsupported WebGL2 operation: CompressedTexSubImage2D");
  return;
}
void WebGlImplementation::CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
}
void WebGlImplementation::CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
}
GLuint WebGlImplementation::CreateProgram(void)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glCreateProgram();
}
GLuint WebGlImplementation::CreateShader(GLenum type)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glCreateShader(type);
}
void WebGlImplementation::CullFace(GLenum mode)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glCullFace(mode);
}
void WebGlImplementation::DeleteBuffers(GLsizei n, const GLuint* buffers)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDeleteBuffers(n, buffers);
}
void WebGlImplementation::DeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDeleteFramebuffers(n, framebuffers);
}
void WebGlImplementation::DeleteProgram(GLuint program)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDeleteProgram(program);
}
void WebGlImplementation::DeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDeleteRenderbuffers(n, renderbuffers);
}
void WebGlImplementation::DeleteShader(GLuint shader)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDeleteShader(shader);
}
void WebGlImplementation::DeleteTextures(GLsizei n, const GLuint* textures)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDeleteTextures(n, textures);
}
void WebGlImplementation::DepthFunc(GLenum func)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDepthFunc(func);
}
void WebGlImplementation::DepthMask(GLboolean flag)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDepthMask(flag);
}
void WebGlImplementation::DepthRangef(GLclampf zNear, GLclampf zFar)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDepthRangef(zNear, zFar);
}
void WebGlImplementation::DetachShader(GLuint program, GLuint shader)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDetachShader(program, shader);
}
void WebGlImplementation::Disable(GLenum cap)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDisable(cap);
}
void WebGlImplementation::DisableVertexAttribArray(GLuint index)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDisableVertexAttribArray(index);
}
void WebGlImplementation::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDrawArrays(mode, first, count);
}
void WebGlImplementation::DrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDrawElements(mode, count, type, indices);
}
void WebGlImplementation::Enable(GLenum cap)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glEnable(cap);
}
void WebGlImplementation::EnableVertexAttribArray(GLuint index)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glEnableVertexAttribArray(index);
}
void WebGlImplementation::Finish(void)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glFinish();
}
void WebGlImplementation::Flush(void)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glFlush();
}
void WebGlImplementation::FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}
void WebGlImplementation::FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glFramebufferTexture2D(target, attachment, textarget, texture, level);
}
void WebGlImplementation::FrontFace(GLenum mode)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glFrontFace(mode);
}
void WebGlImplementation::GenBuffers(GLsizei n, GLuint* buffers)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGenBuffers(n, buffers);
}
void WebGlImplementation::GenerateMipmap(GLenum target)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGenerateMipmap(target);
}
void WebGlImplementation::GenFramebuffers(GLsizei n, GLuint* framebuffers)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGenFramebuffers(n, framebuffers);
}
void WebGlImplementation::GenRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGenRenderbuffers(n, renderbuffers);
}
void WebGlImplementation::GenTextures(GLsizei n, GLuint* textures)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGenTextures(n, textures);
}
void WebGlImplementation::GetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetActiveAttrib(program, index, bufsize, length, size, type, name);
}
void WebGlImplementation::GetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetActiveUniform(program, index, bufsize, length, size, type, name);
}
void WebGlImplementation::GetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetAttachedShaders(program, maxcount, count, shaders);
}
GLint WebGlImplementation::GetAttribLocation(GLuint program, const char* name)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glGetAttribLocation(program, name);
}
void WebGlImplementation::GetBooleanv(GLenum pname, GLboolean* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetBooleanv(pname, params);
}
void WebGlImplementation::GetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetBufferParameteriv(target, pname, params);
}
GLenum WebGlImplementation::GetError(void)
{
  return glGetError();
}
void WebGlImplementation::GetFloatv(GLenum pname, GLfloat* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetFloatv(pname, params);
}
void WebGlImplementation::GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
}
void WebGlImplementation::GetIntegerv(GLenum pname, GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetIntegerv(pname, params);
}
void WebGlImplementation::GetProgramiv(GLuint program, GLenum pname, GLint* params)
{
  if(params) *params = 0;
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetProgramiv(program, pname, params);
}
void WebGlImplementation::GetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog)
{
  if(length) *length = 0;
  if(infolog && bufsize > 0) infolog[0] = 0;
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetProgramInfoLog(program, bufsize, length, infolog);
}
void WebGlImplementation::GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetRenderbufferParameteriv(target, pname, params);
}
void WebGlImplementation::GetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
  if(params) *params = 0;
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetShaderiv(shader, pname, params);
}
void WebGlImplementation::GetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog)
{
  if(length) *length = 0;
  if(infolog && bufsize > 0) infolog[0] = 0;
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetShaderInfoLog(shader, bufsize, length, infolog);
}
void WebGlImplementation::GetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
}
void WebGlImplementation::GetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source)
{
  if(length) *length = 0;
  if(source && bufsize > 0) source[0] = 0;
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetShaderSource(shader, bufsize, length, source);
}
const GLubyte* WebGlImplementation::GetString(GLenum name)
{
  if(!WebFailure::CheckGl())
  {
    return nullptr;
  }
  return glGetString(name);
}
void WebGlImplementation::GetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetTexParameterfv(target, pname, params);
}
void WebGlImplementation::GetTexParameteriv(GLenum target, GLenum pname, GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetTexParameteriv(target, pname, params);
}
void WebGlImplementation::GetUniformfv(GLuint program, GLint location, GLfloat* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetUniformfv(program, location, params);
}
void WebGlImplementation::GetUniformiv(GLuint program, GLint location, GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetUniformiv(program, location, params);
}
GLint WebGlImplementation::GetUniformLocation(GLuint program, const char* name)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glGetUniformLocation(program, name);
}
void WebGlImplementation::GetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetVertexAttribfv(index, pname, params);
}
void WebGlImplementation::GetVertexAttribiv(GLuint index, GLenum pname, GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetVertexAttribiv(index, pname, params);
}
void WebGlImplementation::GetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetVertexAttribPointerv(index, pname, pointer);
}
void WebGlImplementation::Hint(GLenum target, GLenum mode)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glHint(target, mode);
}
GLboolean WebGlImplementation::IsBuffer(GLuint buffer)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glIsBuffer(buffer);
}
GLboolean WebGlImplementation::IsEnabled(GLenum cap)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glIsEnabled(cap);
}
GLboolean WebGlImplementation::IsFramebuffer(GLuint framebuffer)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glIsFramebuffer(framebuffer);
}
GLboolean WebGlImplementation::IsProgram(GLuint program)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glIsProgram(program);
}
GLboolean WebGlImplementation::IsRenderbuffer(GLuint renderbuffer)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glIsRenderbuffer(renderbuffer);
}
GLboolean WebGlImplementation::IsShader(GLuint shader)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glIsShader(shader);
}
GLboolean WebGlImplementation::IsTexture(GLuint texture)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glIsTexture(texture);
}
void WebGlImplementation::LineWidth(GLfloat width)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glLineWidth(width);
}
void WebGlImplementation::LinkProgram(GLuint program)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glLinkProgram(program);
}
void WebGlImplementation::PixelStorei(GLenum pname, GLint param)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glPixelStorei(pname, param);
}
void WebGlImplementation::PolygonOffset(GLfloat factor, GLfloat units)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glPolygonOffset(factor, units);
}
void WebGlImplementation::ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glReadPixels(x, y, width, height, format, type, pixels);
}
void WebGlImplementation::ReleaseShaderCompiler(void)
{
  WebFailure::Latch("unsupported WebGL2 operation: ReleaseShaderCompiler");
  return;
}
void WebGlImplementation::RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glRenderbufferStorage(target, internalformat, width, height);
}
void WebGlImplementation::SampleCoverage(GLclampf value, GLboolean invert)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glSampleCoverage(value, invert);
}
void WebGlImplementation::Scissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glScissor(x, y, width, height);
}
void WebGlImplementation::ShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length)
{
  WebFailure::Latch("unsupported WebGL2 operation: ShaderBinary");
  return;
}
void WebGlImplementation::ShaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glShaderSource(shader, count, string, length);
}
void WebGlImplementation::StencilFunc(GLenum func, GLint ref, GLuint mask)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glStencilFunc(func, ref, mask);
}
void WebGlImplementation::StencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glStencilFuncSeparate(face, func, ref, mask);
}
void WebGlImplementation::StencilMask(GLuint mask)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glStencilMask(mask);
}
void WebGlImplementation::StencilMaskSeparate(GLenum face, GLuint mask)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glStencilMaskSeparate(face, mask);
}
void WebGlImplementation::StencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glStencilOp(fail, zfail, zpass);
}
void WebGlImplementation::StencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glStencilOpSeparate(face, fail, zfail, zpass);
}
void WebGlImplementation::TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  if(target == 0x8D65)
  {
    WebFailure::Latch("external texture unsupported"); return;
  }
  glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}
void WebGlImplementation::TexParameterf(GLenum target, GLenum pname, GLfloat param)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glTexParameterf(target, pname, param);
}
void WebGlImplementation::TexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glTexParameterfv(target, pname, params);
}
void WebGlImplementation::TexParameteri(GLenum target, GLenum pname, GLint param)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glTexParameteri(target, pname, param);
}
void WebGlImplementation::TexParameteriv(GLenum target, GLenum pname, const GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glTexParameteriv(target, pname, params);
}
void WebGlImplementation::TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  if(target == 0x8D65)
  {
    WebFailure::Latch("external texture unsupported"); return;
  }
  glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}
void WebGlImplementation::Uniform1f(GLint location, GLfloat x)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform1f(location, x);
}
void WebGlImplementation::Uniform1fv(GLint location, GLsizei count, const GLfloat* v)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform1fv(location, count, v);
}
void WebGlImplementation::Uniform1i(GLint location, GLint x)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform1i(location, x);
}
void WebGlImplementation::Uniform1iv(GLint location, GLsizei count, const GLint* v)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform1iv(location, count, v);
}
void WebGlImplementation::Uniform2f(GLint location, GLfloat x, GLfloat y)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform2f(location, x, y);
}
void WebGlImplementation::Uniform2fv(GLint location, GLsizei count, const GLfloat* v)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform2fv(location, count, v);
}
void WebGlImplementation::Uniform2i(GLint location, GLint x, GLint y)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform2i(location, x, y);
}
void WebGlImplementation::Uniform2iv(GLint location, GLsizei count, const GLint* v)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform2iv(location, count, v);
}
void WebGlImplementation::Uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform3f(location, x, y, z);
}
void WebGlImplementation::Uniform3fv(GLint location, GLsizei count, const GLfloat* v)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform3fv(location, count, v);
}
void WebGlImplementation::Uniform3i(GLint location, GLint x, GLint y, GLint z)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform3i(location, x, y, z);
}
void WebGlImplementation::Uniform3iv(GLint location, GLsizei count, const GLint* v)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform3iv(location, count, v);
}
void WebGlImplementation::Uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform4f(location, x, y, z, w);
}
void WebGlImplementation::Uniform4fv(GLint location, GLsizei count, const GLfloat* v)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform4fv(location, count, v);
}
void WebGlImplementation::Uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform4i(location, x, y, z, w);
}
void WebGlImplementation::Uniform4iv(GLint location, GLsizei count, const GLint* v)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform4iv(location, count, v);
}
void WebGlImplementation::UniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniformMatrix2fv(location, count, transpose, value);
}
void WebGlImplementation::UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniformMatrix3fv(location, count, transpose, value);
}
void WebGlImplementation::UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniformMatrix4fv(location, count, transpose, value);
}
void WebGlImplementation::UseProgram(GLuint program)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUseProgram(program);
}
void WebGlImplementation::ValidateProgram(GLuint program)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glValidateProgram(program);
}
void WebGlImplementation::VertexAttrib1f(GLuint indx, GLfloat x)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttrib1f(indx, x);
}
void WebGlImplementation::VertexAttrib1fv(GLuint indx, const GLfloat* values)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttrib1fv(indx, values);
}
void WebGlImplementation::VertexAttrib2f(GLuint indx, GLfloat x, GLfloat y)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttrib2f(indx, x, y);
}
void WebGlImplementation::VertexAttrib2fv(GLuint indx, const GLfloat* values)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttrib2fv(indx, values);
}
void WebGlImplementation::VertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttrib3f(indx, x, y, z);
}
void WebGlImplementation::VertexAttrib3fv(GLuint indx, const GLfloat* values)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttrib3fv(indx, values);
}
void WebGlImplementation::VertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttrib4f(indx, x, y, z, w);
}
void WebGlImplementation::VertexAttrib4fv(GLuint indx, const GLfloat* values)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttrib4fv(indx, values);
}
void WebGlImplementation::VertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttribPointer(indx, size, type, normalized, stride, ptr);
}
void WebGlImplementation::Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glViewport(x, y, width, height);
}
void WebGlImplementation::ReadBuffer(GLenum mode)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glReadBuffer(mode);
}
void WebGlImplementation::DrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid* indices)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDrawRangeElements(mode, start, end, count, type, indices);
}
void WebGlImplementation::TexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
}
void WebGlImplementation::TexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid* pixels)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}
void WebGlImplementation::CopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);
}
void WebGlImplementation::CompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid* data)
{
  WebFailure::Latch("unsupported WebGL2 operation: CompressedTexImage3D");
  return;
}
void WebGlImplementation::CompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid* data)
{
  WebFailure::Latch("unsupported WebGL2 operation: CompressedTexSubImage3D");
  return;
}
void WebGlImplementation::GenQueries(GLsizei n, GLuint* ids)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGenQueries(n, ids);
}
void WebGlImplementation::DeleteQueries(GLsizei n, const GLuint* ids)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDeleteQueries(n, ids);
}
GLboolean WebGlImplementation::IsQuery(GLuint id)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glIsQuery(id);
}
void WebGlImplementation::BeginQuery(GLenum target, GLuint id)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBeginQuery(target, id);
}
void WebGlImplementation::EndQuery(GLenum target)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glEndQuery(target);
}
void WebGlImplementation::GetQueryiv(GLenum target, GLenum pname, GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetQueryiv(target, pname, params);
}
void WebGlImplementation::GetQueryObjectuiv(GLuint id, GLenum pname, GLuint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetQueryObjectuiv(id, pname, params);
}
GLboolean WebGlImplementation::UnmapBuffer(GLenum target)
{
  WebFailure::Latch("unsupported WebGL2 operation: UnmapBuffer");
  return {};
}
void WebGlImplementation::GetBufferPointerv(GLenum target, GLenum pname, GLvoid** params)
{
  WebFailure::Latch("unsupported WebGL2 operation: GetBufferPointerv");
  return;
}
void WebGlImplementation::DrawBuffers(GLsizei n, const GLenum* bufs)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDrawBuffers(n, bufs);
}
void WebGlImplementation::UniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniformMatrix2x3fv(location, count, transpose, value);
}
void WebGlImplementation::UniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniformMatrix3x2fv(location, count, transpose, value);
}
void WebGlImplementation::UniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniformMatrix2x4fv(location, count, transpose, value);
}
void WebGlImplementation::UniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniformMatrix4x2fv(location, count, transpose, value);
}
void WebGlImplementation::UniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniformMatrix3x4fv(location, count, transpose, value);
}
void WebGlImplementation::UniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniformMatrix4x3fv(location, count, transpose, value);
}
void WebGlImplementation::BlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}
void WebGlImplementation::RenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
}
void WebGlImplementation::FramebufferTexture2DMultisample(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples)
{
  WebFailure::Latch("unsupported WebGL2 operation: FramebufferTexture2DMultisample");
  return;
}
void WebGlImplementation::FramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glFramebufferTextureLayer(target, attachment, texture, level, layer);
}
GLvoid* WebGlImplementation::MapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
  WebFailure::Latch("unsupported WebGL2 operation: MapBufferRange");
  return nullptr;
}
void WebGlImplementation::FlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length)
{
  WebFailure::Latch("unsupported WebGL2 operation: FlushMappedBufferRange");
  return;
}
void WebGlImplementation::BindVertexArray(GLuint array)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBindVertexArray(array);
}
void WebGlImplementation::DeleteVertexArrays(GLsizei n, const GLuint* arrays)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDeleteVertexArrays(n, arrays);
}
void WebGlImplementation::GenVertexArrays(GLsizei n, GLuint* arrays)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGenVertexArrays(n, arrays);
}
GLboolean WebGlImplementation::IsVertexArray(GLuint array)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glIsVertexArray(array);
}
void WebGlImplementation::GetIntegeri_v(GLenum target, GLuint index, GLint* data)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetIntegeri_v(target, index, data);
}
void WebGlImplementation::BeginTransformFeedback(GLenum primitiveMode)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBeginTransformFeedback(primitiveMode);
}
void WebGlImplementation::EndTransformFeedback(void)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glEndTransformFeedback();
}
void WebGlImplementation::BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBindBufferRange(target, index, buffer, offset, size);
}
void WebGlImplementation::BindBufferBase(GLenum target, GLuint index, GLuint buffer)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBindBufferBase(target, index, buffer);
}
void WebGlImplementation::TransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar* const* varyings, GLenum bufferMode)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glTransformFeedbackVaryings(program, count, varyings, bufferMode);
}
void WebGlImplementation::GetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetTransformFeedbackVarying(program, index, bufSize, length, size, type, name);
}
void WebGlImplementation::VertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttribIPointer(index, size, type, stride, pointer);
}
void WebGlImplementation::GetVertexAttribIiv(GLuint index, GLenum pname, GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetVertexAttribIiv(index, pname, params);
}
void WebGlImplementation::GetVertexAttribIuiv(GLuint index, GLenum pname, GLuint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetVertexAttribIuiv(index, pname, params);
}
void WebGlImplementation::VertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttribI4i(index, x, y, z, w);
}
void WebGlImplementation::VertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttribI4ui(index, x, y, z, w);
}
void WebGlImplementation::VertexAttribI4iv(GLuint index, const GLint* v)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttribI4iv(index, v);
}
void WebGlImplementation::VertexAttribI4uiv(GLuint index, const GLuint* v)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttribI4uiv(index, v);
}
void WebGlImplementation::GetUniformuiv(GLuint program, GLint location, GLuint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetUniformuiv(program, location, params);
}
GLint WebGlImplementation::GetFragDataLocation(GLuint program, const GLchar *name)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glGetFragDataLocation(program, name);
}
void WebGlImplementation::Uniform1ui(GLint location, GLuint v0)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform1ui(location, v0);
}
void WebGlImplementation::Uniform2ui(GLint location, GLuint v0, GLuint v1)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform2ui(location, v0, v1);
}
void WebGlImplementation::Uniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform3ui(location, v0, v1, v2);
}
void WebGlImplementation::Uniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform4ui(location, v0, v1, v2, v3);
}
void WebGlImplementation::Uniform1uiv(GLint location, GLsizei count, const GLuint* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform1uiv(location, count, value);
}
void WebGlImplementation::Uniform2uiv(GLint location, GLsizei count, const GLuint* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform2uiv(location, count, value);
}
void WebGlImplementation::Uniform3uiv(GLint location, GLsizei count, const GLuint* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform3uiv(location, count, value);
}
void WebGlImplementation::Uniform4uiv(GLint location, GLsizei count, const GLuint* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniform4uiv(location, count, value);
}
void WebGlImplementation::ClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glClearBufferiv(buffer, drawbuffer, value);
}
void WebGlImplementation::ClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glClearBufferuiv(buffer, drawbuffer, value);
}
void WebGlImplementation::ClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glClearBufferfv(buffer, drawbuffer, value);
}
void WebGlImplementation::ClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glClearBufferfi(buffer, drawbuffer, depth, stencil);
}
const GLubyte* WebGlImplementation::GetStringi(GLenum name, GLuint index)
{
  if(!WebFailure::CheckGl())
  {
    return nullptr;
  }
  return glGetStringi(name, index);
}
void WebGlImplementation::CopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glCopyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size);
}
void WebGlImplementation::GetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar* const* uniformNames, GLuint* uniformIndices)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetUniformIndices(program, uniformCount, uniformNames, uniformIndices);
}
void WebGlImplementation::GetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetActiveUniformsiv(program, uniformCount, uniformIndices, pname, params);
}
GLuint WebGlImplementation::GetUniformBlockIndex(GLuint program, const GLchar* uniformBlockName)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glGetUniformBlockIndex(program, uniformBlockName);
}
void WebGlImplementation::GetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params);
}
void WebGlImplementation::GetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetActiveUniformBlockName(program, uniformBlockIndex, bufSize, length, uniformBlockName);
}
void WebGlImplementation::UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
}
void WebGlImplementation::DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDrawArraysInstanced(mode, first, count, instanceCount);
}
void WebGlImplementation::DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLsizei instanceCount)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDrawElementsInstanced(mode, count, type, indices, instanceCount);
}
GLsync WebGlImplementation::FenceSync(GLenum condition, GLbitfield flags)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return reinterpret_cast<Dali::GLsync>(glFenceSync(condition, flags));
}
GLboolean WebGlImplementation::IsSync(GLsync sync)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glIsSync(reinterpret_cast<::GLsync>(sync));
}
void WebGlImplementation::DeleteSync(GLsync sync)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDeleteSync(reinterpret_cast<::GLsync>(sync));
}
GLenum WebGlImplementation::ClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glClientWaitSync(reinterpret_cast<::GLsync>(sync), flags, timeout);
}
void WebGlImplementation::WaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glWaitSync(reinterpret_cast<::GLsync>(sync), flags, timeout);
}
void WebGlImplementation::GetInteger64v(GLenum pname, GLint64* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetInteger64v(pname, params);
}
void WebGlImplementation::GetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetSynciv(reinterpret_cast<::GLsync>(sync), pname, bufSize, length, values);
}
void WebGlImplementation::GetInteger64i_v(GLenum target, GLuint index, GLint64* data)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetInteger64i_v(target, index, data);
}
void WebGlImplementation::GetBufferParameteri64v(GLenum target, GLenum pname, GLint64* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetBufferParameteri64v(target, pname, params);
}
void WebGlImplementation::GenSamplers(GLsizei count, GLuint* samplers)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGenSamplers(count, samplers);
}
void WebGlImplementation::DeleteSamplers(GLsizei count, const GLuint* samplers)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDeleteSamplers(count, samplers);
}
GLboolean WebGlImplementation::IsSampler(GLuint sampler)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glIsSampler(sampler);
}
void WebGlImplementation::BindSampler(GLuint unit, GLuint sampler)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBindSampler(unit, sampler);
}
void WebGlImplementation::SamplerParameteri(GLuint sampler, GLenum pname, GLint param)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glSamplerParameteri(sampler, pname, param);
}
void WebGlImplementation::SamplerParameteriv(GLuint sampler, GLenum pname, const GLint* param)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glSamplerParameteriv(sampler, pname, param);
}
void WebGlImplementation::SamplerParameterf(GLuint sampler, GLenum pname, GLfloat param)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glSamplerParameterf(sampler, pname, param);
}
void WebGlImplementation::SamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat* param)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glSamplerParameterfv(sampler, pname, param);
}
void WebGlImplementation::GetSamplerParameteriv(GLuint sampler, GLenum pname, GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetSamplerParameteriv(sampler, pname, params);
}
void WebGlImplementation::GetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetSamplerParameterfv(sampler, pname, params);
}
void WebGlImplementation::VertexAttribDivisor(GLuint index, GLuint divisor)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glVertexAttribDivisor(index, divisor);
}
void WebGlImplementation::BindTransformFeedback(GLenum target, GLuint id)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glBindTransformFeedback(target, id);
}
void WebGlImplementation::DeleteTransformFeedbacks(GLsizei n, const GLuint* ids)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glDeleteTransformFeedbacks(n, ids);
}
void WebGlImplementation::GenTransformFeedbacks(GLsizei n, GLuint* ids)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGenTransformFeedbacks(n, ids);
}
GLboolean WebGlImplementation::IsTransformFeedback(GLuint id)
{
  if(!WebFailure::CheckGl())
  {
    return {};
  }
  return glIsTransformFeedback(id);
}
void WebGlImplementation::PauseTransformFeedback(void)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glPauseTransformFeedback();
}
void WebGlImplementation::ResumeTransformFeedback(void)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glResumeTransformFeedback();
}
void WebGlImplementation::GetProgramBinary(GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, GLvoid* binary)
{
  WebFailure::Latch("unsupported WebGL2 operation: GetProgramBinary");
  return;
}
void WebGlImplementation::ProgramBinary(GLuint program, GLenum binaryFormat, const GLvoid* binary, GLsizei length)
{
  WebFailure::Latch("unsupported WebGL2 operation: ProgramBinary");
  return;
}
void WebGlImplementation::ProgramParameteri(GLuint program, GLenum pname, GLint value)
{
  WebFailure::Latch("unsupported WebGL2 operation: ProgramParameteri");
  return;
}
void WebGlImplementation::InvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum* attachments)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glInvalidateFramebuffer(target, numAttachments, attachments);
}
void WebGlImplementation::InvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glInvalidateSubFramebuffer(target, numAttachments, attachments, x, y, width, height);
}
void WebGlImplementation::TexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glTexStorage2D(target, levels, internalformat, width, height);
}
void WebGlImplementation::TexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glTexStorage3D(target, levels, internalformat, width, height, depth);
}
void WebGlImplementation::GetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params)
{
  if(!WebFailure::CheckGl())
  {
    return;
  }
  glGetInternalformativ(target, internalformat, pname, bufSize, params);
}
void WebGlImplementation::BlendBarrier(void)
{
  WebFailure::Latch("unsupported WebGL2 operation: BlendBarrier");
  return;
}
}
