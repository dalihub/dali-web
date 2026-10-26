// Test-only GL responses. Every unexpected call aborts.
#pragma once
#include <dali/integration-api/gl-abstraction.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>
namespace Dali
{
class MemoryTestGl : public Integration::GlAbstraction
{
public:
  void PreRender() override
  {
    std::fprintf(stderr, "Unexpected GL: PreRender\n"); std::abort();
  }
  void PostRender() override
  {
    std::fprintf(stderr, "Unexpected GL: PostRender\n"); std::abort();
  }
  bool IsSurfacelessContextSupported() const override
  {
    std::fprintf(stderr, "Unexpected GL: IsSurfacelessContextSupported\n"); std::abort();
  }
  bool IsAdvancedBlendEquationSupported() override
  {
    std::fprintf(stderr, "Unexpected GL: IsAdvancedBlendEquationSupported\n"); std::abort();
  }
  bool IsMultisampledRenderToTextureSupported() override
  {
    std::fprintf(stderr, "Unexpected GL: IsMultisampledRenderToTextureSupported\n"); std::abort();
  }
  bool IsBlendEquationSupported(Dali::BlendEquation::Type blendEquation) override
  {
    std::fprintf(stderr, "Unexpected GL: IsBlendEquationSupported\n"); std::abort();
  }
  uint32_t GetShaderLanguageVersion() override
  {
    std::fprintf(stderr, "Unexpected GL: GetShaderLanguageVersion\n"); std::abort();
  }
  std::string GetShaderVersionPrefix() override
  {
    std::fprintf(stderr, "Unexpected GL: GetShaderVersionPrefix\n"); std::abort();
  }
  std::string GetVertexShaderPrefix() override
  {
    std::fprintf(stderr, "Unexpected GL: GetVertexShaderPrefix\n"); std::abort();
  }
  std::string GetFragmentShaderPrefix() override
  {
    std::fprintf(stderr, "Unexpected GL: GetFragmentShaderPrefix\n"); std::abort();
  }
  bool TextureRequiresConverting(const GLenum imageGlFormat, const GLenum textureGlFormat, const bool isSubImage) const override
  {
    std::fprintf(stderr, "Unexpected GL: TextureRequiresConverting\n"); std::abort();
  }
  void ActiveTexture (GLenum texture) override
  {
    std::fprintf(stderr, "Unexpected GL: ActiveTexture\n"); std::abort();
  }
  void AttachShader (GLuint program, GLuint shader) override
  {
    std::fprintf(stderr, "Unexpected GL: AttachShader\n"); std::abort();
  }
  void BindAttribLocation (GLuint program, GLuint index, const char* name) override
  {
    std::fprintf(stderr, "Unexpected GL: BindAttribLocation\n"); std::abort();
  }
  void BindBuffer (GLenum target, GLuint buffer) override
  {
    std::fprintf(stderr, "Unexpected GL: BindBuffer\n"); std::abort();
  }
  void BindFramebuffer (GLenum target, GLuint framebuffer) override
  {
    std::fprintf(stderr, "Unexpected GL: BindFramebuffer\n"); std::abort();
  }
  void BindRenderbuffer (GLenum target, GLuint renderbuffer) override
  {
    std::fprintf(stderr, "Unexpected GL: BindRenderbuffer\n"); std::abort();
  }
  void BindTexture (GLenum target, GLuint texture) override
  {
    std::fprintf(stderr, "Unexpected GL: BindTexture\n"); std::abort();
  }
  void BlendColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) override
  {
    std::fprintf(stderr, "Unexpected GL: BlendColor\n"); std::abort();
  }
  void BlendEquation ( GLenum mode ) override
  {
    std::fprintf(stderr, "Unexpected GL: BlendEquation\n"); std::abort();
  }
  void BlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha) override
  {
    std::fprintf(stderr, "Unexpected GL: BlendEquationSeparate\n"); std::abort();
  }
  void BlendFunc (GLenum sfactor, GLenum dfactor) override
  {
    std::fprintf(stderr, "Unexpected GL: BlendFunc\n"); std::abort();
  }
  void BlendFuncSeparate (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) override
  {
    std::fprintf(stderr, "Unexpected GL: BlendFuncSeparate\n"); std::abort();
  }
  void BufferData (GLenum target, GLsizeiptr size, const void* data, GLenum usage) override
  {
    std::fprintf(stderr, "Unexpected GL: BufferData\n"); std::abort();
  }
  void BufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const void* data) override
  {
    std::fprintf(stderr, "Unexpected GL: BufferSubData\n"); std::abort();
  }
  GLenum CheckFramebufferStatus (GLenum target) override
  {
    std::fprintf(stderr, "Unexpected GL: CheckFramebufferStatus\n"); std::abort();
  }
  void Clear (GLbitfield mask) override
  {
    std::fprintf(stderr, "Unexpected GL: Clear\n"); std::abort();
  }
  void ClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) override
  {
    std::fprintf(stderr, "Unexpected GL: ClearColor\n"); std::abort();
  }
  void ClearDepthf (GLclampf depth) override
  {
    std::fprintf(stderr, "Unexpected GL: ClearDepthf\n"); std::abort();
  }
  void ClearStencil (GLint s) override
  {
    std::fprintf(stderr, "Unexpected GL: ClearStencil\n"); std::abort();
  }
  void ColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) override
  {
    std::fprintf(stderr, "Unexpected GL: ColorMask\n"); std::abort();
  }
  void CompileShader (GLuint shader) override
  {
    std::fprintf(stderr, "Unexpected GL: CompileShader\n"); std::abort();
  }
  void CompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data) override
  {
    std::fprintf(stderr, "Unexpected GL: CompressedTexImage2D\n"); std::abort();
  }
  void CompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data) override
  {
    std::fprintf(stderr, "Unexpected GL: CompressedTexSubImage2D\n"); std::abort();
  }
  void CopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) override
  {
    std::fprintf(stderr, "Unexpected GL: CopyTexImage2D\n"); std::abort();
  }
  void CopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) override
  {
    std::fprintf(stderr, "Unexpected GL: CopyTexSubImage2D\n"); std::abort();
  }
  GLuint CreateProgram (void) override
  {
    std::fprintf(stderr, "Unexpected GL: CreateProgram\n"); std::abort();
  }
  GLuint CreateShader (GLenum type) override
  {
    std::fprintf(stderr, "Unexpected GL: CreateShader\n"); std::abort();
  }
  void CullFace (GLenum mode) override
  {
    std::fprintf(stderr, "Unexpected GL: CullFace\n"); std::abort();
  }
  void DeleteBuffers (GLsizei n, const GLuint* buffers) override
  {
    std::fprintf(stderr, "Unexpected GL: DeleteBuffers\n"); std::abort();
  }
  void DeleteFramebuffers (GLsizei n, const GLuint* framebuffers) override
  {
    std::fprintf(stderr, "Unexpected GL: DeleteFramebuffers\n"); std::abort();
  }
  void DeleteProgram (GLuint program) override
  {
    std::fprintf(stderr, "Unexpected GL: DeleteProgram\n"); std::abort();
  }
  void DeleteRenderbuffers (GLsizei n, const GLuint* renderbuffers) override
  {
    std::fprintf(stderr, "Unexpected GL: DeleteRenderbuffers\n"); std::abort();
  }
  void DeleteShader (GLuint shader) override
  {
    std::fprintf(stderr, "Unexpected GL: DeleteShader\n"); std::abort();
  }
  void DeleteTextures (GLsizei n, const GLuint* textures) override
  {
    std::fprintf(stderr, "Unexpected GL: DeleteTextures\n"); std::abort();
  }
  void DepthFunc (GLenum func) override
  {
    std::fprintf(stderr, "Unexpected GL: DepthFunc\n"); std::abort();
  }
  void DepthMask (GLboolean flag) override
  {
    std::fprintf(stderr, "Unexpected GL: DepthMask\n"); std::abort();
  }
  void DepthRangef (GLclampf zNear, GLclampf zFar) override
  {
    std::fprintf(stderr, "Unexpected GL: DepthRangef\n"); std::abort();
  }
  void DetachShader (GLuint program, GLuint shader) override
  {
    std::fprintf(stderr, "Unexpected GL: DetachShader\n"); std::abort();
  }
  void Disable (GLenum cap) override
  {
    assert(cap == 0x0BD0);
  }
  void DisableVertexAttribArray (GLuint index) override
  {
    std::fprintf(stderr, "Unexpected GL: DisableVertexAttribArray\n"); std::abort();
  }
  void DrawArrays (GLenum mode, GLint first, GLsizei count) override
  {
    std::fprintf(stderr, "Unexpected GL: DrawArrays\n"); std::abort();
  }
  void DrawElements (GLenum mode, GLsizei count, GLenum type, const void* indices) override
  {
    std::fprintf(stderr, "Unexpected GL: DrawElements\n"); std::abort();
  }
  void Enable (GLenum cap) override
  {
    std::fprintf(stderr, "Unexpected GL: Enable\n"); std::abort();
  }
  void EnableVertexAttribArray (GLuint index) override
  {
    std::fprintf(stderr, "Unexpected GL: EnableVertexAttribArray\n"); std::abort();
  }
  void Finish (void) override
  {
    std::fprintf(stderr, "Unexpected GL: Finish\n"); std::abort();
  }
  void Flush (void) override
  {
    std::fprintf(stderr, "Unexpected GL: Flush\n"); std::abort();
  }
  void FramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) override
  {
    std::fprintf(stderr, "Unexpected GL: FramebufferRenderbuffer\n"); std::abort();
  }
  void FramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) override
  {
    std::fprintf(stderr, "Unexpected GL: FramebufferTexture2D\n"); std::abort();
  }
  void FrontFace (GLenum mode) override
  {
    std::fprintf(stderr, "Unexpected GL: FrontFace\n"); std::abort();
  }
  void GenBuffers (GLsizei n, GLuint* buffers) override
  {
    std::fprintf(stderr, "Unexpected GL: GenBuffers\n"); std::abort();
  }
  void GenerateMipmap (GLenum target) override
  {
    std::fprintf(stderr, "Unexpected GL: GenerateMipmap\n"); std::abort();
  }
  void GenFramebuffers (GLsizei n, GLuint* framebuffers) override
  {
    std::fprintf(stderr, "Unexpected GL: GenFramebuffers\n"); std::abort();
  }
  void GenRenderbuffers (GLsizei n, GLuint* renderbuffers) override
  {
    std::fprintf(stderr, "Unexpected GL: GenRenderbuffers\n"); std::abort();
  }
  void GenTextures (GLsizei n, GLuint* textures) override
  {
    std::fprintf(stderr, "Unexpected GL: GenTextures\n"); std::abort();
  }
  void GetActiveAttrib (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name) override
  {
    std::fprintf(stderr, "Unexpected GL: GetActiveAttrib\n"); std::abort();
  }
  void GetActiveUniform (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name) override
  {
    std::fprintf(stderr, "Unexpected GL: GetActiveUniform\n"); std::abort();
  }
  void GetAttachedShaders (GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders) override
  {
    std::fprintf(stderr, "Unexpected GL: GetAttachedShaders\n"); std::abort();
  }
  GLint GetAttribLocation (GLuint program, const char* name) override
  {
    std::fprintf(stderr, "Unexpected GL: GetAttribLocation\n"); std::abort();
  }
  void GetBooleanv (GLenum pname, GLboolean* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetBooleanv\n"); std::abort();
  }
  void GetBufferParameteriv (GLenum target, GLenum pname, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetBufferParameteriv\n"); std::abort();
  }
  GLenum GetError (void) override
  {
    std::fprintf(stderr, "Unexpected GL: GetError\n"); std::abort();
  }
  void GetFloatv (GLenum pname, GLfloat* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetFloatv\n"); std::abort();
  }
  void GetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetFramebufferAttachmentParameteriv\n"); std::abort();
  }
  void GetIntegerv (GLenum pname, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetIntegerv\n"); std::abort();
  }
  void GetProgramiv (GLuint program, GLenum pname, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetProgramiv\n"); std::abort();
  }
  void GetProgramInfoLog (GLuint program, GLsizei bufsize, GLsizei* length, char* infolog) override
  {
    std::fprintf(stderr, "Unexpected GL: GetProgramInfoLog\n"); std::abort();
  }
  void GetRenderbufferParameteriv (GLenum target, GLenum pname, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetRenderbufferParameteriv\n"); std::abort();
  }
  void GetShaderiv (GLuint shader, GLenum pname, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetShaderiv\n"); std::abort();
  }
  void GetShaderInfoLog (GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog) override
  {
    std::fprintf(stderr, "Unexpected GL: GetShaderInfoLog\n"); std::abort();
  }
  void GetShaderPrecisionFormat (GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision) override
  {
    std::fprintf(stderr, "Unexpected GL: GetShaderPrecisionFormat\n"); std::abort();
  }
  void GetShaderSource (GLuint shader, GLsizei bufsize, GLsizei* length, char* source) override
  {
    std::fprintf(stderr, "Unexpected GL: GetShaderSource\n"); std::abort();
  }
  const GLubyte* GetString (GLenum name) override
  {
    std::fprintf(stderr, "Unexpected GL: GetString\n"); std::abort();
  }
  void GetTexParameterfv (GLenum target, GLenum pname, GLfloat* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetTexParameterfv\n"); std::abort();
  }
  void GetTexParameteriv (GLenum target, GLenum pname, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetTexParameteriv\n"); std::abort();
  }
  void GetUniformfv (GLuint program, GLint location, GLfloat* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetUniformfv\n"); std::abort();
  }
  void GetUniformiv (GLuint program, GLint location, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetUniformiv\n"); std::abort();
  }
  GLint GetUniformLocation (GLuint program, const char* name) override
  {
    std::fprintf(stderr, "Unexpected GL: GetUniformLocation\n"); std::abort();
  }
  void GetVertexAttribfv (GLuint index, GLenum pname, GLfloat* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetVertexAttribfv\n"); std::abort();
  }
  void GetVertexAttribiv (GLuint index, GLenum pname, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetVertexAttribiv\n"); std::abort();
  }
  void GetVertexAttribPointerv (GLuint index, GLenum pname, void** pointer) override
  {
    std::fprintf(stderr, "Unexpected GL: GetVertexAttribPointerv\n"); std::abort();
  }
  void Hint (GLenum target, GLenum mode) override
  {
    std::fprintf(stderr, "Unexpected GL: Hint\n"); std::abort();
  }
  GLboolean IsBuffer (GLuint buffer) override
  {
    std::fprintf(stderr, "Unexpected GL: IsBuffer\n"); std::abort();
  }
  GLboolean IsEnabled (GLenum cap) override
  {
    std::fprintf(stderr, "Unexpected GL: IsEnabled\n"); std::abort();
  }
  GLboolean IsFramebuffer (GLuint framebuffer) override
  {
    std::fprintf(stderr, "Unexpected GL: IsFramebuffer\n"); std::abort();
  }
  GLboolean IsProgram (GLuint program) override
  {
    std::fprintf(stderr, "Unexpected GL: IsProgram\n"); std::abort();
  }
  GLboolean IsRenderbuffer (GLuint renderbuffer) override
  {
    std::fprintf(stderr, "Unexpected GL: IsRenderbuffer\n"); std::abort();
  }
  GLboolean IsShader (GLuint shader) override
  {
    std::fprintf(stderr, "Unexpected GL: IsShader\n"); std::abort();
  }
  GLboolean IsTexture (GLuint texture) override
  {
    std::fprintf(stderr, "Unexpected GL: IsTexture\n"); std::abort();
  }
  void LineWidth (GLfloat width) override
  {
    std::fprintf(stderr, "Unexpected GL: LineWidth\n"); std::abort();
  }
  void LinkProgram (GLuint program) override
  {
    std::fprintf(stderr, "Unexpected GL: LinkProgram\n"); std::abort();
  }
  void PixelStorei (GLenum pname, GLint param) override
  {
    std::fprintf(stderr, "Unexpected GL: PixelStorei\n"); std::abort();
  }
  void PolygonOffset (GLfloat factor, GLfloat units) override
  {
    std::fprintf(stderr, "Unexpected GL: PolygonOffset\n"); std::abort();
  }
  void ReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels) override
  {
    std::fprintf(stderr, "Unexpected GL: ReadPixels\n"); std::abort();
  }
  void ReleaseShaderCompiler (void) override
  {
    std::fprintf(stderr, "Unexpected GL: ReleaseShaderCompiler\n"); std::abort();
  }
  void RenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height) override
  {
    std::fprintf(stderr, "Unexpected GL: RenderbufferStorage\n"); std::abort();
  }
  void SampleCoverage (GLclampf value, GLboolean invert) override
  {
    std::fprintf(stderr, "Unexpected GL: SampleCoverage\n"); std::abort();
  }
  void Scissor (GLint x, GLint y, GLsizei width, GLsizei height) override
  {
    std::fprintf(stderr, "Unexpected GL: Scissor\n"); std::abort();
  }
  void ShaderBinary (GLsizei n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length) override
  {
    std::fprintf(stderr, "Unexpected GL: ShaderBinary\n"); std::abort();
  }
  void ShaderSource (GLuint shader, GLsizei count, const char** string, const GLint* length) override
  {
    std::fprintf(stderr, "Unexpected GL: ShaderSource\n"); std::abort();
  }
  void StencilFunc (GLenum func, GLint ref, GLuint mask) override
  {
    std::fprintf(stderr, "Unexpected GL: StencilFunc\n"); std::abort();
  }
  void StencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask) override
  {
    std::fprintf(stderr, "Unexpected GL: StencilFuncSeparate\n"); std::abort();
  }
  void StencilMask (GLuint mask) override
  {
    std::fprintf(stderr, "Unexpected GL: StencilMask\n"); std::abort();
  }
  void StencilMaskSeparate (GLenum face, GLuint mask) override
  {
    std::fprintf(stderr, "Unexpected GL: StencilMaskSeparate\n"); std::abort();
  }
  void StencilOp (GLenum fail, GLenum zfail, GLenum zpass) override
  {
    std::fprintf(stderr, "Unexpected GL: StencilOp\n"); std::abort();
  }
  void StencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass) override
  {
    std::fprintf(stderr, "Unexpected GL: StencilOpSeparate\n"); std::abort();
  }
  void TexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels) override
  {
    std::fprintf(stderr, "Unexpected GL: TexImage2D\n"); std::abort();
  }
  void TexParameterf (GLenum target, GLenum pname, GLfloat param) override
  {
    std::fprintf(stderr, "Unexpected GL: TexParameterf\n"); std::abort();
  }
  void TexParameterfv (GLenum target, GLenum pname, const GLfloat* params) override
  {
    std::fprintf(stderr, "Unexpected GL: TexParameterfv\n"); std::abort();
  }
  void TexParameteri (GLenum target, GLenum pname, GLint param) override
  {
    std::fprintf(stderr, "Unexpected GL: TexParameteri\n"); std::abort();
  }
  void TexParameteriv (GLenum target, GLenum pname, const GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: TexParameteriv\n"); std::abort();
  }
  void TexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) override
  {
    std::fprintf(stderr, "Unexpected GL: TexSubImage2D\n"); std::abort();
  }
  void Uniform1f (GLint location, GLfloat x) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform1f\n"); std::abort();
  }
  void Uniform1fv (GLint location, GLsizei count, const GLfloat* v) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform1fv\n"); std::abort();
  }
  void Uniform1i (GLint location, GLint x) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform1i\n"); std::abort();
  }
  void Uniform1iv (GLint location, GLsizei count, const GLint* v) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform1iv\n"); std::abort();
  }
  void Uniform2f (GLint location, GLfloat x, GLfloat y) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform2f\n"); std::abort();
  }
  void Uniform2fv (GLint location, GLsizei count, const GLfloat* v) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform2fv\n"); std::abort();
  }
  void Uniform2i (GLint location, GLint x, GLint y) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform2i\n"); std::abort();
  }
  void Uniform2iv (GLint location, GLsizei count, const GLint* v) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform2iv\n"); std::abort();
  }
  void Uniform3f (GLint location, GLfloat x, GLfloat y, GLfloat z) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform3f\n"); std::abort();
  }
  void Uniform3fv (GLint location, GLsizei count, const GLfloat* v) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform3fv\n"); std::abort();
  }
  void Uniform3i (GLint location, GLint x, GLint y, GLint z) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform3i\n"); std::abort();
  }
  void Uniform3iv (GLint location, GLsizei count, const GLint* v) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform3iv\n"); std::abort();
  }
  void Uniform4f (GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform4f\n"); std::abort();
  }
  void Uniform4fv (GLint location, GLsizei count, const GLfloat* v) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform4fv\n"); std::abort();
  }
  void Uniform4i (GLint location, GLint x, GLint y, GLint z, GLint w) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform4i\n"); std::abort();
  }
  void Uniform4iv (GLint location, GLsizei count, const GLint* v) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform4iv\n"); std::abort();
  }
  void UniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
    std::fprintf(stderr, "Unexpected GL: UniformMatrix2fv\n"); std::abort();
  }
  void UniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
    std::fprintf(stderr, "Unexpected GL: UniformMatrix3fv\n"); std::abort();
  }
  void UniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
    std::fprintf(stderr, "Unexpected GL: UniformMatrix4fv\n"); std::abort();
  }
  void UseProgram (GLuint program) override
  {
    std::fprintf(stderr, "Unexpected GL: UseProgram\n"); std::abort();
  }
  void ValidateProgram (GLuint program) override
  {
    std::fprintf(stderr, "Unexpected GL: ValidateProgram\n"); std::abort();
  }
  void VertexAttrib1f (GLuint indx, GLfloat x) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttrib1f\n"); std::abort();
  }
  void VertexAttrib1fv (GLuint indx, const GLfloat* values) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttrib1fv\n"); std::abort();
  }
  void VertexAttrib2f (GLuint indx, GLfloat x, GLfloat y) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttrib2f\n"); std::abort();
  }
  void VertexAttrib2fv (GLuint indx, const GLfloat* values) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttrib2fv\n"); std::abort();
  }
  void VertexAttrib3f (GLuint indx, GLfloat x, GLfloat y, GLfloat z) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttrib3f\n"); std::abort();
  }
  void VertexAttrib3fv (GLuint indx, const GLfloat* values) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttrib3fv\n"); std::abort();
  }
  void VertexAttrib4f (GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttrib4f\n"); std::abort();
  }
  void VertexAttrib4fv (GLuint indx, const GLfloat* values) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttrib4fv\n"); std::abort();
  }
  void VertexAttribPointer (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttribPointer\n"); std::abort();
  }
  void Viewport (GLint x, GLint y, GLsizei width, GLsizei height) override
  {
    std::fprintf(stderr, "Unexpected GL: Viewport\n"); std::abort();
  }
  void ReadBuffer (GLenum mode) override
  {
    std::fprintf(stderr, "Unexpected GL: ReadBuffer\n"); std::abort();
  }
  void DrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid* indices) override
  {
    std::fprintf(stderr, "Unexpected GL: DrawRangeElements\n"); std::abort();
  }
  void TexImage3D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels) override
  {
    std::fprintf(stderr, "Unexpected GL: TexImage3D\n"); std::abort();
  }
  void TexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid* pixels) override
  {
    std::fprintf(stderr, "Unexpected GL: TexSubImage3D\n"); std::abort();
  }
  void CopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) override
  {
    std::fprintf(stderr, "Unexpected GL: CopyTexSubImage3D\n"); std::abort();
  }
  void CompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid* data) override
  {
    std::fprintf(stderr, "Unexpected GL: CompressedTexImage3D\n"); std::abort();
  }
  void CompressedTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid* data) override
  {
    std::fprintf(stderr, "Unexpected GL: CompressedTexSubImage3D\n"); std::abort();
  }
  void GenQueries (GLsizei n, GLuint* ids) override
  {
    std::fprintf(stderr, "Unexpected GL: GenQueries\n"); std::abort();
  }
  void DeleteQueries (GLsizei n, const GLuint* ids) override
  {
    std::fprintf(stderr, "Unexpected GL: DeleteQueries\n"); std::abort();
  }
  GLboolean IsQuery (GLuint id) override
  {
    std::fprintf(stderr, "Unexpected GL: IsQuery\n"); std::abort();
  }
  void BeginQuery (GLenum target, GLuint id) override
  {
    std::fprintf(stderr, "Unexpected GL: BeginQuery\n"); std::abort();
  }
  void EndQuery (GLenum target) override
  {
    std::fprintf(stderr, "Unexpected GL: EndQuery\n"); std::abort();
  }
  void GetQueryiv (GLenum target, GLenum pname, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetQueryiv\n"); std::abort();
  }
  void GetQueryObjectuiv (GLuint id, GLenum pname, GLuint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetQueryObjectuiv\n"); std::abort();
  }
  GLboolean UnmapBuffer (GLenum target) override
  {
    std::fprintf(stderr, "Unexpected GL: UnmapBuffer\n"); std::abort();
  }
  void GetBufferPointerv (GLenum target, GLenum pname, GLvoid** params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetBufferPointerv\n"); std::abort();
  }
  void DrawBuffers (GLsizei n, const GLenum* bufs) override
  {
    std::fprintf(stderr, "Unexpected GL: DrawBuffers\n"); std::abort();
  }
  void UniformMatrix2x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
    std::fprintf(stderr, "Unexpected GL: UniformMatrix2x3fv\n"); std::abort();
  }
  void UniformMatrix3x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
    std::fprintf(stderr, "Unexpected GL: UniformMatrix3x2fv\n"); std::abort();
  }
  void UniformMatrix2x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
    std::fprintf(stderr, "Unexpected GL: UniformMatrix2x4fv\n"); std::abort();
  }
  void UniformMatrix4x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
    std::fprintf(stderr, "Unexpected GL: UniformMatrix4x2fv\n"); std::abort();
  }
  void UniformMatrix3x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
    std::fprintf(stderr, "Unexpected GL: UniformMatrix3x4fv\n"); std::abort();
  }
  void UniformMatrix4x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) override
  {
    std::fprintf(stderr, "Unexpected GL: UniformMatrix4x3fv\n"); std::abort();
  }
  void BlitFramebuffer (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) override
  {
    std::fprintf(stderr, "Unexpected GL: BlitFramebuffer\n"); std::abort();
  }
  void RenderbufferStorageMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) override
  {
    std::fprintf(stderr, "Unexpected GL: RenderbufferStorageMultisample\n"); std::abort();
  }
  void FramebufferTexture2DMultisample(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples) override
  {
    std::fprintf(stderr, "Unexpected GL: FramebufferTexture2DMultisample\n"); std::abort();
  }
  void FramebufferTextureLayer (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer) override
  {
    std::fprintf(stderr, "Unexpected GL: FramebufferTextureLayer\n"); std::abort();
  }
  GLvoid* MapBufferRange (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) override
  {
    std::fprintf(stderr, "Unexpected GL: MapBufferRange\n"); std::abort();
  }
  void FlushMappedBufferRange (GLenum target, GLintptr offset, GLsizeiptr length) override
  {
    std::fprintf(stderr, "Unexpected GL: FlushMappedBufferRange\n"); std::abort();
  }
  void BindVertexArray (GLuint array) override
  {
    std::fprintf(stderr, "Unexpected GL: BindVertexArray\n"); std::abort();
  }
  void DeleteVertexArrays (GLsizei n, const GLuint* arrays) override
  {
    std::fprintf(stderr, "Unexpected GL: DeleteVertexArrays\n"); std::abort();
  }
  void GenVertexArrays (GLsizei n, GLuint* arrays) override
  {
    std::fprintf(stderr, "Unexpected GL: GenVertexArrays\n"); std::abort();
  }
  GLboolean IsVertexArray (GLuint array) override
  {
    std::fprintf(stderr, "Unexpected GL: IsVertexArray\n"); std::abort();
  }
  void GetIntegeri_v (GLenum target, GLuint index, GLint* data) override
  {
    std::fprintf(stderr, "Unexpected GL: GetIntegeri_v\n"); std::abort();
  }
  void BeginTransformFeedback (GLenum primitiveMode) override
  {
    std::fprintf(stderr, "Unexpected GL: BeginTransformFeedback\n"); std::abort();
  }
  void EndTransformFeedback (void) override
  {
    std::fprintf(stderr, "Unexpected GL: EndTransformFeedback\n"); std::abort();
  }
  void BindBufferRange (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) override
  {
    std::fprintf(stderr, "Unexpected GL: BindBufferRange\n"); std::abort();
  }
  void BindBufferBase (GLenum target, GLuint index, GLuint buffer) override
  {
    std::fprintf(stderr, "Unexpected GL: BindBufferBase\n"); std::abort();
  }
  void TransformFeedbackVaryings (GLuint program, GLsizei count, const GLchar* const* varyings, GLenum bufferMode) override
  {
    std::fprintf(stderr, "Unexpected GL: TransformFeedbackVaryings\n"); std::abort();
  }
  void GetTransformFeedbackVarying (GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, GLchar* name) override
  {
    std::fprintf(stderr, "Unexpected GL: GetTransformFeedbackVarying\n"); std::abort();
  }
  void VertexAttribIPointer (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttribIPointer\n"); std::abort();
  }
  void GetVertexAttribIiv (GLuint index, GLenum pname, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetVertexAttribIiv\n"); std::abort();
  }
  void GetVertexAttribIuiv (GLuint index, GLenum pname, GLuint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetVertexAttribIuiv\n"); std::abort();
  }
  void VertexAttribI4i (GLuint index, GLint x, GLint y, GLint z, GLint w) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttribI4i\n"); std::abort();
  }
  void VertexAttribI4ui (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttribI4ui\n"); std::abort();
  }
  void VertexAttribI4iv (GLuint index, const GLint* v) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttribI4iv\n"); std::abort();
  }
  void VertexAttribI4uiv (GLuint index, const GLuint* v) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttribI4uiv\n"); std::abort();
  }
  void GetUniformuiv (GLuint program, GLint location, GLuint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetUniformuiv\n"); std::abort();
  }
  GLint GetFragDataLocation (GLuint program, const GLchar *name) override
  {
    std::fprintf(stderr, "Unexpected GL: GetFragDataLocation\n"); std::abort();
  }
  void Uniform1ui (GLint location, GLuint v0) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform1ui\n"); std::abort();
  }
  void Uniform2ui (GLint location, GLuint v0, GLuint v1) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform2ui\n"); std::abort();
  }
  void Uniform3ui (GLint location, GLuint v0, GLuint v1, GLuint v2) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform3ui\n"); std::abort();
  }
  void Uniform4ui (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform4ui\n"); std::abort();
  }
  void Uniform1uiv (GLint location, GLsizei count, const GLuint* value) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform1uiv\n"); std::abort();
  }
  void Uniform2uiv (GLint location, GLsizei count, const GLuint* value) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform2uiv\n"); std::abort();
  }
  void Uniform3uiv (GLint location, GLsizei count, const GLuint* value) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform3uiv\n"); std::abort();
  }
  void Uniform4uiv (GLint location, GLsizei count, const GLuint* value) override
  {
    std::fprintf(stderr, "Unexpected GL: Uniform4uiv\n"); std::abort();
  }
  void ClearBufferiv (GLenum buffer, GLint drawbuffer, const GLint* value) override
  {
    std::fprintf(stderr, "Unexpected GL: ClearBufferiv\n"); std::abort();
  }
  void ClearBufferuiv (GLenum buffer, GLint drawbuffer, const GLuint* value) override
  {
    std::fprintf(stderr, "Unexpected GL: ClearBufferuiv\n"); std::abort();
  }
  void ClearBufferfv (GLenum buffer, GLint drawbuffer, const GLfloat* value) override
  {
    std::fprintf(stderr, "Unexpected GL: ClearBufferfv\n"); std::abort();
  }
  void ClearBufferfi (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil) override
  {
    std::fprintf(stderr, "Unexpected GL: ClearBufferfi\n"); std::abort();
  }
  const GLubyte* GetStringi (GLenum name, GLuint index) override
  {
    std::fprintf(stderr, "Unexpected GL: GetStringi\n"); std::abort();
  }
  void CopyBufferSubData (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size) override
  {
    std::fprintf(stderr, "Unexpected GL: CopyBufferSubData\n"); std::abort();
  }
  void GetUniformIndices (GLuint program, GLsizei uniformCount, const GLchar* const* uniformNames, GLuint* uniformIndices) override
  {
    std::fprintf(stderr, "Unexpected GL: GetUniformIndices\n"); std::abort();
  }
  void GetActiveUniformsiv (GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetActiveUniformsiv\n"); std::abort();
  }
  GLuint GetUniformBlockIndex (GLuint program, const GLchar* uniformBlockName) override
  {
    std::fprintf(stderr, "Unexpected GL: GetUniformBlockIndex\n"); std::abort();
  }
  void GetActiveUniformBlockiv (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetActiveUniformBlockiv\n"); std::abort();
  }
  void GetActiveUniformBlockName (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName) override
  {
    std::fprintf(stderr, "Unexpected GL: GetActiveUniformBlockName\n"); std::abort();
  }
  void UniformBlockBinding (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding) override
  {
    std::fprintf(stderr, "Unexpected GL: UniformBlockBinding\n"); std::abort();
  }
  void DrawArraysInstanced (GLenum mode, GLint first, GLsizei count, GLsizei instanceCount) override
  {
    std::fprintf(stderr, "Unexpected GL: DrawArraysInstanced\n"); std::abort();
  }
  void DrawElementsInstanced (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLsizei instanceCount) override
  {
    std::fprintf(stderr, "Unexpected GL: DrawElementsInstanced\n"); std::abort();
  }
  GLsync FenceSync (GLenum condition, GLbitfield flags) override
  {
    std::fprintf(stderr, "Unexpected GL: FenceSync\n"); std::abort();
  }
  GLboolean IsSync (GLsync sync) override
  {
    std::fprintf(stderr, "Unexpected GL: IsSync\n"); std::abort();
  }
  void DeleteSync (GLsync sync) override
  {
    std::fprintf(stderr, "Unexpected GL: DeleteSync\n"); std::abort();
  }
  GLenum ClientWaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout) override
  {
    std::fprintf(stderr, "Unexpected GL: ClientWaitSync\n"); std::abort();
  }
  void WaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout) override
  {
    std::fprintf(stderr, "Unexpected GL: WaitSync\n"); std::abort();
  }
  void GetInteger64v (GLenum pname, GLint64* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetInteger64v\n"); std::abort();
  }
  void GetSynciv (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values) override
  {
    std::fprintf(stderr, "Unexpected GL: GetSynciv\n"); std::abort();
  }
  void GetInteger64i_v (GLenum target, GLuint index, GLint64* data) override
  {
    std::fprintf(stderr, "Unexpected GL: GetInteger64i_v\n"); std::abort();
  }
  void GetBufferParameteri64v (GLenum target, GLenum pname, GLint64* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetBufferParameteri64v\n"); std::abort();
  }
  void GenSamplers (GLsizei count, GLuint* samplers) override
  {
    std::fprintf(stderr, "Unexpected GL: GenSamplers\n"); std::abort();
  }
  void DeleteSamplers (GLsizei count, const GLuint* samplers) override
  {
    std::fprintf(stderr, "Unexpected GL: DeleteSamplers\n"); std::abort();
  }
  GLboolean IsSampler (GLuint sampler) override
  {
    std::fprintf(stderr, "Unexpected GL: IsSampler\n"); std::abort();
  }
  void BindSampler (GLuint unit, GLuint sampler) override
  {
    std::fprintf(stderr, "Unexpected GL: BindSampler\n"); std::abort();
  }
  void SamplerParameteri (GLuint sampler, GLenum pname, GLint param) override
  {
    std::fprintf(stderr, "Unexpected GL: SamplerParameteri\n"); std::abort();
  }
  void SamplerParameteriv (GLuint sampler, GLenum pname, const GLint* param) override
  {
    std::fprintf(stderr, "Unexpected GL: SamplerParameteriv\n"); std::abort();
  }
  void SamplerParameterf (GLuint sampler, GLenum pname, GLfloat param) override
  {
    std::fprintf(stderr, "Unexpected GL: SamplerParameterf\n"); std::abort();
  }
  void SamplerParameterfv (GLuint sampler, GLenum pname, const GLfloat* param) override
  {
    std::fprintf(stderr, "Unexpected GL: SamplerParameterfv\n"); std::abort();
  }
  void GetSamplerParameteriv (GLuint sampler, GLenum pname, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetSamplerParameteriv\n"); std::abort();
  }
  void GetSamplerParameterfv (GLuint sampler, GLenum pname, GLfloat* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetSamplerParameterfv\n"); std::abort();
  }
  void VertexAttribDivisor (GLuint index, GLuint divisor) override
  {
    std::fprintf(stderr, "Unexpected GL: VertexAttribDivisor\n"); std::abort();
  }
  void BindTransformFeedback (GLenum target, GLuint id) override
  {
    std::fprintf(stderr, "Unexpected GL: BindTransformFeedback\n"); std::abort();
  }
  void DeleteTransformFeedbacks (GLsizei n, const GLuint* ids) override
  {
    std::fprintf(stderr, "Unexpected GL: DeleteTransformFeedbacks\n"); std::abort();
  }
  void GenTransformFeedbacks (GLsizei n, GLuint* ids) override
  {
    std::fprintf(stderr, "Unexpected GL: GenTransformFeedbacks\n"); std::abort();
  }
  GLboolean IsTransformFeedback (GLuint id) override
  {
    std::fprintf(stderr, "Unexpected GL: IsTransformFeedback\n"); std::abort();
  }
  void PauseTransformFeedback (void) override
  {
    std::fprintf(stderr, "Unexpected GL: PauseTransformFeedback\n"); std::abort();
  }
  void ResumeTransformFeedback (void) override
  {
    std::fprintf(stderr, "Unexpected GL: ResumeTransformFeedback\n"); std::abort();
  }
  void GetProgramBinary (GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, GLvoid* binary) override
  {
    std::fprintf(stderr, "Unexpected GL: GetProgramBinary\n"); std::abort();
  }
  void ProgramBinary (GLuint program, GLenum binaryFormat, const GLvoid* binary, GLsizei length) override
  {
    std::fprintf(stderr, "Unexpected GL: ProgramBinary\n"); std::abort();
  }
  void ProgramParameteri (GLuint program, GLenum pname, GLint value) override
  {
    std::fprintf(stderr, "Unexpected GL: ProgramParameteri\n"); std::abort();
  }
  void InvalidateFramebuffer (GLenum target, GLsizei numAttachments, const GLenum* attachments) override
  {
    std::fprintf(stderr, "Unexpected GL: InvalidateFramebuffer\n"); std::abort();
  }
  void InvalidateSubFramebuffer (GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height) override
  {
    std::fprintf(stderr, "Unexpected GL: InvalidateSubFramebuffer\n"); std::abort();
  }
  void TexStorage2D (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) override
  {
    std::fprintf(stderr, "Unexpected GL: TexStorage2D\n"); std::abort();
  }
  void TexStorage3D (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) override
  {
    std::fprintf(stderr, "Unexpected GL: TexStorage3D\n"); std::abort();
  }
  void GetInternalformativ (GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params) override
  {
    std::fprintf(stderr, "Unexpected GL: GetInternalformativ\n"); std::abort();
  }
  void BlendBarrier (void) override
  {
    std::fprintf(stderr, "Unexpected GL: BlendBarrier\n"); std::abort();
  }
};
}
