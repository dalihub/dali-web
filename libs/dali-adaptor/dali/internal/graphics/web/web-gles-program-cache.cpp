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

#include <dali/internal/graphics/gles-impl/gles-program-binary-cache.h>
#include <dali/internal/graphics/web/web-gles-failure.h>
namespace DALI_NAMESPACE::Graphics::GLES::ProgramBinaryCache
{
bool IsSupported()
{
  return false;
}
bool LoadProgramBinary(const ProgramCreateInfo&, std::string_view, uint32_t, Integration::GlAbstraction*)
{
  WebFailure::Latch("program binary cache unsupported on WebGL2");
  return false;
}
void SaveProgramBinary(const ProgramCreateInfo&, std::string_view, uint32_t, Integration::GlAbstraction*)
{
  WebFailure::Latch("program binary cache unsupported on WebGL2");
}
}
