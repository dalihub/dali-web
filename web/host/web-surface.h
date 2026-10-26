#pragma once
#include <dali/integration-api/adaptor-framework/render-surface-interface.h>
#include <dali/internal/graphics/gles-impl/gles-graphics-controller.h>
#include <dali/internal/graphics/web/web-gles-failure.h>
#include <emscripten/html5.h>
#include <pthread.h>

namespace Dali::Web
{
class Surface : public Integration::RenderSurfaceInterface
{
public:
  explicit Surface(Graphics::GlesGraphicsController& controller) : mController(controller) {}
  bool Open()
  {
    mOwner = pthread_self();
    EmscriptenWebGLContextAttributes attributes;
    emscripten_webgl_init_context_attributes(&attributes);
    attributes.majorVersion = 2;
    attributes.minorVersion = 0;
    attributes.alpha = false;
    attributes.depth = true;
    attributes.stencil = true;
    attributes.antialias = false;
    attributes.enableExtensionsByDefault = false;
    attributes.proxyContextToMainThread = EMSCRIPTEN_WEBGL_CONTEXT_PROXY_DISALLOW;
    attributes.renderViaOffscreenBackBuffer = false;
    attributes.explicitSwapControl = false;
    mContext = emscripten_webgl_create_context("#canvas", &attributes);
    if(mContext <= 0 || emscripten_webgl_make_context_current(mContext) != EMSCRIPTEN_RESULT_SUCCESS)
    {
      Graphics::GLES::WebFailure::Latch("Worker WebGL2 context creation failed");
      return false;
    }
    return Graphics::GLES::WebFailure::CheckGl();
  }
  void Close()
  {
    CheckOwner();
    if(mContext > 0)
    {
      emscripten_webgl_make_context_current(0);
      emscripten_webgl_destroy_context(mContext);
      mContext = 0;
    }
  }
  PositionSize GetPositionSize() const override
  {
    return {0, 0, mWidth, mHeight};
  }
  void GetDpi(unsigned int& horizontal, unsigned int& vertical) override
  {
    horizontal = vertical = 96;
  }
  int GetSurfaceOrientation() const override
  {
    return 0;
  }
  int GetScreenOrientation() const override
  {
    return 0;
  }
  void InitializeGraphics() override
  {
    MakeContextCurrent();
  }
  void CreateSurface() override
  {
    Graphics::GLES::WebFailure::Latch("surface already owned by Web host");
  }
  void DestroySurface() override
  {
    Close();
  }
  bool ReplaceGraphicsSurface() override
  {
    Graphics::GLES::WebFailure::Latch("context replacement requires a new execution instance");
    return true;
  }
  void MoveResize(PositionSize size) override
  {
    Resize(SurfaceSize(size.width, size.height));
  }
  void Resize(SurfaceSize size) override
  {
    CheckOwner();
    if(size.GetWidth() <= 0 || size.GetHeight() <= 0 || emscripten_set_canvas_element_size("#canvas", size.GetWidth(), size.GetHeight()) != EMSCRIPTEN_RESULT_SUCCESS)
    {
      Graphics::GLES::WebFailure::Latch("canvas resize failed");
      return;
    }
    mWidth = size.GetWidth(); mHeight = size.GetHeight();
  }
  void StartRender() override
  {
    MakeContextCurrent();
  }
  bool PreRender(bool, const std::vector<BoundsInteger>&, BoundsInteger& clip) override
  {
    MakeContextCurrent(); clip = GetPositionSize();
    return Graphics::GLES::WebFailure::CheckGl();
  }
  void PostRender() override
  {
    CheckOwner();
    // Implicit OffscreenCanvas presentation occurs when the Worker callback
    // returns. This check does not claim GPU/display completion.
    Graphics::GLES::WebFailure::CheckGl();
  }
  void StopRender() override
  {
    Graphics::GLES::WebFailure::Stop();
  }
  void ReleaseLock() override
  {
    Graphics::GLES::WebFailure::Latch("native compositor lock unsupported");
  }
  void SetThreadSynchronization(ThreadSynchronizationInterface&) override
  {
    Graphics::GLES::WebFailure::Latch("native thread synchronization unsupported");
  }
  Type GetSurfaceType() override
  {
    return WINDOW_RENDER_SURFACE;
  }
  void MakeContextCurrent() override
  {
    CheckOwner();
    if(mContext <= 0 || emscripten_webgl_make_context_current(mContext) != EMSCRIPTEN_RESULT_SUCCESS)
    {
      Graphics::GLES::WebFailure::Latch("WebGL context activation failed");
    }
  }
private:
  void CheckOwner() const
  {
    DALI_ASSERT_ALWAYS(pthread_equal(mOwner, pthread_self()));
  }
  Graphics::GlesGraphicsController& mController;
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE mContext{0};
  pthread_t mOwner{};
  int mWidth{320};
  int mHeight{240};
};
}
