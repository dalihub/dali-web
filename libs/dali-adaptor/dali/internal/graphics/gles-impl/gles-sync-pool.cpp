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

// Class header
#include <dali/internal/graphics/gles-impl/gles-sync-pool.h>

// External Headers
#include <dali/devel-api/threading/mutex.h>
#include <dali/graphics-api/graphics-sync-object-create-info.h>

// Internal Headers
#include <dali/internal/graphics/gles-impl/gles-graphics-controller.h>
#include <dali/internal/graphics/gles-impl/gles-sync-policy.h>

namespace DALI_NAMESPACE::Graphics::GLES
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogSyncFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_FENCE_SYNC");
#endif
} // namespace

SyncPool::~SyncPool() = default;

SyncPool::SyncObjectId SyncPool::AllocateSyncObject(const Context* writeContext, SyncPool::SyncContext syncContext)
{
  auto agingSyncObject = std::make_unique<AgingSyncObject>(mController, writeContext, (syncContext == SyncContext::EGL));

  auto syncPoolObjectId = ++mSyncObjectId;
  if(DALI_UNLIKELY(syncPoolObjectId == INVALID_SYNC_OBJECT_ID))
  {
    syncPoolObjectId = ++mSyncObjectId;
  }

  // Take ownership of sync object
  mSyncObjects.insert(std::make_pair(syncPoolObjectId, std::move(agingSyncObject)));

  DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "context = %p, type = %d, [%d]\n", writeContext, syncContext, syncPoolObjectId);

  return syncPoolObjectId;
}

void SyncPool::Wait(SyncPool::SyncObjectId syncPoolObjectId)
{
  AgingSyncObject* agingSyncObject = GetAgingSyncObject(syncPoolObjectId);

  DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "[%d]\n", syncPoolObjectId);

  if(DALI_LIKELY(agingSyncObject != nullptr))
  {
    agingSyncObject->Wait();
  }
  else
  {
    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "Already synced [%d]\n", syncPoolObjectId);
  }
}

bool SyncPool::ClientWait(SyncPool::SyncObjectId syncPoolObjectId)
{
  AgingSyncObject* agingSyncObject = GetAgingSyncObject(syncPoolObjectId);

  DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "[%d]\n", syncPoolObjectId);

  if(DALI_LIKELY(agingSyncObject != nullptr))
  {
    return agingSyncObject->ClientWait();
  }
  else
  {
    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "Already synced [%d]\n", syncPoolObjectId);
  }

  return true;
}

void SyncPool::FreeSyncObject(SyncPool::SyncObjectId syncPoolObjectId)
{
  auto iter = mSyncObjects.find(syncPoolObjectId);
  if(iter != mSyncObjects.end())
  {
    // Move memory of sync object to discard queue
    DiscardAgingSyncObject(std::move(iter->second));

    mSyncObjects.erase(iter);

    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "Free: [%d]\n", syncPoolObjectId);
  }
  else
  {
    // Already freed. Do nothing
    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "Already freed: [%d]\n", syncPoolObjectId);
  }
}

/**
 * Age sync objects. Call at the end of each frame.
 * When a sync object is older than 2 frames, delete it.
 */
void SyncPool::AgeSyncObjects()
{
  CollectSyncObjects();
  mFrameCount++;

  if(!mSyncObjects.empty())
  {
    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "AgeSyncObjects: count: %d\n", mSyncObjects.size());

    // Age the remaining sync objects.
    for(auto iter = mSyncObjects.begin(); iter != mSyncObjects.end();)
    {
      auto* agingSyncObject = (iter->second).get();
      if(DALI_LIKELY(agingSyncObject))
      {
        if(agingSyncObject->Age() > 0)
        {
          ++iter;
        }
        else
        {
          DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "Discard [%d]\n", iter->first);

          // Move memory of sync object to discard queue
          DiscardAgingSyncObject(std::move(iter->second));

          iter = mSyncObjects.erase(iter);
        }
      }
      else
      {
        // Something wrong
        iter = mSyncObjects.erase(iter);
      }
    }

    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "AgeSyncObjects: count after erase: %d\n", mSyncObjects.size());
  }

  // Shared sync objects
  if(!mSharedSyncObjects.empty())
  {
    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "mSharedSyncObjects: count: %d\n", mSharedSyncObjects.size());

    // Age the remaining sync objects.
    for(auto iter = mSharedSyncObjects.begin(); iter != mSharedSyncObjects.end();)
    {
      if(iter->get()->UpdateAge() > 0)
      {
        ++iter;
      }
      else
      {
        iter = mSharedSyncObjects.erase(iter);
      }
    }

    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "mSharedSyncObjects: count after erase: %d\n", mSharedSyncObjects.size());
  }
}

