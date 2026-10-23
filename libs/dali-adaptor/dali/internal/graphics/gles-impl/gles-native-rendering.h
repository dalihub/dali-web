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

#pragma once

#include <memory>
#include <dali/public-api/common/dali-namespace.h>

namespace DALI_NAMESPACE::Graphics
{
class GlesGraphicsController;
namespace GLES
{
// Build-selected native callback context handling. No EGL types cross this boundary.
class NativeRendering
{
public:
  virtual ~NativeRendering() = default;
  virtual void Prepare() = 0;
  virtual void Restore() = 0;
};

std::unique_ptr<NativeRendering> CreateNativeRendering(GlesGraphicsController& controller);
} // namespace GLES
} // namespace DALI_NAMESPACE::Graphics
