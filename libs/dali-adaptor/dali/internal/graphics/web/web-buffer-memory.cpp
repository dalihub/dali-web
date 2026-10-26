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

#include <dali/internal/graphics/web/web-buffer-memory.h>
#include <dali/internal/graphics/web/web-gles-failure.h>
#include <new>

namespace DALI_NAMESPACE::Graphics::GLES
{
WebBufferMemory* WebBufferMemory::Create(WebBufferStorage* storage, uint32_t offset, uint32_t size)
{
  if(WebFailure::HasFailure() || WebFailure::IsStopped())
  {
    return nullptr;
  }
  if(!storage || !storage->IsAlive() || !size || offset > storage->Size() || size > storage->Size() - offset || storage->mapped)
  {
    WebFailure::Latch("buffer map bounds, lifetime or duplicate map");
    return nullptr;
  }
  if(!storage->Allocate())
  {
    return nullptr;
  }
  auto* memory = new(std::nothrow) WebBufferMemory(storage, offset, size);
  if(!memory)
  {
    WebFailure::Latch("mapping allocation failed");
  }
  return memory;
}
WebBufferMemory::WebBufferMemory(WebBufferStorage* storage, uint32_t offset, uint32_t size)
: mStorage(storage), mOffset(offset), mSize(size)
{
  mStorage->Reference();
  mStorage->mapped = true;
}
WebBufferMemory::~WebBufferMemory()
{
  Unlock(true);
  mStorage->mapped = false;
  mStorage->Unreference();
}
void* WebBufferMemory::LockRegion(uint32_t offset, uint32_t size)
{
  if(!mStorage->IsAlive() || WebFailure::HasFailure())
  {
    return nullptr;
  }
  if(!mStorage->Data() || mOffset > mStorage->Size() || mSize > mStorage->Size() - mOffset ||
     mLocked || offset > mSize || size > mSize - offset || !size)
  {
    WebFailure::Latch("buffer lock bounds or duplicate lock");
    return nullptr;
  }
  mLocked = true;
  mDirty = true;
  return mStorage->Data() + mOffset + offset;
}
void WebBufferMemory::Unlock(bool flush)
{
  mLocked = false;
  if(flush)
  {
    Flush();
  }
}
void WebBufferMemory::Flush()
{
  if(mDirty)
  {
    // Callback byte count does not describe a dirty location. Preserve and
    // upload the mapping conservatively, including Unlock(false) then unmap.
    mStorage->Upload(mOffset, mSize);
    mDirty = mLocked;
  }
}
}
