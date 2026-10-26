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


#pragma once
#include <dali/integration-api/gl-abstraction.h>
namespace DALI_NAMESPACE::Graphics::GLES
{
// WebGL2 wrapper; unsupported GLES calls latch failure instead of importing
// native entry points or returning success. Owned by the render pthread.
class WebGlImplementation : public Integration::GlAbstraction
{
public:
  bool ContextCreated();
  int GetMaxTextureSize() const
  {
    return mMaxTextureSize;
  }
  int GetMaxCombinedTextureUnits() const
  {
    return mMaxTextureUnits;
  }
  void PreRender() override;
  void PostRender() override;
  bool IsSurfacelessContextSupported() const override;
  bool IsAdvancedBlendEquationSupported() override;
  bool IsMultisampledRenderToTextureSupported() override;
  bool IsBlendEquationSupported(Dali::BlendEquation::Type blendEquation) override;
  uint32_t GetShaderLanguageVersion() override;
  std::string GetShaderVersionPrefix() override;
  std::string GetVertexShaderPrefix() override;
  std::string GetFragmentShaderPrefix() override;
  bool TextureRequiresConverting(const GLenum imageGlFormat, const GLenum textureGlFormat, const bool isSubImage) const override;
  void ActiveTexture(GLenum texture) override;
  void AttachShader(GLuint program, GLuint shader) override;
  void BindAttribLocation(GLuint program, GLuint index, const char* name) override;
  void BindBuffer(GLenum target, GLuint buffer) override;
  void BindFramebuffer(GLenum target, GLuint framebuffer) override;
  void BindRenderbuffer(GLenum target, GLuint renderbuffer) override;
  void BindTexture(GLenum target, GLuint texture) override;
  void BlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) override;
  void BlendEquation( GLenum mode ) override;
  void BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) override;
  void BlendFunc(GLenum sfactor, GLenum dfactor) override;
  void BlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) override;
  void BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) override;
  void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data) override;
  GLenum CheckFramebufferStatus(GLenum target) override;
  void Clear(GLbitfield mask) override;
  void ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) override;
  void ClearDepthf(GLclampf depth) override;
  void ClearStencil(GLint s) override;
  void ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) override;
  void CompileShader(GLuint shader) override;
  void CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data) override;
  void CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data) override;
  void CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) override;
  void CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) override;
  GLuint CreateProgram(void) override;
  GLuint CreateShader(GLenum type) override;
  void CullFace(GLenum mode) override;
  void DeleteBuffers(GLsizei n, const GLuint* buffers) override;
  void DeleteFramebuffers(GLsizei n, const GLuint* framebuffers) override;
  void DeleteProgram(GLuint program) override;
  void DeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers) override;
  void DeleteShader(GLuint shader) override;
  void DeleteTextures(GLsizei n, const GLuint* textures) override;
  void DepthFunc(GLenum func) override;
  void DepthMask(GLboolean flag) override;
  void DepthRangef(GLclampf zNear, GLclampf zFar) override;
  void DetachShader(GLuint program, GLuint shader) override;
  void Disable(GLenum cap) override;
  void DisableVertexAttribArray(GLuint index) override;
  void DrawArrays(GLenum mode, GLint first, GLsizei count) override;
  void DrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) override;
  void Enable(GLenum cap) override;
  void EnableVertexAttribArray(GLuint index) override;
  void Finish(void) override;
  void Flush(void) override;
  void FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) override;
  void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) override;
  void FrontFace(GLenum mode) override;
  void GenBuffers(GLsizei n, GLuint* buffers) override;
  void GenerateMipmap(GLenum target) override;
  void GenFramebuffers(GLsizei n, GLuint* framebuffers) override;
  void GenRenderbuffers(GLsizei n, GLuint* renderbuffers) override;
  void GenTextures(GLsizei n, GLuint* textures) override;
  void GetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name) override;
  void GetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name) override;
  void GetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders) override;
  GLint GetAttribLocation(GLuint program, const char* name) override;
  void GetBooleanv(GLenum pname, GLboolean* params) override;
  void GetBufferParameteriv(GLenum target, GLenum pname, GLint* params) override;
  GLenum GetError(void) override;
  void GetFloatv(GLenum pname, GLfloat* params) override;
  void GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params) override;
  void GetIntegerv(GLenum pname, GLint* params) override;
  void GetProgramiv(GLuint program, GLenum pname, GLint* params) override;
  void GetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog) override;
  void GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params) override;
  void GetShaderiv(GLuint shader, GLenum pname, GLint* params) override;
  void GetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog) override;
  void GetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision) override;
  void GetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source) override;
  const GLubyte* GetString(GLenum name) override;
  void GetTexParameterfv(GLenum target, GLenum pname, GLfloat* params) override;
  void GetTexParameteriv(GLenum target, GLenum pname, GLint* params) override;
  void GetUniformfv(GLuint program, GLint location, GLfloat* params) override;
  void GetUniformiv(GLuint program, GLint location, GLint* params) override;
  GLint GetUniformLocation(GLuint program, const char* name) override;
  void GetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params) override;
  void GetVertexAttribiv(GLuint index, GLenum pname, GLint* params) override;
  void GetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer) override;
  void Hint(GLenum target, GLenum mode) override;
  GLboolean IsBuffer(GLuint buffer) override;
  GLboolean IsEnabled(GLenum cap) override;
  GLboolean IsFramebuffer(GLuint framebuffer) override;
  GLboolean IsProgram(GLuint program) override;
  GLboolean IsRenderbuffer(GLuint renderbuffer) override;
  GLboolean IsShader(GLuint shader) override;
  GLboolean IsTexture(GLuint texture) override;
  void LineWidth(GLfloat width) override;
  void LinkProgram(GLuint program) override;
  void PixelStorei(GLenum pname, GLint param) override;
  void PolygonOffset(GLfloat factor, GLfloat units) override;
  void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels) override;
  void ReleaseShaderCompiler(void) override;
  void RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) override;
  void SampleCoverage(GLclampf value, GLboolean invert) override;
  void Scissor(GLint x, GLint y, GLsizei width, GLsizei height) override;
  void ShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length) override;
  void ShaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length) override;
  void StencilFunc(GLenum func, GLint ref, GLuint mask) override;
  void StencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask) override;
  void StencilMask(GLuint mask) override;
  void StencilMaskSeparate(GLenum face, GLuint mask) override;
  void StencilOp(GLenum fail, GLenum zfail, GLenum zpass) override;
  void StencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass) override;
  void TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels) override;
  void TexParameterf(GLenum target, GLenum pname, GLfloat param) override;
  void TexParameterfv(GLenum target, GLenum pname, const GLfloat* params) override;
  void TexParameteri(GLenum target, GLenum pname, GLint param) override;
  void TexParameteriv(GLenum target, GLenum pname, const GLint* params) override;
  void TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) override;
  void Uniform1f(GLint location, GLfloat x) override;
  void Uniform1fv(GLint location, GLsizei count, const GLfloat* v) override;
  void Uniform1i(GLint location, GLint x) override;
  void Uniform1iv(GLint location, GLsizei count, const GLint* v) override;
  void Uniform2f(GLint location, GLfloat x, GLfloat y) override;
  void Uniform2fv(GLint location, GLsizei count, const GLfloat* v) override;
  void Uniform2i(GLint location, GLint x, GLint y) override;
  void Uniform2iv(GLint location, GLsizei count, const GLint* v) override;
  void Uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z) override;
  void Uniform3fv(GLint location, GLsizei count, const GLfloat* v) override;
  void Uniform3i(GLint location, GLint x, GLint y, GLint z) override;
  void Uniform3iv(GLint location, GLsizei count, const GLint* v) override;
  void Uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) override;
  void Uniform4fv(GLint location, GLsizei count, const GLfloat* v) override;
  void Uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w) override;
  void Uniform4iv(GLint location, GLsizei count, const GLint* v) override;
  void UniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override;
  void UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override;
  void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override;
  void UseProgram(GLuint program) override;
  void ValidateProgram(GLuint program) override;
  void VertexAttrib1f(GLuint indx, GLfloat x) override;
  void VertexAttrib1fv(GLuint indx, const GLfloat* values) override;
  void VertexAttrib2f(GLuint indx, GLfloat x, GLfloat y) override;
  void VertexAttrib2fv(GLuint indx, const GLfloat* values) override;
  void VertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z) override;
  void VertexAttrib3fv(GLuint indx, const GLfloat* values) override;
  void VertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w) override;
  void VertexAttrib4fv(GLuint indx, const GLfloat* values) override;
  void VertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr) override;
  void Viewport(GLint x, GLint y, GLsizei width, GLsizei height) override;
  void ReadBuffer(GLenum mode) override;
  void DrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid* indices) override;
  void TexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels) override;
  void TexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid* pixels) override;
  void CopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) override;
  void CompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid* data) override;
  void CompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid* data) override;
  void GenQueries(GLsizei n, GLuint* ids) override;
  void DeleteQueries(GLsizei n, const GLuint* ids) override;
  GLboolean IsQuery(GLuint id) override;
  void BeginQuery(GLenum target, GLuint id) override;
  void EndQuery(GLenum target) override;
  void GetQueryiv(GLenum target, GLenum pname, GLint* params) override;
  void GetQueryObjectuiv(GLuint id, GLenum pname, GLuint* params) override;
  GLboolean UnmapBuffer(GLenum target) override;
  void GetBufferPointerv(GLenum target, GLenum pname, GLvoid** params) override;
  void DrawBuffers(GLsizei n, const GLenum* bufs) override;
  void UniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override;
  void UniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override;
  void UniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override;
  void UniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override;
  void UniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override;
  void UniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override;
  void BlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) override;
  void RenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) override;
  void FramebufferTexture2DMultisample(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples) override;
  void FramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer) override;
  GLvoid* MapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) override;
  void FlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length) override;
  void BindVertexArray(GLuint array) override;
  void DeleteVertexArrays(GLsizei n, const GLuint* arrays) override;
  void GenVertexArrays(GLsizei n, GLuint* arrays) override;
  GLboolean IsVertexArray(GLuint array) override;
  void GetIntegeri_v(GLenum target, GLuint index, GLint* data) override;
  void BeginTransformFeedback(GLenum primitiveMode) override;
  void EndTransformFeedback(void) override;
  void BindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) override;
  void BindBufferBase(GLenum target, GLuint index, GLuint buffer) override;
  void TransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar* const* varyings, GLenum bufferMode) override;
  void GetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name) override;
  void VertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) override;
  void GetVertexAttribIiv(GLuint index, GLenum pname, GLint* params) override;
  void GetVertexAttribIuiv(GLuint index, GLenum pname, GLuint* params) override;
  void VertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w) override;
  void VertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w) override;
  void VertexAttribI4iv(GLuint index, const GLint* v) override;
  void VertexAttribI4uiv(GLuint index, const GLuint* v) override;
  void GetUniformuiv(GLuint program, GLint location, GLuint* params) override;
  GLint GetFragDataLocation(GLuint program, const GLchar *name) override;
  void Uniform1ui(GLint location, GLuint v0) override;
  void Uniform2ui(GLint location, GLuint v0, GLuint v1) override;
  void Uniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2) override;
  void Uniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) override;
  void Uniform1uiv(GLint location, GLsizei count, const GLuint* value) override;
  void Uniform2uiv(GLint location, GLsizei count, const GLuint* value) override;
  void Uniform3uiv(GLint location, GLsizei count, const GLuint* value) override;
  void Uniform4uiv(GLint location, GLsizei count, const GLuint* value) override;
  void ClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint* value) override;
  void ClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint* value) override;
  void ClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat* value) override;
  void ClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) override;
  const GLubyte* GetStringi(GLenum name, GLuint index) override;
  void CopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) override;
  void GetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar* const* uniformNames, GLuint* uniformIndices) override;
  void GetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params) override;
  GLuint GetUniformBlockIndex(GLuint program, const GLchar* uniformBlockName) override;
  void GetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params) override;
  void GetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName) override;
  void UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding) override;
  void DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount) override;
  void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLsizei instanceCount) override;
  GLsync FenceSync(GLenum condition, GLbitfield flags) override;
  GLboolean IsSync(GLsync sync) override;
  void DeleteSync(GLsync sync) override;
  GLenum ClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout) override;
  void WaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout) override;
  void GetInteger64v(GLenum pname, GLint64* params) override;
  void GetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values) override;
  void GetInteger64i_v(GLenum target, GLuint index, GLint64* data) override;
  void GetBufferParameteri64v(GLenum target, GLenum pname, GLint64* params) override;
  void GenSamplers(GLsizei count, GLuint* samplers) override;
  void DeleteSamplers(GLsizei count, const GLuint* samplers) override;
  GLboolean IsSampler(GLuint sampler) override;
  void BindSampler(GLuint unit, GLuint sampler) override;
  void SamplerParameteri(GLuint sampler, GLenum pname, GLint param) override;
  void SamplerParameteriv(GLuint sampler, GLenum pname, const GLint* param) override;
  void SamplerParameterf(GLuint sampler, GLenum pname, GLfloat param) override;
  void SamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat* param) override;
  void GetSamplerParameteriv(GLuint sampler, GLenum pname, GLint* params) override;
  void GetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat* params) override;
  void VertexAttribDivisor(GLuint index, GLuint divisor) override;
  void BindTransformFeedback(GLenum target, GLuint id) override;
  void DeleteTransformFeedbacks(GLsizei n, const GLuint* ids) override;
  void GenTransformFeedbacks(GLsizei n, GLuint* ids) override;
  GLboolean IsTransformFeedback(GLuint id) override;
  void PauseTransformFeedback(void) override;
  void ResumeTransformFeedback(void) override;
  void GetProgramBinary(GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, GLvoid* binary) override;
  void ProgramBinary(GLuint program, GLenum binaryFormat, const GLvoid* binary, GLsizei length) override;
  void ProgramParameteri(GLuint program, GLenum pname, GLint value) override;
  void InvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum* attachments) override;
  void InvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height) override;
  void TexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) override;
  void TexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) override;
  void GetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params) override;
  void BlendBarrier(void) override;
private:
  int mMaxTextureSize{0};
  int mMaxTextureUnits{0};
  bool mReady{false};
};
}
