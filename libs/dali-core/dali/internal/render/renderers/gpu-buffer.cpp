/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/devel-api/signals/vertex-buffer-update-callback.h> // Dali::VertexBufferUpdateCallback
#include <dali/internal/render/renderers/gpu-buffer.h>

#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-types.h>
#include <dali/public-api/common/dali-common.h>
#include <cstring>

namespace DALI_NAMESPACE
{
namespace Internal
{
namespace
{
} // namespace

GpuBuffer::GpuBuffer(Graphics::Controller& graphicsController, Graphics::BufferUsageFlags usage, GpuBuffer::WritePolicy writePolicy)
: mUsage(usage),
  mWritePolicy(writePolicy)
{
}

bool GpuBuffer::UpdateDataBuffer(Graphics::Controller& graphicsController, uint32_t size, const void* data)
{
  mValid = false;
  if(!size || !data)
  {
    DALI_LOG_ERROR("Invalid buffer upload data or size\n");
    return false;
  }

  /**
   *  We will create a new buffer in following cases:
   *  1. The buffer doesn't exist
   *  2. The spec changes (size of new buffer changes)
   *  3. The policy is to discard on write
   *
   *  Third option will try to recycle existing buffer when its spec stays unchanged.
   *  It will allow the GPU driver to release the underlying memory and attach new memory
   *  to the GPU buffer object. It will prevent memory locking during writes in case the
   *  buffer is in use by the GPU.
   */
  if(!mGraphicsObject || size > mCapacity || mWritePolicy == WritePolicy::DISCARD)
  {
    Graphics::BufferCreateInfo createInfo{};
    createInfo.SetUsage(mUsage).SetSize(size);
    mGraphicsObject = graphicsController.CreateBuffer(createInfo, std::move(mGraphicsObject));
    mCapacity       = size;
  }

  if(!mGraphicsObject)
  {
    mCapacity = 0;
    DALI_LOG_ERROR("Failed to create upload buffer\n");
    return false;
  }

  Graphics::MapBufferInfo info{};
  info.buffer = mGraphicsObject.get();
  info.usage  = 0 | Graphics::MemoryUsageFlagBits::WRITE;
  info.offset = 0;
  info.size   = size;

  auto memory = graphicsController.MapBufferRange(info);
  if(!memory)
  {
    DALI_LOG_ERROR("Failed to map upload buffer\n");
    return false;
  }
  void* ptr = memory->LockRegion(0, size);
  if(!ptr)
  {
    graphicsController.UnmapMemory(std::move(memory));
    DALI_LOG_ERROR("Failed to lock upload buffer\n");
    return false;
  }
  memcpy(ptr, data, size);
  memory->Unlock(true);
  graphicsController.UnmapMemory(std::move(memory));
  mSize = size;
  mValid = true;
  return true;
}

bool GpuBuffer::UpdateDataBufferWithCallback(Graphics::Controller& graphicsController, Dali::VertexBufferUpdateCallback* callback, uint32_t& bytesUpdatedCount)
{
  mValid = false;
  bytesUpdatedCount = 0;
  if(!callback || !mSize)
  {
    return false;
  }
  // create or orphan object so mapping can be instant
  if(!mGraphicsObject || mWritePolicy == WritePolicy::DISCARD)
  {
    Graphics::BufferCreateInfo createInfo{};
    createInfo.SetUsage(mUsage).SetSize(mSize);
    mGraphicsObject = graphicsController.CreateBuffer(createInfo, std::move(mGraphicsObject));
    mCapacity       = mSize;
  }

  if(!mGraphicsObject)
  {
    mCapacity = 0;
    DALI_LOG_ERROR("Failed to create upload buffer\n");
    return false;
  }

  Graphics::MapBufferInfo info{};
  info.buffer = mGraphicsObject.get();
  info.usage  = 0 | Graphics::MemoryUsageFlagBits::WRITE;
  info.offset = 0;
  info.size   = mSize; // map entire buffer

  auto memory = graphicsController.MapBufferRange(info);
  if(!memory)
  {
    DALI_LOG_ERROR("Failed to map callback buffer\n");
    return false;
  }
  void* ptr = memory->LockRegion(0, mSize);
  if(!ptr)
  {
    graphicsController.UnmapMemory(std::move(memory));
    DALI_LOG_ERROR("Failed to lock callback buffer\n");
    return false;
  }

  bytesUpdatedCount = callback->Invoke(ptr, mSize); // passing low level binary size, not element count!

  memory->Unlock(true);
  graphicsController.UnmapMemory(std::move(memory));
  if(bytesUpdatedCount > mSize)
  {
    bytesUpdatedCount = 0;
    DALI_LOG_ERROR("Callback returned more bytes than buffer capacity\n");
    return false;
  }
  mValid = true;
  return true;
}

void GpuBuffer::Destroy()
{
  mValid = false;
  mCapacity = 0;
  mSize     = 0;
  mGraphicsObject.reset();
}

} // namespace Internal

} //namespace DALI_NAMESPACE
