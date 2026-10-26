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

#include <dali/internal/graphics/web/web-gles-failure.h>
#include <dali/internal/graphics/web/web-gles-fence.h>
#include <dali/integration-api/debug.h>
#include <emscripten/html5_webgl.h>
#include <GLES3/gl3.h>

namespace DALI_NAMESPACE::Graphics::GLES
{
namespace
{
thread_local bool stopped = false;
thread_local const char* failure = nullptr;
thread_local EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = 0;
}
void WebFailure::Stop()
{
  stopped = true;
}
bool WebFailure::IsStopped()
{
  return stopped;
}
void WebFailure::Latch(const char* reason)
{
  if(!failure)
  {
    failure = reason;
    DALI_LOG_ERROR("Web GLES stopped: %s\n", reason);
  }
}
bool WebFailure::HasFailure()
{
  if(WebFence::HasFailure())
  {
    Latch("fence failure");
  }
  return failure != nullptr;
}
const char* WebFailure::Reason()
{
  HasFailure();
  return failure;
}
bool WebFailure::CheckContext()
{
  if(stopped || HasFailure())
  {
    return false;
  }
  const auto current = emscripten_webgl_get_current_context();
  if(current <= 0 || (context && current != context) || emscripten_is_webgl_context_lost(current))
  {
    Latch("context missing, replaced or lost");
    return false;
  }
  context = current;
  return true;
}
bool WebFailure::CheckGl()
{
  if(!CheckContext())
  {
    return false;
  }
  if(glGetError() != GL_NO_ERROR)
  {
    Latch("GL operation failed");
    return false;
  }
  return true;
}
}
