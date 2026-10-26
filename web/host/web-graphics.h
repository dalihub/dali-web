#pragma once
#include "web-surface.h"
#include <dali/internal/graphics/common/graphics-interface.h>
#include <dali/internal/graphics/web/web-gl-implementation.h>
namespace Dali::Web
{
class GraphicsHost : public Graphics::GraphicsInterface
{
public:
  GraphicsHost(Graphics::GlesGraphicsController& controller, Graphics::GLES::WebGlImplementation& gl, Surface& surface)
  : GraphicsInterface({}, Integration::DepthBufferAvailable::TRUE, Integration::StencilBufferAvailable::TRUE, Integration::PartialUpdateAvailable::FALSE, 0, Graphics::ContextPriority::DEFAULT), mController(controller), mGl(gl), mSurface(surface) {}
  void SetInitialized(bool value)
  {
    mInitialized = value;
  }
  Dali::Graphics::Controller& GetController() override
  {
    return mController;
  }
  void Initialize(const Dali::DisplayConnection& displayConnection) override
  {
    Unsupported();
  }
  void Initialize(const Dali::DisplayConnection& displayConnection, bool depth, bool stencil, bool partialRendering, int msaa, Dali::Graphics::ContextPriority contextPriority) override
  {
    Unsupported();
  }
  void InitializeGraphicsAPI(const Dali::DisplayConnection& displayConnection) override
  {
    Unsupported();
  }
  Dali::Any GetDisplay() const override
  {
    Unsupported(); return {};
  }
  void ConfigureSurface(Dali::Integration::RenderSurfaceInterface* surface) override
  {
    Unsupported();
  }
  Graphics::SurfaceId CreateSurface( Graphics::SurfaceFactory* factory, Dali::Internal::Adaptor::WindowBase* windowBase, ColorDepth colorDepth, int width, int height) override
  {
    Unsupported(); return {};
  }
  Graphics::SurfaceId CreateSurface( Graphics::SurfaceFactory* factory, Dali::Internal::Adaptor::WindowBase* windowBase, ColorDepth colorDepth, int width, int height, bool depthBufferRequired, bool stencilBufferRequired, int multiSamplingLevel) override
  {
    Unsupported(); return {};
  }
  bool ReconfigureSurface(Graphics::SurfaceId surfaceId, bool depthBufferRequired, bool stencilBufferRequired, int multiSamplingLevel) override
  {
    Unsupported(); return {};
  }
  void ResetSurfaceState() override
  {
    Unsupported();
  }
  void DestroySurface(Graphics::SurfaceId surfaceId) override
  {
    Unsupported();
  }
  bool ReplaceSurface(Graphics::SurfaceId surfaceId, int width, int height) override
  {
    Unsupported(); return {};
  }
  void ActivateResourceContext() override
  {
    mSurface.MakeContextCurrent(); mController.ActivateResourceContext();
  }
  void ActivateSurfaceContext(Integration::RenderSurfaceInterface* surface) override
  {
    if(surface != &mSurface)
    {
      Unsupported();
      return;
    }
    mSurface.MakeContextCurrent(); mController.ActivateSurfaceContext(surface);
  }
  void MakeContextCurrent(Graphics::SurfaceId surfaceId) override
  {
    if(surfaceId != 1)
    {
      Unsupported();
      return;
    }
    mSurface.MakeContextCurrent();
  }
  void AcquireNextImage(Integration::RenderSurfaceInterface* surface) override
  {
    Unsupported();
  }
  void PostRender() override
  {
    mController.PostRender(); mGl.PostRender();
  }
  void Shutdown() override
  {
    mController.Shutdown(); mInitialized = false;
  }
  void Destroy() override
  {
    Unsupported();
  }
  void Pause() override
  {
    Unsupported();
  }
  void Resume() override
  {
    Unsupported();
  }
  void Resize(Integration::RenderSurfaceInterface* surface, SurfaceSize size) override
  {
    if(surface != &mSurface)
    {
      Unsupported();
      return;
    }
    mSurface.Resize(size);
  }
  int GetBufferAge(Graphics::SurfaceId surfaceId) override
  {
    return 0;
  }
  void SetDamageRegion(Graphics::SurfaceId surfaceId, std::vector<BoundsInteger>& damagedRegion) override
  {
    Unsupported();
  }
  void SwapBuffers(Graphics::SurfaceId surfaceId) override
  {
    if(surfaceId != 1)
    {
      Unsupported();
      return;
    }
    mSurface.PostRender();
  }
  void SwapBuffers(Graphics::SurfaceId surfaceId, const std::vector<BoundsInteger>& damageRects) override
  {
    if(surfaceId != 1)
    {
      Unsupported();
      return;
    }
    mSurface.PostRender();
  }
  bool IsAdvancedBlendEquationSupported() override
  {
    return false;
  }
  bool IsMultisampledRenderToTextureSupported() override
  {
    return false;
  }
  bool IsInitialized() override
  {
    return mInitialized;
  }
  bool IsResourceContextSupported() override
  {
    return false;
  }
  uint32_t GetMaxTextureSize() override
  {
    return mGl.GetMaxTextureSize();
  }
  uint32_t GetMaxCombinedTextureUnits() override
  {
    return mGl.GetMaxCombinedTextureUnits();
  }
  uint8_t GetMaxTextureSamples() override
  {
    return 0;
  }
  uint32_t GetShaderLanguageVersion() override
  {
    return mGl.GetShaderLanguageVersion();
  }
  void CacheConfigurations(Dali::Internal::Adaptor::ConfigurationManager& configurationManager) override
  {
    Unsupported();
  }
  void FrameStart() override
  {
    mController.FrameStart();
  }
  void RenderStart() override
  {
    mGl.PreRender();
  }
  bool ForcePresentRequired() override
  {
    return false;
  }
  bool DidPresent() override
  {
    return mController.DidPresent();
  }
  void PostRenderDebug() override
  {
    Unsupported();
  }
  void LogMemoryPools() override
  {
    Unsupported();
  }
private:
  static void Unsupported()
  {
    Graphics::GLES::WebFailure::Latch("native graphics host operation unsupported");
  }
  Graphics::GlesGraphicsController& mController;
  Graphics::GLES::WebGlImplementation& mGl;
  Surface& mSurface;
  bool mInitialized{false};
};
}
