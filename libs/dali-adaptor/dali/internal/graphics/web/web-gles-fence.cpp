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

#include <dali/internal/graphics/web/web-gles-fence.h>
#include <dali/integration-api/debug.h>
#include <GLES3/gl3.h>
#include <emscripten/html5_webgl.h>
#include <atomic>
#include <thread>
#include <vector>

namespace DALI_NAMESPACE::Graphics::GLES
{
struct WebFence::State
{
  static thread_local std::vector<std::shared_ptr<State>> fences;
  static thread_local bool failed;
  std::atomic<Status> status{Status::Empty};
  std::atomic<bool> releaseRequested{false};
  std::thread::id owner;
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context{0};
  GLsync sync{nullptr};

  bool IsOwner() const
  {
    return owner == std::this_thread::get_id();
  }

  void Delete()
  {
    DALI_ASSERT_ALWAYS(IsOwner() && "Web fence deletion requires its render owner");
    if(sync)
    {
      // Lost contexts already discard their GL objects; never delete against a
      // replacement context with a coincidentally reused object name.
      if(!emscripten_is_webgl_context_lost(context))
      {
        DALI_ASSERT_ALWAYS(emscripten_webgl_get_current_context() == context);
        glDeleteSync(sync);
      }
      sync = nullptr;
    }
  }

  void Poll()
  {
    if(status.load() != Status::Pending || !IsOwner())
    {
      return;
    }
    if(emscripten_is_webgl_context_lost(context))
    {
      sync = nullptr;
      status.store(Status::Failed);
      failed = true;
      DALI_LOG_ERROR("Web fence context lost\n");
      return;
    }
    DALI_ASSERT_ALWAYS(emscripten_webgl_get_current_context() == context);
    const GLenum result = glClientWaitSync(sync, 0, 0);
    if(result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED)
    {
      Delete();
      status.store(Status::Complete);
    }
    else if(result != GL_TIMEOUT_EXPIRED)
    {
      Delete();
      status.store(Status::Failed);
      failed = true;
      DALI_LOG_ERROR("Web fence poll failed: 0x%x\n", result);
    }
  }
};

thread_local std::vector<std::shared_ptr<WebFence::State>> WebFence::State::fences;
thread_local bool WebFence::State::failed = false;

WebFence::WebFence() = default;

WebFence::~WebFence()
{
  Release();
}

bool WebFence::Insert()
{
  DALI_ASSERT_ALWAYS(!mState || mState->IsOwner());
  Release();
  mState = std::make_shared<State>();
  mState->owner = std::this_thread::get_id();
  mState->context = emscripten_webgl_get_current_context();
  DALI_ASSERT_ALWAYS(mState->context > 0 && "Web fence needs a current WebGL2 context");
  if(!emscripten_is_webgl_context_lost(mState->context))
  {
    mState->sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  }
  if(!mState->sync)
  {
    mState->status.store(Status::Failed);
    State::failed = true;
    DALI_LOG_ERROR("Web fence creation failed\n");
    return false;
  }
  mState->status.store(Status::Pending);
  State::fences.push_back(mState);
  // Submission is not completion. Poll must observe a signalled fence later.
  glFlush();
  return true;
}

bool WebFence::Poll()
{
  if(mState)
  {
    mState->Poll();
  }
  return GetStatus() == Status::Complete;
}

WebFence::Status WebFence::GetStatus() const
{
  return mState ? mState->status.load() : Status::Empty;
}

void WebFence::Release()
{
  if(mState)
  {
    mState->releaseRequested.store(true);
    if(mState->IsOwner())
    {
      mState->Delete();
      mState->status.store(Status::Cancelled);
    }
    mState.reset();
  }
}

void WebFence::Collect()
{
  auto& fences = State::fences;
  for(auto iter = fences.begin(); iter != fences.end();)
  {
    auto& state = *iter;
    if(state->releaseRequested.load())
    {
      state->Delete();
      state->status.store(Status::Cancelled);
    }
    else
    {
      state->Poll();
    }
    if(state->status.load() != Status::Pending)
    {
      iter = fences.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
}

bool WebFence::HasFailure()
{
  return State::failed;
}

void WebFence::Shutdown()
{
  for(auto& state : State::fences)
  {
    state->Delete();
    state->status.store(Status::Cancelled);
  }
  State::fences.clear();
}
} // namespace DALI_NAMESPACE::Graphics::GLES
