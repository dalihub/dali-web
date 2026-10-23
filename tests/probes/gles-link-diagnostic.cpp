// Link-only diagnostic. The caller must supply real host services and a current
// context. This is deliberately not a browser bootstrap or executable test.
#include <emscripten/emscripten.h>
#include <dali/integration-api/core.h>
#include <dali/internal/graphics/gles-impl/egl-graphics-controller.h>
#include <dali/internal/graphics/gles/gl-implementation.h>

extern "C" EMSCRIPTEN_KEEPALIVE Dali::Integration::Core* createDiagnosticCore(
  Dali::Integration::RenderController* renderController,
  Dali::Integration::PlatformAbstraction* platform,
  Dali::Internal::Adaptor::GlImplementation* gl,
  Dali::Graphics::EglGraphicsController** graphics)
{
  *graphics = new Dali::Graphics::EglGraphicsController;
  (*graphics)->InitializeGLES(*gl);
  return Dali::Integration::Core::New(*renderController, *platform, **graphics,
    static_cast<Dali::Integration::CorePolicyFlags>(0));
}

extern "C" EMSCRIPTEN_KEEPALIVE Dali::Internal::Adaptor::GlImplementation* createDiagnosticGl()
{
  return new Dali::Internal::Adaptor::GlImplementation;
}

// Separate link roots, not a valid frame loop. No function here is executed.
extern "C" EMSCRIPTEN_KEEPALIVE void initializeDiagnosticGraphics(
  Dali::Graphics::EglGraphicsController* graphics,
  Dali::Integration::GraphicsSyncAbstraction* sync,
  Dali::Graphics::GraphicsInterface* interface)
{
  graphics->Initialize(*sync, *interface);
  graphics->ActivateResourceContext();
}

extern "C" EMSCRIPTEN_KEEPALIVE void retainDiagnosticFrame(
  Dali::Integration::Core* core,
  Dali::Integration::Scene* scene,
  Dali::Integration::UpdateStatus* updateStatus,
  Dali::Integration::RenderStatus* renderStatus)
{
  core->Update(0.0f, 0u, 0u, *updateStatus, false, false, false);
  core->PreRender(*renderStatus, false);
  core->RenderScene(*renderStatus, *scene, false);
  core->PostRender();
}

extern "C" EMSCRIPTEN_KEEPALIVE void retainDiagnosticNativeDependency(
  Dali::Graphics::GLES::SyncPool* pool)
{
  auto sync = pool->AllocateSharedSyncObject(true);
  sync->Poll();
}
