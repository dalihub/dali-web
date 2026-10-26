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

#include <dali/internal/graphics/gles-impl/gles-graphics-buffer.h>
#include <dali/internal/graphics/gles-impl/gles-graphics-controller.h>
#include <dali/internal/graphics/web/web-buffer-storage.h>
#include <dali/internal/graphics/web/web-gles-failure.h>
#include <GLES3/gl3.h>
#include <limits>
#include <new>

namespace DALI_NAMESPACE::Graphics::GLES
{
uint32_t Buffer::GetBufferChangedCount() const
{
  return mBufferChangedCount + (mWebStorage ? mWebStorage->Revision() : 0u);
}

bool Buffer::TryRecycle(const Graphics::BufferCreateInfo& info, Graphics::GlesGraphicsController& controller)
{
  if(GlesGraphicsController::IsShuttingDown() || WebFailure::HasFailure() || !mWebStorage || mWebStorage->mapped ||
     info.size != mCreateInfo.size || info.allocationCallbacks != mCreateInfo.allocationCallbacks ||
     info.propertiesFlags != mCreateInfo.propertiesFlags || info.usage != mCreateInfo.usage ||
     info.nextExtension != mCreateInfo.nextExtension)
  {
    return false;
  }
  // DISCARD/orphan starts a new zero-initialized generation; it is not a
  // promise to preserve the old generation's bytes.
  mBufferChangedCount += mWebStorage->Revision();
  mWebStorage->Detach();
  mWebStorage->Unreference();
  mWebStorage = nullptr;
  mBufferPtr = nullptr;
  controller.AddBuffer(*this);
  return true;
}
bool Buffer::InitializeResource()
{
  if(GlesGraphicsController::IsShuttingDown() || !WebFailure::CheckGl())
  {
    return false;
  }
  if(mWebStorage)
  {
    return true;
  }
  if(!mCreateInfo.size || mCreateInfo.size > uint32_t(std::numeric_limits<GLsizeiptr>::max()))
  {
    WebFailure::Latch("buffer size overflow or zero");
    return false;
  }
  mWebStorage = new(std::nothrow) WebBufferStorage(mCreateInfo.size, bool(mCpuAllocated), mCreateInfo.allocationCallbacks);
  if(!mWebStorage)
  {
    WebFailure::Latch("buffer storage allocation failed");
    return false;
  }
  if(mCpuAllocated)
  {
    if(!mWebStorage->Allocate())
    {
      return false;
    }
    mBufferPtr = mWebStorage->Data();
  }
  else
  {
    if(!mBufferId)
    {
      glGenBuffers(1, &mBufferId);
    }
    if(!mBufferId)
    {
      WebFailure::Latch("GL buffer allocation failed");
      return false;
    }
    mWebStorage->SetBufferId(mBufferId);
    glBindBuffer(GL_COPY_WRITE_BUFFER, mBufferId);
    glBufferData(GL_COPY_WRITE_BUFFER, mCreateInfo.size, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    if(!WebFailure::CheckGl())
    {
      return false;
    }
  }
  IncreaseBufferChangedCount();
  return true;
}
void Buffer::DestroyResource()
{
  if(mWebStorage)
  {
    mWebStorage->Detach();
    mWebStorage->Unreference();
    mWebStorage = nullptr;
  }
  mBufferPtr = nullptr;
  if(mBufferId)
  {
    // Context loss discards GPU objects. Shutdown must run before context
    // destruction; late Buffer deletion must not call a dead Controller.
    if(!GlesGraphicsController::IsShuttingDown() && WebFailure::CheckContext())
    {
      glDeleteBuffers(1, &mBufferId);
    }
    mBufferId = 0;
  }
}
}
