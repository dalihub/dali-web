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

// CLASS HEADER
#include <dali/internal/render/renderers/uniform-buffer.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

#ifdef DEBUG_ENABLED
Debug::Filter* gUniformBufferLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_UNIFORM_BUFFER");
#endif

namespace DALI_NAMESPACE::Internal::Render
{
namespace
{
// GPU UBOs need to be double-buffered in order to avoid stalling the CPU during mapping/unmapping
constexpr uint32_t INTERNAL_UBO_BUFFER_COUNT = 2u;
constexpr uint32_t DEFAUT_MEMORY_ALIGNMENT{1};

} // namespace

Graphics::UniquePtr<UniformBufferV2> UniformBufferV2::New(Dali::Graphics::Controller* controller, bool emulated, uint32_t alignment)
{
  return Graphics::UniquePtr<UniformBufferV2>(new UniformBufferV2(controller, emulated, alignment));
}

UniformBufferV2::UniformBufferV2(Dali::Graphics::Controller* controller, bool emulated, uint32_t alignment)
: mController(controller),
  mBlockAlignment(alignment ? alignment : DEFAUT_MEMORY_ALIGNMENT),
  mCurrentGraphicsBufferIndex(0),
  mEmulated(emulated)
{
  mBufferList.resize(emulated ? 1 : INTERNAL_UBO_BUFFER_COUNT);
  for(size_t i = 0; i < mBufferList.size(); ++i)
  {
    mBufferList[i].graphicsBuffer = nullptr;
    mBufferList[i].graphicsMemory = nullptr;
  }
}

void UniformBufferV2::ReSpecify(uint32_t sizeInBytes)
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Respec(%p) [%d] BufferType:%s  newSize:%d\n", this, mCurrentGraphicsBufferIndex, mEmulated ? "CPU" : "GPU", sizeInBytes);
  if(mFailed)
  {
    mBufferList[mCurrentGraphicsBufferIndex].graphicsMemory.reset();
    mMappedPtr = nullptr;
  }
  mFailed = false;
  if(mEmulated)
  {
    ReSpecifyCPU(sizeInBytes);
  }
  else
  {
    ReSpecifyGPU(sizeInBytes);
  }
}

bool UniformBufferV2::Write(const void* data, uint32_t size, uint32_t offset)
{
  const auto capacity = GetCurrentCapacity();
  if(mFailed || offset > capacity || size > capacity - offset || (size && !data))
  {
    MarkFailed();
    return false;
  }
  if(!size)
  {
    return true;
  }
  if(!mMappedPtr)
  {
    Map();
  }
  if(mFailed || !mMappedPtr)
  {
    MarkFailed();
    return false;
  }
  memcpy(static_cast<uint8_t*>(mMappedPtr) + offset, data, size);
  return true;
}

bool UniformBufferV2::IsValid() const
{
  return !mFailed;
}

void UniformBufferV2::MarkFailed()
{
  if(!mFailed)
  {
    DALI_LOG_ERROR("Uniform buffer preparation failed; discard dependent commands\n");
  }
  mFailed = true;
}

void UniformBufferV2::Map()
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Map(%p) [%d] BufferType:%s\n", this, mCurrentGraphicsBufferIndex, mEmulated ? "CPU" : "GPU");
  if(mEmulated)
  {
    MapCPU();
  }
  else
  {
    MapGPU();
  }
}

void UniformBufferV2::Unmap()
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Unmap(%p) [%d] BufferType:%s\n", this, mCurrentGraphicsBufferIndex, mEmulated ? "CPU" : "GPU");
  if(mEmulated)
  {
    UnmapCPU();
  }
  else
  {
    UnmapGPU();
  }
}

bool UniformBufferV2::Flush()
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Flush(%p) [%d] BufferType:%s\n", this, mCurrentGraphicsBufferIndex, mEmulated ? "CPU" : "GPU");

  // Flush only for GPU buffertype by unmapping
  if(!mEmulated && mBufferList[mCurrentGraphicsBufferIndex].graphicsMemory)
  {
    auto& buffer = mBufferList[mCurrentGraphicsBufferIndex];
    if(buffer.graphicsMemory)
    {
      UnmapGPU();
      // flush range?
    }

    // Swap buffers for GPU UBOs
    if(!mFailed)
    {
      mCurrentGraphicsBufferIndex = (mCurrentGraphicsBufferIndex + 1) % mBufferList.size();
    }
    DALI_ASSERT_DEBUG(mCurrentGraphicsBufferIndex < mBufferList.size());
  }
  return !mFailed;
}

