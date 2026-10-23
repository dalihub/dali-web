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

#include <dali/internal/graphics/gles-impl/gles-sync-factory.h>
#include <dali/internal/graphics/gles-impl/gles-sync-policy.h>
#include <dali/internal/graphics/gles-impl/gles-sync-object.h>
#include <dali/internal/graphics/gles-impl/egl-sync-object.h>
#include <dali/internal/graphics/gles/egl-sync-implementation.h>

namespace DALI_NAMESPACE::Graphics
{
Graphics::UniquePtr<SyncObject> GlesGraphicsController::CreateSyncObject(const SyncObjectCreateInfo& syncObjectCreateInfo,
                                                                        UniquePtr<SyncObject>&&     oldSyncObject)
{
  if(GetGLESVersion() < GLES::GLESVersion::GLES_30)
  {
    return NewObject<EGL::SyncObject, SyncDeleter>(syncObjectCreateInfo, *this, std::move(oldSyncObject));
  }
  else
  {
    return NewObject<GLES::SyncObject, SyncDeleter>(syncObjectCreateInfo, *this, std::move(oldSyncObject));
  }
}

void GlesGraphicsController::ShutdownSyncObjects()
{
}

namespace GLES
{
void CollectSyncObjects()
{
}

SyncPool::SyncContext GetTextureDependencySyncContext(GlesGraphicsController& controller)
{
  return static_cast<Internal::Adaptor::EglSyncImplementation&>(controller.GetSyncImplementation()).UseGlSyncForTextureDependencies()
           ? SyncPool::SyncContext::GL
           : SyncPool::SyncContext::EGL;
}

} // namespace GLES
} // namespace DALI_NAMESPACE::Graphics
