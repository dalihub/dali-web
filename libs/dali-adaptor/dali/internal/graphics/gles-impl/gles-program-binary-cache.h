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
#include <dali/graphics-api/graphics-program-create-info.h>
#include <dali/integration-api/gl-abstraction.h>
namespace DALI_NAMESPACE::Graphics::GLES::ProgramBinaryCache
{
bool IsSupported();
bool LoadProgramBinary(const ProgramCreateInfo& info, std::string_view name, uint32_t program, Integration::GlAbstraction* gl);
void SaveProgramBinary(const ProgramCreateInfo& info, std::string_view name, uint32_t program, Integration::GlAbstraction* gl);
}
