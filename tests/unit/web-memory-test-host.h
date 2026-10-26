// Test-only host boundary. Unexpected platform calls fail immediately.
#pragma once
#include <dali/internal/graphics/common/graphics-interface.h>
#include <cstdlib>
namespace Dali::Graphics
{
class MemoryTestHost : public GraphicsInterface
{
public:
  explicit MemoryTestHost(GlesGraphicsController& controller)
  : GraphicsInterface({}, {}, {}, {}, 0, ContextPriority::DEFAULT), mController(controller) {}
  Dali::Graphics::Controller& GetController() override
  {
    return mController;
  }
  void Initialize(const Dali::DisplayConnection& displayConnection) override
  {
    std::abort();
  }
  void Initialize(const Dali::DisplayConnection& displayConnection, bool depth, bool stencil, bool partialRendering, int msaa, Dali::Graphics::ContextPriority contextPriority) override
  {
    std::abort();
  }
  void InitializeGraphicsAPI(const Dali::DisplayConnection& displayConnection) override
  {
    std::abort();
  }
  Dali::Any GetDisplay() const override
  {
    std::abort();
  }
  void ConfigureSurface(Dali::Integration::RenderSurfaceInterface* surface) override
  {
    std::abort();
  }
  Graphics::SurfaceId CreateSurface( Graphics::SurfaceFactory* factory, Dali::Internal::Adaptor::WindowBase* windowBase, ColorDepth colorDepth, int width, int height) override
  {
    std::abort();
  }
  Graphics::SurfaceId CreateSurface( Graphics::SurfaceFactory* factory, Dali::Internal::Adaptor::WindowBase* windowBase, ColorDepth colorDepth, int width, int height, bool depthBufferRequired, bool stencilBufferRequired, int multiSamplingLevel) override
  {
    std::abort();
  }
  bool ReconfigureSurface(Graphics::SurfaceId surfaceId, bool depthBufferRequired, bool stencilBufferRequired, int multiSamplingLevel) override
  {
    std::abort();
  }
  void ResetSurfaceState() override
  {
    std::abort();
  }
  void DestroySurface(Graphics::SurfaceId surfaceId) override
  {
    std::abort();
  }
  bool ReplaceSurface(Graphics::SurfaceId surfaceId, int width, int height) override
  {
    std::abort();
  }
  void ActivateResourceContext() override
  {
    mController.ActivateResourceContext();
  }
  void ActivateSurfaceContext(Integration::RenderSurfaceInterface* surface) override
  {
    std::abort();
  }
  void MakeContextCurrent(Graphics::SurfaceId surfaceId) override
  {
    std::abort();
  }
  void AcquireNextImage(Integration::RenderSurfaceInterface* surface) override
  {
    std::abort();
  }
  void PostRender() override
  {
    std::abort();
  }
  void Shutdown() override
  {
    std::abort();
  }
  void Destroy() override
  {
    std::abort();
  }
  void Pause() override
  {
    std::abort();
  }
  void Resume() override
  {
    std::abort();
  }
  void Resize(Integration::RenderSurfaceInterface* surface, SurfaceSize size) override
  {
    std::abort();
  }
  int GetBufferAge(Graphics::SurfaceId surfaceId) override
  {
    std::abort();
  }
  void SetDamageRegion(Graphics::SurfaceId surfaceId, std::vector<BoundsInteger>& damagedRegion) override
  {
    std::abort();
  }
  void SwapBuffers(Graphics::SurfaceId surfaceId) override
  {
    std::abort();
  }
  void SwapBuffers(Graphics::SurfaceId surfaceId, const std::vector<BoundsInteger>& damageRects) override
  {
    std::abort();
  }
  bool IsAdvancedBlendEquationSupported() override
  {
    std::abort();
  }
  bool IsMultisampledRenderToTextureSupported() override
  {
    std::abort();
  }
  bool IsInitialized() override
  {
    std::abort();
  }
  bool IsResourceContextSupported() override
  {
    std::abort();
  }
  uint32_t GetMaxTextureSize() override
  {
    std::abort();
  }
  uint32_t GetMaxCombinedTextureUnits() override
  {
    std::abort();
  }
  uint8_t GetMaxTextureSamples() override
  {
    std::abort();
  }
  uint32_t GetShaderLanguageVersion() override
  {
    std::abort();
  }
  void CacheConfigurations(Dali::Internal::Adaptor::ConfigurationManager& configurationManager) override
  {
    std::abort();
  }
  void FrameStart() override
  {
    std::abort();
  }
  void RenderStart() override
  {
    std::abort();
  }
  bool ForcePresentRequired() override
  {
    std::abort();
  }
  bool DidPresent() override
  {
    std::abort();
  }
  void PostRenderDebug() override
  {
    std::abort();
  }
  void LogMemoryPools() override
  {
    std::abort();
  }
private:
  GlesGraphicsController& mController;
};
}
