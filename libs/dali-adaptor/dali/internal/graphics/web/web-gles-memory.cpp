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
#include <dali/internal/graphics/web/web-buffer-memory.h>
#include <dali/internal/graphics/web/web-gles-failure.h>
#include <new>

namespace DALI_NAMESPACE::Graphics
{
Graphics::UniquePtr<Memory> GlesGraphicsController::MapBufferRange(const MapBufferInfo& info)
{
  if(!CanSubmit() || IsShuttingDown())
  {
    return nullptr;
  }
  if(!info.buffer || !info.size || (info.usage & (0 | MemoryUsageFlagBits::READ)) ||
     !(info.usage & (0 | MemoryUsageFlagBits::WRITE)))
  {
    GLES::WebFailure::Latch("unsupported buffer mapping");
    return nullptr;
  }
  if(!mCreateBufferQueue.empty())
  {
    ActivateResourceContext();
    ProcessCreateQueues();
  }
  auto* buffer = static_cast<GLES::Buffer*>(info.buffer);
  if(!CanSubmit())
  {
    return nullptr;
  }
  return Graphics::UniquePtr<Memory>(GLES::WebBufferMemory::Create(buffer->GetWebStorage(), info.offset, info.size));
}
bool GlesGraphicsController::CanSubmit()
{
  return GLES::WebFailure::CheckGl();
}
bool GlesGraphicsController::RetryResourceCreation()
{
  return !IsShuttingDown() && !GLES::WebFailure::HasFailure();
}
void GlesGraphicsController::ResourceCreationFailed()
{
  GLES::WebFailure::Latch("resource initialization failed");
}
}
