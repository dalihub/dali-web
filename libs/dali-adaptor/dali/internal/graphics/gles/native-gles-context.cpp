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
#include <dali/internal/graphics/gles/egl-graphics.h>
#include <algorithm>

namespace DALI_NAMESPACE::Graphics
{
void GlesGraphicsController::CreateSurfaceContext(Dali::Integration::RenderSurfaceInterface* surface)
{
  std::unique_ptr<GLES::Context> context = std::make_unique<GLES::Context>(*this, mGlAbstraction);
  mSurfaceContexts.push_back(std::make_pair(surface, std::move(context)));
}

void GlesGraphicsController::DeleteSurfaceContext(Dali::Integration::RenderSurfaceInterface* surface)
{
  auto       iter       = mSurfaceContexts.begin();
  auto       newEndIter = mSurfaceContexts.begin();
  const auto endIter    = mSurfaceContexts.end();
  for(; iter != endIter; ++iter)
  {
    if(iter->first != surface)
    {
      if(newEndIter != iter)
      {
        *newEndIter = std::move(*iter);
      }
      newEndIter++;
      continue;
    }
    else
    {
      const auto* invalidatedContext = iter->second.get();

      // Mark as given context will be deleted soon.
      // It will make sync object id that created by given context
      // become invalidated.
      mSyncPool.InvalidateContext(invalidatedContext);

      // Remove cached info about given context from texture dependency checker.
      // It will guard several cache-miss issues if new context is created with same pointer as given context.
      mTextureDependencyChecker.InvalidateContext(invalidatedContext);
    }
  }

  mSurfaceContexts.erase(newEndIter, endIter);
}

void GlesGraphicsController::ActivateResourceContext()
{
  mCurrentContext = mContext.get();
  mCurrentContext->GlContextCreated();
  if(!mSharedContext)
  {
    auto eglGraphics = dynamic_cast<Dali::Internal::Adaptor::EglGraphics*>(mGraphics);
    if(eglGraphics)
    {
      mSharedContext = eglGraphics->GetEglImplementation().GetContext();
    }
  }
}

void GlesGraphicsController::ActivateSurfaceContext(Dali::Integration::RenderSurfaceInterface* surface)
{
  if(surface && mGraphics->IsResourceContextSupported())
  {
    auto* context = GetSurfaceContext(surface);

    if(context)
    {
      mCurrentContext = context;
      mCurrentContext->GlContextCreated();
    }
  }
}

GLES::Context* GlesGraphicsController::GetSurfaceContext(Dali::Integration::RenderSurfaceInterface* surface) const
{
  if(DALI_LIKELY(surface))
  {
    auto iter = std::find_if(mSurfaceContexts.begin(), mSurfaceContexts.end(), [surface](const SurfaceContextPair& iter)
    { return (iter.first == surface); });

    if(iter != mSurfaceContexts.end())
    {
      return iter->second.get();
    }
  }
  return nullptr;
}

} // namespace DALI_NAMESPACE::Graphics
