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
#include <dali/internal/graphics/gles-impl/gles2-graphics-memory.h>
#include <dali/internal/graphics/gles-impl/gles3-graphics-memory.h>
#include <dali/internal/graphics/common/graphics-interface.h>

namespace DALI_NAMESPACE::Graphics
{
Graphics::UniquePtr<Memory> GlesGraphicsController::MapBufferRange(const MapBufferInfo& mapInfo)
{
  // Mapping buffer requires the object to be created NOW
  // Workaround - flush now, otherwise there will be given a staging buffer
  // in case when the buffer is not there yet
  if(!mCreateBufferQueue.empty())
  {
    mGraphics->ActivateResourceContext();
    ProcessCreateQueues();
  }

  if(GetGLESVersion() < GLES::GLESVersion::GLES_30)
  {
    return Graphics::UniquePtr<Memory>(new GLES::Memory2(mapInfo, *this));
  }
  else
  {
    return Graphics::UniquePtr<Memory>(new GLES::Memory3(mapInfo, *this));
  }
}

bool GlesGraphicsController::CanSubmit()
{
  return true;
}

bool GlesGraphicsController::RetryResourceCreation()
{
  return true;
}

void GlesGraphicsController::ResourceCreationFailed()
{
}
} // namespace DALI_NAMESPACE::Graphics
