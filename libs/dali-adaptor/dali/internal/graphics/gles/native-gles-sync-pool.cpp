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
#include <dali/internal/graphics/gles/egl-sync-implementation.h>

namespace DALI_NAMESPACE::Graphics::GLES
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogSyncFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_FENCE_SYNC");
#endif
} // namespace

struct SyncPool::SharedSyncObject::Impl
{
  Impl(Graphics::GlesGraphicsController& controller, const Context* _context, uint32_t _frameCount, bool useNativeFenceFd)
  : controller(controller),
    context(_context),
    frameCount(_frameCount)
  {
    eglSyncObject = static_cast<Internal::Adaptor::EglSyncObject*>(controller.GetSyncImplementation().CreateSyncObject(Integration::GraphicsSyncAbstraction::SyncObject::SyncType::NATIVE_FENCE_SYNC));

    if(DALI_LIKELY(eglSyncObject) && useNativeFenceFd)
    {
      fenceFd = eglSyncObject->DuplicateNativeFenceFD();

      if(fenceFd != -1)
      {
        // Destroy the egl sync object. We don't need it.
        controller.GetSyncImplementation().DestroySyncObject(eglSyncObject);
        eglSyncObject = nullptr;
      }
    }
    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "SyncPool::SharedSyncObject::Impl: [%d] [%p]\n", fenceFd, this);
  }

  ~Impl()
  {
    if(fenceFd != -1)
    {
      Internal::Adaptor::NativeFence::CloseFD(fenceFd);
    }
    if(eglSyncObject)
    {
      if(DALI_LIKELY(!GlesGraphicsController::IsShuttingDown()))
      {
        // TODO : ~Impl could be called from any thread.
        // Could we guarentee that graphics controller is still valid at this moment?
        controller.GetSyncImplementation().DestroySyncObject(eglSyncObject);
      }
    }

    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "SyncPool::SharedSyncObject::~Impl: [%p]\n", this);
  }

  uint8_t UpdateAge()
  {
    Dali::Mutex::ScopedLock lock(mutex);
    uint8_t                 oldAge = age;
    if(age > 0)
    {
      age--;
    }
    if(oldAge == 0)
    {
      synced = true;

      if(fenceFd != -1)
      {
        DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "Set synced [%d]\n", fenceFd);

        Internal::Adaptor::NativeFence::CloseFD(fenceFd);
        fenceFd = -1;
      }
    }
    return oldAge;
  }

  bool ClientWait()
  {
    Dali::Mutex::ScopedLock lock(mutex);

    if(synced)
    {
      DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "Already synced [%p]\n", this);
      return true;
    }

    if(eglSyncObject)
    {
      if(DALI_LIKELY(!GlesGraphicsController::IsShuttingDown()))
      {
        DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "ClientWait() [%p]\n", this);
        eglSyncObject->ClientWait();
        synced = true;

        controller.GetSyncImplementation().DestroySyncObject(eglSyncObject);
      }
      eglSyncObject = nullptr;
    }
    else
    {
      PollWithoutLock();
    }

    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "ClientWait(); Result: %s\n", synced ? "Synced" : "NOT SYNCED");

    return synced;
  }

  bool Poll()
  {
    Dali::Mutex::ScopedLock lock(mutex);

    return PollWithoutLock();
  }

  bool IsFenceFdSupported()
  {
    return fenceFd == -1 ? false : true;
  }

private:
  // Should be called with mutex lock
  bool PollWithoutLock()
  {
    if(synced)
    {
      DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "Already synced [%d]\n", fenceFd);
      return true;
    }
    else
    {
      if(fenceFd != -1)
      {
        DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "AgingSyncObject::ClientWait(); Poll [%d]\n", fenceFd);

        synced  = Internal::Adaptor::NativeFence::PollFD(fenceFd);
        fenceFd = -1;
      }
      else
      {
        DALI_LOG_ERROR("Something wrong! [%p]\n", this);
      }
    }

    return synced;
  }

public:
  GlesGraphicsController&            controller;
  const Context*                    context{nullptr};
  Internal::Adaptor::EglSyncObject* eglSyncObject{nullptr};
  Mutex                             mutex;

  uint32_t frameCount{0u};
  int32_t  fenceFd{-1};
  uint8_t  age{3u};
  bool     synced{false};
};

SyncPool::SharedSyncObject::SharedSyncObject(Graphics::GlesGraphicsController& controller, const Context* context, uint32_t frameCount, bool useNativeFenceFd)
{
  mImpl = std::unique_ptr<Impl>(new Impl(controller, context, frameCount, useNativeFenceFd));
}

SyncPool::SharedSyncObject::~SharedSyncObject() = default;

bool SyncPool::SharedSyncObject::Poll()
{
  return mImpl->Poll();
}

bool SyncPool::SharedSyncObject::ClientWait()
{
  return mImpl->ClientWait();
}

bool SyncPool::SharedSyncObject::IsFenceFdSupported()
{
  return mImpl->IsFenceFdSupported();
}

uint8_t SyncPool::SharedSyncObject::UpdateAge()
{
  return mImpl->UpdateAge();
}