void UniformBufferV2::Rollback()
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Rollback(%p) [%d]\n", this, mCurrentGraphicsBufferIndex);
  if(!mBufferList.empty())
  {
    mBufferList[mCurrentGraphicsBufferIndex].currentOffset = 0; // reset offset
  }
}

bool UniformBufferV2::IncrementOffsetBy(uint32_t value)
{
  auto& buffer = mBufferList[mCurrentGraphicsBufferIndex];
  const uint32_t padding = value % mBlockAlignment ? mBlockAlignment - value % mBlockAlignment : 0u;
  if(mFailed || buffer.currentOffset > buffer.capacity || value > buffer.capacity - buffer.currentOffset || padding > buffer.capacity - buffer.currentOffset - value)
  {
    MarkFailed();
    return false;
  }
  buffer.currentOffset += value + padding;
  return true;
}

bool UniformBufferV2::MemoryCompare(void* data, uint32_t offset, uint32_t size)
{
  const auto capacity = GetCurrentCapacity();
  if(mFailed || offset > capacity || size > capacity - offset || (size && (!data || !mMappedPtr)))
  {
    MarkFailed();
    return false;
  }
  return !size || !memcmp(data, static_cast<uint8_t*>(mMappedPtr) + offset, size);
}

uint32_t UniformBufferV2::GetBlockAlignment() const
{
  return mBlockAlignment;
}

uint32_t UniformBufferV2::GetCurrentOffset() const
{
  if(!mBufferList.empty())
  {
    return mBufferList[mCurrentGraphicsBufferIndex].currentOffset;
  }
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::General, "Buffer should be allocated before getting offset\n");
  return 0;
}

uint32_t UniformBufferV2::GetCurrentCapacity() const
{
  uint32_t capacity = 0;
  if(!mBufferList.empty())
  {
    DALI_ASSERT_DEBUG(mBufferList.size() > mCurrentGraphicsBufferIndex);
    capacity = mBufferList[mCurrentGraphicsBufferIndex].capacity;
  }
  return capacity;
}

Dali::Graphics::Buffer* UniformBufferV2::GetGraphicsBuffer() const
{
  return mFailed ? nullptr : mBufferList[mCurrentGraphicsBufferIndex].graphicsBuffer.get();
}

void UniformBufferV2::ReSpecifyCPU(uint32_t sizeInBytes)
{
  GfxBuffer gfxBuffer;

  uint32_t currentCapacity = GetCurrentCapacity();

  if(sizeInBytes > currentCapacity)
  {
    Graphics::UniquePtr<Graphics::Buffer> oldBuffer{nullptr};

    // If the CPU buffer already exist use it when applying respec
    if(!mBufferList.empty())
    {
      mBufferList[0].graphicsMemory = nullptr;                                  // Discard mapped memory if exists
      oldBuffer                     = std::move(mBufferList[0].graphicsBuffer); // Store old buffer for re-using
    }
    Graphics::BufferPropertiesFlags flags = 0u | Graphics::BufferPropertiesFlagBit::CPU_ALLOCATED;

    auto createInfo = Graphics::BufferCreateInfo()
                        .SetSize(sizeInBytes)
                        .SetBufferPropertiesFlags(flags)
                        .SetUsage(0u | Graphics::BufferUsage::UNIFORM_BUFFER);

    gfxBuffer.graphicsBuffer = mController->CreateBuffer(createInfo, std::move(oldBuffer));
    gfxBuffer.capacity = gfxBuffer.graphicsBuffer ? sizeInBytes : 0;
    gfxBuffer.currentOffset  = 0;

    mBufferList[0] = std::move(gfxBuffer);
    if(!mBufferList[mCurrentGraphicsBufferIndex].graphicsBuffer)
    {
      mMappedPtr = nullptr;
      MarkFailed();
      return;
    }
    // make sure buffer is created (move creation to run in parallel in the backed
    // as this may be a major slowdown)
    mController->WaitIdle();
  }

  mMappedPtr = nullptr;

  if(sizeInBytes)
  {
    // After respecifying the buffer, we can map it persistently as it already exists
    // in the CPU memory
    MapCPU();
  }
}