void SyncPool::ProcessDiscardSyncObjects(const Context* currentContext)
{
  auto iter = mDiscardSyncObjects.find(currentContext);
  if(iter != mDiscardSyncObjects.end())
  {
#ifdef DEBUG_ENABLED
    auto& agingSyncObjectsList = iter->second;

    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "ProcessDiscardSyncObjects: context[%p], count: %zu\n", currentContext, agingSyncObjectsList.size());
#endif

    mDiscardSyncObjects.erase(iter);
  }
}

void SyncPool::InvalidateContext(const Context* invalidatedContext)
{
  if(!mSyncObjects.empty())
  {
    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "InvalidateContext: context[%p], count: %d\n", invalidatedContext, mSyncObjects.size());

    for(auto iter = mSyncObjects.begin(); iter != mSyncObjects.end();)
    {
      auto* agingSyncObject = (iter->second).get();
      if(agingSyncObject == nullptr ||
         agingSyncObject->writeContext == invalidatedContext)
      {
        // Release memory of sync object
        // Note : We don't need to call DiscardAgingSyncObject here.
        //        Even if current context is not given context, we should call it
        //        since some DALI objects should be destroyed.
        //        Don't worry about EGLSync leak, since it will be destroyed by eglDestroyContext.
        iter = mSyncObjects.erase(iter);
      }
      else
      {
        ++iter;
      }
    }

    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "InvalidateContext: context[%p], count after erase: %d\n", mSyncObjects.size());
  }

  // Release discarded sync objects.
  ProcessDiscardSyncObjects(invalidatedContext);
}

void SyncPool::DiscardAgingSyncObject(std::unique_ptr<AgingSyncObject>&& agingSyncObject)
{
  const Context* currentContext = agingSyncObject->writeContext;
  mDiscardSyncObjects[currentContext].emplace_back(std::move(agingSyncObject));
}

SyncPool::AgingSyncObject* SyncPool::GetAgingSyncObject(SyncPool::SyncObjectId syncPoolObjectId) const
{
  auto iter = mSyncObjects.find(syncPoolObjectId);
  if(iter != mSyncObjects.end())
  {
    return iter->second.get();
  }
  return nullptr;
}

std::shared_ptr<SyncPool::SharedSyncObject> SyncPool::AllocateSharedSyncObject(bool useNativeFenceFd)
{
  const Context* context = mController.GetCurrentContext();

  // Find existing one
  for(auto&& iter : mSharedSyncObjects)
  {
    if(iter->Match(context, mFrameCount, useNativeFenceFd))
    {
      // We already have a sync object for the current context and current frame
      DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "Already allocated [%p] [%p, %d]\n", iter.get(), context, mFrameCount, useNativeFenceFd);
      return iter;
    }
  }

  // Otherwise, allocate a new one
  auto syncObject = std::make_shared<SyncPool::SharedSyncObject>(mController, context, mFrameCount, useNativeFenceFd);

  mSharedSyncObjects.push_back(syncObject);

  DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "syncObject = %p, context = %p, frame = %d\n", syncObject.get(), context, mFrameCount);

  return syncObject;
}

} //namespace DALI_NAMESPACE::Graphics::GLES
