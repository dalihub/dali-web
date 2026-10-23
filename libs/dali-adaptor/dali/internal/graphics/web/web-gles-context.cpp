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

#include <dali/internal/graphics/gles-impl/gles-graphics-controller.h>
#include <emscripten/html5_webgl.h>

namespace DALI_NAMESPACE::Graphics
{
void GlesGraphicsController::CreateSurfaceContext(Integration::RenderSurfaceInterface* surface)
{
  DALI_ASSERT_ALWAYS(surface && !mSingleSurface && "Web GLES supports one surface");
  // Registration only: the resource context owns the single WebGL state cache.
  mSingleSurface = surface;
}

void GlesGraphicsController::DeleteSurfaceContext(Integration::RenderSurfaceInterface* surface)
{
  DALI_ASSERT_ALWAYS(GetSurfaceContext(surface) && "Unknown Web GLES surface");
  mSyncPool.InvalidateContext(mContext.get());
  mTextureDependencyChecker.InvalidateContext(mContext.get());
  mSingleSurface = nullptr;
}

void GlesGraphicsController::ActivateResourceContext()
{
  const auto handle = emscripten_webgl_get_current_context();
  DALI_ASSERT_ALWAYS(handle > 0 && "WebGL context must be current on the render owner");
  DALI_ASSERT_ALWAYS((mWebContextHandle == 0 || mWebContextHandle == handle) && "WebGL context replacement requires a new controller");
  mWebContextHandle = handle;
  mCurrentContext = mContext.get();
  mCurrentContext->GlContextCreated();
}

void GlesGraphicsController::ActivateSurfaceContext(Integration::RenderSurfaceInterface* surface)
{
  DALI_ASSERT_ALWAYS(GetSurfaceContext(surface) && "Unknown Web GLES surface");
  ActivateResourceContext();
}

GLES::Context* GlesGraphicsController::GetSurfaceContext(Integration::RenderSurfaceInterface* surface) const
{
  return surface && surface == mSingleSurface ? mContext.get() : nullptr;
}
} // namespace DALI_NAMESPACE::Graphics