void UniformBufferV2::ReSpecifyGPU(uint32_t sizeInBytes)
{
  uint32_t currentCapacity = GetCurrentCapacity();

  if(sizeInBytes > currentCapacity)
  {
    GfxBuffer                             gfxBuffer;
    Graphics::UniquePtr<Graphics::Buffer> oldBuffer{nullptr};

    // If the GPU buffer already exist use it when applying respec
    if(!mBufferList.empty())
    {
      mBufferList[mCurrentGraphicsBufferIndex].graphicsMemory = nullptr;                                                            // Discard mapped memory if exists
      oldBuffer                                               = std::move(mBufferList[mCurrentGraphicsBufferIndex].graphicsBuffer); // Store old buffer for re-using
    }
    Graphics::BufferPropertiesFlags flags = 0u;

    auto createInfo = Graphics::BufferCreateInfo()
                        .SetSize(sizeInBytes)
                        .SetBufferPropertiesFlags(flags)
                        .SetUsage(0u | Graphics::BufferUsage::UNIFORM_BUFFER);

    gfxBuffer.graphicsBuffer = mController->CreateBuffer(createInfo, std::move(oldBuffer));
    gfxBuffer.capacity = gfxBuffer.graphicsBuffer ? sizeInBytes : 0;
    gfxBuffer.currentOffset  = 0;

    mBufferList[mCurrentGraphicsBufferIndex] = std::move(gfxBuffer);
    if(!mBufferList[mCurrentGraphicsBufferIndex].graphicsBuffer)
    {
      mMappedPtr = nullptr;
      MarkFailed();
      return;
    }
    // make sure buffer is created (move creation to run in parallel in the backend
    // as this may be a major slowdown)
    mController->WaitIdle();
    DALI_ASSERT_ALWAYS(mBufferList[mCurrentGraphicsBufferIndex].capacity == sizeInBytes && "std::move failed");
  }

  mMappedPtr = nullptr;

  if(sizeInBytes)
  {
    MapGPU(); // Note, this will flush the creation buffer queues in the backend and initialize buffer.
  }
}

void UniformBufferV2::MapCPU()
{
  auto& buffer = mBufferList[0]; // CPU contains always one buffer
  if(mFailed || !buffer.graphicsBuffer)
  {
    MarkFailed();
    return;
  }
  if(!buffer.graphicsMemory)
  {
    Graphics::MapBufferInfo info{};
    info.buffer           = buffer.graphicsBuffer.get();
    info.usage            = 0 | Graphics::MemoryUsageFlagBits::WRITE;
    info.offset           = 0;
    info.size             = buffer.capacity;
    buffer.graphicsMemory = mController->MapBufferRange(info);
  }

  // obtain pointer instantly
  if(buffer.graphicsMemory)
  {
    mMappedPtr = buffer.graphicsMemory->LockRegion(0, buffer.capacity);
    DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "CPU buffer is mapped to %p\n", mMappedPtr);
  }
  if(!mMappedPtr)
  {
    MarkFailed();
  }
}

void UniformBufferV2::MapGPU()
{
  auto& buffer = mBufferList[mCurrentGraphicsBufferIndex];
  if(mFailed || !buffer.graphicsBuffer)
  {
    MarkFailed();
    return;
  }
  if(!buffer.graphicsMemory)
  {
    Graphics::MapBufferInfo info{};
    info.buffer           = buffer.graphicsBuffer.get();
    info.usage            = 0 | Graphics::MemoryUsageFlagBits::WRITE;
    info.offset           = 0;
    info.size             = buffer.capacity;
    buffer.graphicsMemory = mController->MapBufferRange(info);
  }

  // obtain pointer instantly
  if(buffer.graphicsMemory)
  {
    mMappedPtr = buffer.graphicsMemory->LockRegion(0, buffer.capacity);
    DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "GPU buffer is mapped to %p\n", mMappedPtr);
  }
  if(!mMappedPtr)
  {
    MarkFailed();
  }
}

void UniformBufferV2::UnmapCPU()
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "CPU buffer is unmapped\n");
}

void UniformBufferV2::UnmapGPU()
{
  auto& buffer = mBufferList[mCurrentGraphicsBufferIndex];
  if(buffer.graphicsMemory)
  {
    mController->UnmapMemory(std::move(buffer.graphicsMemory));
    buffer.graphicsMemory = nullptr;
  }
  mMappedPtr = nullptr;
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "GPU buffer is unmapped\n");
}

} //namespace DALI_NAMESPACE::Internal::Render
