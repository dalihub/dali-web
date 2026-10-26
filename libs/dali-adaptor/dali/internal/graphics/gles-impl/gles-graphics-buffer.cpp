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
#include "gles-graphics-buffer.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/gl-defines.h>

// INTERNAL INCLUDES
#include "gles-graphics-controller.h"

namespace DALI_NAMESPACE::Graphics::GLES
{
Buffer::Buffer(const Graphics::BufferCreateInfo& createInfo, Graphics::GlesGraphicsController& controller)
: BufferResource(createInfo, controller),
  mCpuAllocated(false),
  mTransient(false)
{
  // Check if buffer is CPU allocated
  if(((0 | BufferUsage::UNIFORM_BUFFER) & mCreateInfo.usage) &&
     (0 | BufferPropertiesFlagBit::CPU_ALLOCATED) & mCreateInfo.propertiesFlags)
  {
    // cpu allocated buffer
    mCpuAllocated = true;
  }

  // Check if buffer is transient
  if((0 | BufferPropertiesFlagBit::TRANSIENT_MEMORY) & mCreateInfo.propertiesFlags)
  {
    // memory is transient, may be lazily allocated when
    // mapped, together with cpu allocated it may create emulated uniform
    // buffer in this implementation
    mTransient = true;
  }

  // Check the target enum for this buffer
  if((0 | BufferUsage::UNIFORM_BUFFER) & mCreateInfo.usage)
  {
    mBufferTarget = GL_UNIFORM_BUFFER;
  }
  else if((0 | BufferUsage::INDEX_BUFFER) & mCreateInfo.usage)
  {
    mBufferTarget = GL_ELEMENT_ARRAY_BUFFER;
  }
  // else if((0 | BufferUsage::VERTEX_BUFFER) & mCreateInfo.usage) default enum : GL_ARRAY_BUFFER

  controller.AddBuffer(*this);
}

void Buffer::DiscardResource()
{
  mController.DiscardResource(this);
}

void Buffer::Bind(Graphics::BufferUsage bindingTarget) const
{
  auto* context = mController.GetCurrentContext();
  auto* gl      = mController.GetGL();
  if(DALI_UNLIKELY(!gl || !context))
  {
    return;
  }

  // CPU allocated buffer may be bound only as Uniform Buffer
  // on special binding point
  if(mCpuAllocated && mBufferPtr)
  {
    if(bindingTarget == Graphics::BufferUsage::UNIFORM_BUFFER)
    {
      // TODO: probably nothing to do, the GLES Context
      //       we may use CPU backed buffer for future data
      //       transfers (copy operations)
    }
  }
  else
  {
    switch(bindingTarget)
    {
      case Graphics::BufferUsage::VERTEX_BUFFER:
      {
        context->BindBuffer(GL_ARRAY_BUFFER, mBufferId);
        break;
      }
      case Graphics::BufferUsage::INDEX_BUFFER:
      {
        context->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferId);
        break;
      }
      default:
      {
        // Nothing to do
      }
    }
  }
}

} //namespace DALI_NAMESPACE::Graphics::GLES
