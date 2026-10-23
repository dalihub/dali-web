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
 *
 */

#include <dali/internal/graphics/gles-impl/gles-native-rendering.h>
#include <dali/internal/graphics/gles-impl/gles-graphics-controller.h>
#include <dali/internal/graphics/common/egl-include.h>
#include <dali/integration-api/debug.h>

namespace DALI_NAMESPACE::Graphics::GLES
{
namespace
{
DALI_INIT_TIME_CHECKER_FILTER(gTimeCheckerFilter, DALI_EGL_PERFORMANCE_LOG_THRESHOLD_TIME);

class NativeRenderingContext : public NativeRendering
{
public:
  explicit NativeRenderingContext(GlesGraphicsController& controller)
  : mController(controller)
  {
  }

  ~NativeRenderingContext() override
  {
    if(mNativeDrawContext)
    {
      eglDestroyContext(eglGetCurrentDisplay(), mNativeDrawContext);
    }
  }

  void Prepare() override;
  void Restore() override;

private:
  GlesGraphicsController& mController;
  EGLContext mNativeDrawContext{0u};
  EGLSurface mCacheDrawReadSurface{0u};
  EGLSurface mCacheDrawWriteSurface{0u};
  EGLContext mCacheEGLGraphicsContext{0u};
};

void NativeRenderingContext::Prepare()
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);

  // this should be pretty much constant
  auto display     = eglGetCurrentDisplay();
  auto drawSurface = eglGetCurrentSurface(EGL_DRAW);
  auto readSurface = eglGetCurrentSurface(EGL_READ);
  auto context     = eglGetCurrentContext();

  // push the surface and context data to the impl
  // It's needed to restore context
  if(!mCacheEGLGraphicsContext)
  {
    mCacheDrawWriteSurface   = drawSurface;
    mCacheDrawReadSurface    = readSurface;
    mCacheEGLGraphicsContext = context;
  }

  if(!mNativeDrawContext)
  {
    EGLint configId{0u};

    {
      DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "eglQueryContext(Native)");
      eglQueryContext(display, mController.GetSharedContext(), EGL_CONFIG_ID, &configId);
    }

    EGLint configAttribs[3];
    configAttribs[0] = EGL_CONFIG_ID;
    configAttribs[1] = configId;
    configAttribs[2] = EGL_NONE;

    EGLConfig config;
    EGLint    numConfigs;
    {
      DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "eglChooseConfig(Native)");
      if(eglChooseConfig(display, configAttribs, &config, 1, &numConfigs) != EGL_TRUE)
      {
        DALI_LOG_ERROR("eglChooseConfig failed!\n");
        return;
      }
    }

    auto version = int(mController.GetGLESVersion());

    std::vector<EGLint> attribs;
    attribs.push_back(EGL_CONTEXT_MAJOR_VERSION_KHR);
    attribs.push_back(version / 10);
    attribs.push_back(EGL_CONTEXT_MINOR_VERSION_KHR);
    attribs.push_back(version % 10);
    attribs.push_back(EGL_NONE);

    {
      DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "eglCreateContext(Native)");
      mNativeDrawContext = eglCreateContext(display, config, mController.GetSharedContext(), attribs.data());
      if(mNativeDrawContext == EGL_NO_CONTEXT)
      {
        DALI_LOG_ERROR("eglCreateContext failed!\n");
        return;
      }
    }
  }

  eglMakeCurrent(display, drawSurface, readSurface, mNativeDrawContext);

  DALI_TIME_CHECKER_END_WITH_MESSAGE(gTimeCheckerFilter, "PrepareForNativeRendering");
}

void NativeRenderingContext::Restore()
{
  DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "RestoreFromNativeRendering");
  auto display = eglGetCurrentDisplay();

  // bring back original context
  eglMakeCurrent(display, mCacheDrawWriteSurface, mCacheDrawReadSurface, mCacheEGLGraphicsContext);
}

} // namespace

std::unique_ptr<NativeRendering> CreateNativeRendering(GlesGraphicsController& controller)
{
  return std::make_unique<NativeRenderingContext>(controller);
}
} // namespace DALI_NAMESPACE::Graphics::GLES
