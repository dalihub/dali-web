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
#include <dali/internal/graphics/web/web-gles-fence.h>
#include <dali/internal/graphics/web/web-gles-failure.h>

namespace DALI_NAMESPACE::Graphics
{
namespace GLES
{
class WebSyncObject : public SyncObject
{
public:
  WebSyncObject(const SyncObjectCreateInfo& info, GlesGraphicsController& controller)
  : SyncObject(info, controller)
  {
  }
  bool InitializeResource() override
  {
    return mFence.Insert();
  }
  void DestroyResource() override
  {
    mFence.Release();
  }
  bool IsSynced() override
  {
    return mFence.Poll();
  }
private:
  WebFence mFence;
};

SyncPool::SyncContext GetTextureDependencySyncContext(GlesGraphicsController&)
{
  return SyncPool::SyncContext::GL;
}

void CollectSyncObjects()
{
  WebFence::Collect();
}
} // namespace GLES

Graphics::UniquePtr<SyncObject> GlesGraphicsController::CreateSyncObject(const SyncObjectCreateInfo& info, UniquePtr<SyncObject>&& oldObject)
{
  return NewObject<GLES::WebSyncObject, SyncDeleter>(info, *this, std::move(oldObject));
}

void GlesGraphicsController::ShutdownSyncObjects()
{
  GLES::WebFence::Shutdown();
  GLES::WebFailure::Stop();
}
} // namespace DALI_NAMESPACE::Graphics
