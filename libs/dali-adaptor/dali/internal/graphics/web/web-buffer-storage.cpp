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

#include <dali/internal/graphics/web/web-buffer-storage.h>
#include <dali/internal/graphics/web/web-gles-failure.h>
#include <GLES3/gl3.h>
#include <cstdlib>
#include <cstring>

namespace DALI_NAMESPACE::Graphics::GLES
{
WebBufferStorage::WebBufferStorage(uint32_t size, bool cpu, const AllocationCallbacks* allocator)
: mSize(size), mCpu(cpu)
{
  if(allocator)
  {
    mAllocator = *allocator;
  }
}
void WebBufferStorage::Unreference()
{
  if(--mReferences == 0)
  {
    delete this;
  }
}
WebBufferStorage::~WebBufferStorage()
{
  if(mData)
  {
    if(mAllocator.freeCallback)
    {
      mAllocator.freeCallback(mData, mAllocator.userData);
    }
    else
    {
      std::free(mData);
    }
  }
}
bool WebBufferStorage::IsAlive() const
{
  return mAlive && !WebFailure::IsStopped();
}
void WebBufferStorage::Detach()
{
  mAlive = false;
}
bool WebBufferStorage::Allocate()
{
  if(mData)
  {
    return true;
  }
  if(!IsAlive() || !mSize || WebFailure::HasFailure())
  {
    return false;
  }
  if(bool(mAllocator.allocCallback) != bool(mAllocator.freeCallback))
  {
    WebFailure::Latch("incomplete buffer allocator");
    return false;
  }
  mData = static_cast<unsigned char*>(mAllocator.allocCallback ?
    mAllocator.allocCallback(mSize, 0, mAllocator.userData) : std::malloc(mSize));
  if(!mData)
  {
    WebFailure::Latch("buffer shadow allocation failed");
    return false;
  }
  std::memset(mData, 0, mSize);
  // Mapping has no full-overwrite hint. Initialize the first shadow and GPU
  // consistently before callbacks/partial writes can observe preserved bytes.
  return mCpu || Upload(0, mSize);
}
bool WebBufferStorage::Upload(uint32_t offset, uint32_t size)
{
  if(!IsAlive() || WebFailure::HasFailure())
  {
    return false;
  }
  if(!mData || offset > mSize || size > mSize - offset)
  {
    WebFailure::Latch("buffer upload bounds");
    return false;
  }
  if(!mCpu && size)
  {
    if(!WebFailure::CheckGl())
    {
      return false;
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, mBufferId);
    glBufferSubData(GL_COPY_WRITE_BUFFER, offset, size, mData + offset);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    if(!WebFailure::CheckGl())
    {
      return false;
    }
  }
  ++mRevision;
  return true;
}
}
