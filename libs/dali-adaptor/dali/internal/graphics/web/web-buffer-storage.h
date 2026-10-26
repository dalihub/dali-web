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
#include <dali/graphics-api/graphics-types.h>

namespace DALI_NAMESPACE::Graphics::GLES
{
// Shared only to keep a map's address valid through buffer destruction.
// Access and GL operations remain on the render owner.
class WebBufferStorage
{
public:
  WebBufferStorage(uint32_t size, bool cpu, const AllocationCallbacks* allocator);
  ~WebBufferStorage();
  void Reference() { ++mReferences; }
  void Unreference();
  bool Allocate();
  bool Upload(uint32_t offset, uint32_t size);
  void Detach();
  bool IsAlive() const;
  unsigned char* Data() const { return mData; }
  uint32_t Size() const { return mSize; }
  void SetBufferId(uint32_t id) { mBufferId = id; }
  uint32_t Revision() const { return mRevision; }
  bool mapped{false};

private:
  uint32_t mReferences{1};
  bool mAlive{true};
  uint32_t mBufferId{0};
  uint32_t mRevision{0};
  uint32_t mSize;
  bool mCpu;
  AllocationCallbacks mAllocator{};
  unsigned char* mData{nullptr};
};
}
