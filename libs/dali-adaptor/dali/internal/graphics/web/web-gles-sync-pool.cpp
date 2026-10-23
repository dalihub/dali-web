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

#include <dali/internal/graphics/gles-impl/gles-sync-pool.h>
#include <dali/internal/graphics/gles-impl/gles-graphics-controller.h>
#include <dali/internal/graphics/web/web-gles-fence.h>

namespace DALI_NAMESPACE::Graphics::GLES
{
struct SyncPool::SharedSyncObject::Impl
{
  Impl(GlesGraphicsController& controller, const Context* context, uint32_t frameCount, bool nativeFd)
  : context(context), frameCount(frameCount)
  {
    DALI_ASSERT_ALWAYS(!nativeFd && "Web GLES does not support native fence FD interop");
    DALI_ASSERT_ALWAYS(context && context == controller.GetCurrentContext());
    fence.Insert();
  }
  const Context* context;
  uint32_t frameCount;
  WebFence fence;
};

SyncPool::SharedSyncObject::SharedSyncObject(GlesGraphicsController& controller, const Context* context, uint32_t frameCount, bool nativeFd)
: mImpl(std::make_unique<Impl>(controller, context, frameCount, nativeFd))
{
}
SyncPool::SharedSyncObject::~SharedSyncObject() = default;

bool SyncPool::SharedSyncObject::Poll()
{
  return mImpl->fence.Poll();
}

bool SyncPool::SharedSyncObject::ClientWait()
{
  // Web waits are polls; never block the browser event loop.
  return Poll();
}

bool SyncPool::SharedSyncObject::IsFenceFdSupported()
{
  return false;
}

uint8_t SyncPool::SharedSyncObject::UpdateAge()
{
  mImpl->fence.Poll();
  return mImpl->fence.GetStatus() == WebFence::Status::Pending ? 1u : 0u;
}

bool SyncPool::SharedSyncObject::Match(const Context*, uint32_t, bool)
{
  // A fence inserted earlier in a frame cannot cover later submissions.
  return false;
}

struct SyncPool::AgingSyncObject::Impl
{
  explicit Impl(GlesGraphicsController& controller)
  : controller(controller)
  {
    fence.Insert();
  }
  GlesGraphicsController& controller;
  WebFence fence;
};

SyncPool::AgingSyncObject::AgingSyncObject(GlesGraphicsController& controller, const Context* context, bool egl)
: writeContext(context), mImpl(std::make_unique<Impl>(controller))
{
  DALI_ASSERT_ALWAYS(!egl && "Web GLES does not support EGL synchronization");
  DALI_ASSERT_ALWAYS(context && context == controller.GetCurrentContext());
}
SyncPool::AgingSyncObject::~AgingSyncObject() = default;

bool SyncPool::AgingSyncObject::ClientWait()
{
  return mImpl->fence.Poll();
}

void SyncPool::AgingSyncObject::Wait()
{
  // Single-context dependencies must use command order and never reach a
  // cross-context GPU wait. Do not emulate an unsupported wait with success.
  DALI_ASSERT_ALWAYS(false && "Web cross-context GPU waits are unsupported");
}

uint8_t SyncPool::AgingSyncObject::Age()
{
  mImpl->fence.Poll();
  // No frame-count approximation: pending fences stay pending indefinitely.
  return mImpl->fence.GetStatus() == WebFence::Status::Pending ? 1u : 0u;
}
} // namespace DALI_NAMESPACE::Graphics::GLES
