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

#pragma once

#include <dali/internal/graphics/gles-impl/gles-graphics-controller.h>
#include <new>

namespace DALI_NAMESPACE::Graphics
{
template<class GLESType, template<class> class Deleter, class GfxCreateInfo, class T>
auto NewObject(const GfxCreateInfo& info, GlesGraphicsController& controller, T&& oldObject)
{
  // Use allocator
  using Type = typename T::element_type;
  using UPtr = Graphics::UniquePtr<Type>;
  if(info.allocationCallbacks)
  {
    if(!info.allocationCallbacks->allocCallback || !info.allocationCallbacks->freeCallback)
    {
      controller.ResourceCreationFailed();
      return UPtr(nullptr);
    }
    auto* memory = info.allocationCallbacks->allocCallback(
      sizeof(GLESType),
      0,
      info.allocationCallbacks->userData);
    if(!memory)
    {
      controller.ResourceCreationFailed();
      return UPtr(nullptr);
    }
    return UPtr(new(memory) GLESType(info, controller), Deleter<GLESType>());
  }
  else // Use standard allocator
  {
    // We are given all object for recycling
    if(oldObject)
    {
      auto reusedObject = oldObject.release();
      // If succeeded, attach the object to the unique_ptr and return it back
      if(static_cast<GLESType*>(reusedObject)->TryRecycle(info, controller))
      {
        return UPtr(reusedObject, Deleter<GLESType>());
      }
      else
      {
        // can't reuse so kill object by giving it back to original
        // unique pointer.
        oldObject.reset(reusedObject);
      }
    }

    // Create brand new object
    auto* object = new(std::nothrow) GLESType(info, controller);
    if(!object)
    {
      controller.ResourceCreationFailed();
    }
    return UPtr(object, Deleter<GLESType>());
  }
}

} // namespace DALI_NAMESPACE::Graphics