bool SyncPool::SharedSyncObject::Match(const Context* context, uint32_t frameCount, bool useNativeFenceFd)
{
  if(mImpl->context == context && mImpl->frameCount == frameCount && useNativeFenceFd)
  {
    return true;
  }
  return false;
}

struct SyncPool::AgingSyncObject::Impl
{
  Impl(GlesGraphicsController& controller, const Context* writeContext, bool egl);
  ~Impl();
  void Wait();
  bool ClientWait();
  GlesGraphicsController& controller;
  const Context* writeContext;
  union
  {
    GLsync glSyncObject;
    Internal::Adaptor::EglSyncObject* eglSyncObject;
  };
  bool synced{false};
  bool egl{false};
};

SyncPool::AgingSyncObject::Impl::Impl(Graphics::GlesGraphicsController& controller, const Context* writeContext, bool _egl)
: controller(controller),
  writeContext(writeContext),
  egl(_egl)
{
  if(egl)
  {
    eglSyncObject = static_cast<Internal::Adaptor::EglSyncObject*>(controller.GetSyncImplementation().CreateSyncObject(Integration::GraphicsSyncAbstraction::SyncObject::SyncType::FENCE_SYNC));
    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "AgingSyncObject::cons; EGL::CreateSyncObject: %p\n", eglSyncObject);
  }
  else
  {
    auto* gl = controller.GetGL();
    if(DALI_LIKELY(gl))
    {
      glSyncObject = gl->FenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
      if(DALI_UNLIKELY(glSyncObject == nullptr))
      {
        // Windows backend last-resort fallback if ANGLE cannot create a GLES
        // fence. Rendering without an explicit barrier may appear correct due
        // to ANGLE or driver scheduling, but cross-context ordering is not
        // guaranteed. Finish() guarantees producer completion, at the cost of
        // a CPU/GPU stall that can reduce multi-pass rendering performance.
        gl->Finish();
        synced = true;
      }
    }
  }
}

SyncPool::AgingSyncObject::Impl::~Impl()
{
  if(DALI_LIKELY(!GlesGraphicsController::IsShuttingDown()))
  {
    if(egl)
    {
      DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "AgingSyncObject::dstr; EGL::DestroySyncObject: %p\n", eglSyncObject);
      controller.GetSyncImplementation().DestroySyncObject(eglSyncObject);
    }
    else
    {
      auto* gl = controller.GetGL();
      if(DALI_LIKELY(gl) && glSyncObject != nullptr)
      {
        gl->DeleteSync(glSyncObject);
      }
    }
  }
}

bool SyncPool::AgingSyncObject::Impl::ClientWait()
{
  if(synced)
  {
    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "Already synced\n");
    return true;
  }

  if(egl)
  {
    if(eglSyncObject)
    {
      DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "AgingSyncObject::ClientWait(); EGL::ClientWaitSync\n");
      eglSyncObject->ClientWait();
      synced = true;
    }
  }
  else
  {
    auto* gl = controller.GetGL();
    if(DALI_LIKELY(gl) && glSyncObject)
    {
      DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "AgingSyncObject::ClientWait(); glClientWaitSync 1ms\n");
      const GLuint64 TIMEOUT = 1000000; //1ms!
      GLenum         result  = gl->ClientWaitSync(glSyncObject, GL_SYNC_FLUSH_COMMANDS_BIT, TIMEOUT);

      synced = (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED);
    }
  }

  DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "AgingSyncObject::ClientWait(); Result: %s\n", synced ? "Synced" : "NOT SYNCED");

  return synced;
}

void SyncPool::AgingSyncObject::Impl::Wait()
{
  if(synced)
  {
    DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "Already synced\n");
    return;
  }

  if(egl)
  {
    if(eglSyncObject)
    {
      DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "AgingSyncObject::Wait(); EGL::WaitSync\n");
      eglSyncObject->Wait();
    }
  }
  else
  {
    auto* gl = controller.GetGL();
    if(DALI_LIKELY(gl) && glSyncObject)
    {
      DALI_LOG_INFO(gLogSyncFilter, Debug::Verbose, "AgingSyncObject::Wait(); glWaitSync\n");
      gl->WaitSync(glSyncObject, 0, GL_TIMEOUT_IGNORED);
    }
  }
  synced = true;
}

SyncPool::AgingSyncObject::AgingSyncObject(GlesGraphicsController& controller, const Context* context, bool egl)
: writeContext(context),
  mImpl(std::make_unique<Impl>(controller, context, egl))
{
}

SyncPool::AgingSyncObject::~AgingSyncObject() = default;

void SyncPool::AgingSyncObject::Wait()
{
  mImpl->Wait();
}

bool SyncPool::AgingSyncObject::ClientWait()
{
  return mImpl->ClientWait();
}

uint8_t SyncPool::AgingSyncObject::Age()
{
  uint8_t oldAge = age;
  if(age > 0)
  {
    age--;
  }
  return oldAge;
}


} // namespace DALI_NAMESPACE::Graphics::GLES
