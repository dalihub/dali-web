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

#include <dali/internal/graphics/gles-impl/gles-native-rendering.h>
#include <dali/public-api/common/dali-common.h>

namespace DALI_NAMESPACE::Graphics::GLES
{
namespace
{
class UnsupportedNativeRendering : public NativeRendering
{
public:
  void Prepare() override
  {
    DALI_ASSERT_ALWAYS(false && "Native EGL rendering callbacks are unsupported on Web");
  }

  void Restore() override
  {
    DALI_ASSERT_ALWAYS(false && "Native EGL rendering callbacks are unsupported on Web");
  }
};
} // namespace

std::unique_ptr<NativeRendering> CreateNativeRendering(GlesGraphicsController&)
{
  return std::make_unique<UnsupportedNativeRendering>();
}
} // namespace DALI_NAMESPACE::Graphics::GLES
