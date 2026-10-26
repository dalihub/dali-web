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
#include <dali/integration-api/gl-defines.h>
// EXTERNAL HEADERS
#include <dali/public-api/common/dali-utility.h>
#if defined(DALI_PROFILE_WINDOWS)
#include <process.h>
#else
#include <unistd.h>
#endif
#include <filesystem>
#include <fstream>
#include <iostream>
#include <locale>

// INTERNAL HEADERS
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <dali/internal/graphics/common/shader-parser.h>
#include <dali/internal/system/common/system-error-print.h>
#include <dali/public-api/dali-adaptor-version.h>


#include <dali/internal/graphics/gles-impl/gles-graphics-shader.h>

extern std::string GetSystemProgramBinaryPath();
extern std::string GetCustomProgramBinaryPath();
namespace DALI_NAMESPACE::Graphics::GLES::ProgramBinaryCache
{
namespace
{
const char* VERSION_SEPARATOR = "-";
const char* SHADER_SUFFIX = ".shader";
}
bool IsSupported()
{
  return true;
}
bool SaveFile(const std::string& filename, const unsigned char* buffer, unsigned int numBytes);
std::string GetProgramBinaryName(const ProgramCreateInfo& info, std::string_view name)
{
  // Check shader with dali-version, name and total shader size
  uint32_t totalShaderSize = 0u;
  for(const auto& state : *info.shaderState)
  {
    const auto* shader = static_cast<const GLES::Shader*>(state.shader);
    totalShaderSize += shader->GetCreateInfo().sourceSize;
  }

  std::string programBinaryName = std::to_string(ADAPTOR_MAJOR_VERSION) + VERSION_SEPARATOR + std::to_string(ADAPTOR_MINOR_VERSION) + VERSION_SEPARATOR + std::to_string(ADAPTOR_MICRO_VERSION) + VERSION_SEPARATOR + std::string(name) + VERSION_SEPARATOR + std::to_string(totalShaderSize) + SHADER_SUFFIX;
  return programBinaryName;
}

bool LoadProgramBinary(const ProgramCreateInfo& info, std::string_view name, uint32_t program, Integration::GlAbstraction* gl)
{
  std::string programBinaryName;
  if(info.internal)
  {
    programBinaryName = GetSystemProgramBinaryPath() + GetProgramBinaryName(info, name);
  }
  else
  {
    programBinaryName = GetCustomProgramBinaryPath() + GetProgramBinaryName(info, name);
  }

  bool result = false;
  Dali::Vector<char> buffer;
  result = Dali::FileLoader::ReadFile(programBinaryName, buffer);
  DALI_LOG_DEBUG_INFO("ReadFile result:%d, buffer.Size():%zu, Name[%s]\n\n", result, buffer.Size(), programBinaryName.c_str());

  if(result)
  {
    if(buffer.Size() == 0)
    {
      DALI_LOG_ERROR("Can't load binary shader from file [%s]\n", programBinaryName.c_str());
      return false;
    }

    if(DALI_UNLIKELY(!gl))
    {
      DALI_LOG_ERROR("Can't Get GL \n");
      return false;
    }

    // if formatsLength is less than 0 or greater than 2, it is difficult to guarantee accurate operation.
    // currently, we don't consider having multiple formats. so it is implemented to operate only when there is one format.
    GLint formatsLength = -1;
    gl->GetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formatsLength);
    if(formatsLength != 1)
    {
      DALI_LOG_ERROR("GL_NUM_PROGRAM_BINARY_FORMATS return invalid value : %d. Load failed \n", formatsLength);
      return false;
    }

    std::vector<GLint> formats(formatsLength);
    gl->GetIntegerv(GL_PROGRAM_BINARY_FORMATS, &formats[0]);
    if(!formats[0])
    {
      DALI_LOG_ERROR("GL_PROGRAM_BINARY_FORMATS is failed \n");
      return false;
    }

    DALI_LOG_DEBUG_INFO("Program binary format : %d", formats[0]);

    gl->ProgramBinary(program, formats[0], buffer.Begin(), static_cast<GLsizei>(buffer.Size()));

    GLint status{0};
    gl->GetProgramiv(program, GL_LINK_STATUS, &status);
    if(status != GL_TRUE)
    {
      char output[4096];
      GLsizei size{0u};
      gl->GetProgramInfoLog(program, 4096, &size, output);

      // log on error
      DALI_LOG_ERROR("glProgramBinary[%s] failed:\n%s. Need to re-compile shader\n", std::string(name).c_str(), output);
      return false;
    }
  }

  return result;
}

void SaveProgramBinary(const ProgramCreateInfo& info, std::string_view name, uint32_t program, Integration::GlAbstraction* gl)
{
  GLint binaryLength{0u};
  GLint binarySize{0u};
  GLenum format;
  if(DALI_UNLIKELY(!gl))
  {
    DALI_LOG_ERROR("Can't Get GL \n");
    return;
  }

  gl->GetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binaryLength);
  if(binaryLength == 0)
  {
    DALI_LOG_ERROR("GL_PROGRAM_BINARY_LENGTH is zero. maybe this device doesn't support glProgramBinary \n");
    return;
  }

  std::vector<uint8_t> programBinary(binaryLength);
  gl->GetProgramBinary(program, binaryLength, &binarySize, &format, programBinary.data());
  if(binarySize != binaryLength)
  {
    DALI_LOG_ERROR("Program binary created but size mismatch %d != %d\n", binarySize, binaryLength);
    return;
  }

  DALI_LOG_DEBUG_INFO("Program binary format : %d", format);

  std::string programBinaryName;
  if(info.internal)
  {
    programBinaryName = GetSystemProgramBinaryPath() + GetProgramBinaryName(info, name);
  }
  else
  {
    DALI_LOG_DEBUG_INFO("[Enable] Shader program binary, but this shader[%s] is not internal shader. so we save this shader user path %s \n", std::string(name).c_str(), GetCustomProgramBinaryPath().c_str());
    programBinaryName = GetCustomProgramBinaryPath() + GetProgramBinaryName(info, name);
  }

#if defined(DALI_PROFILE_WINDOWS)
  const auto processId = _getpid();
#else
  const auto processId = getpid();
#endif
  auto temporaryProgramBinaryName = programBinaryName + std::to_string(processId) + ".tmp";
  bool programBinarySaved = SaveFile(temporaryProgramBinaryName, (unsigned char*)programBinary.data(), binaryLength);
  if(!programBinarySaved)
  {
    DALI_LOG_ERROR("Program binary save failed!! file = %s \n", programBinaryName.c_str());
    return;
  }

  if(std::filesystem::exists(programBinaryName))
  {
    std::filesystem::remove(temporaryProgramBinaryName);
  }
  else
  {
    std::filesystem::rename(temporaryProgramBinaryName, programBinaryName);
  }

  DALI_LOG_DEBUG_INFO("ProgramBinary is saved [success:%d] file = %s buffer size = %d \n", programBinarySaved, programBinaryName.c_str(), binaryLength);
}

bool SaveFile(const std::string& filename, const unsigned char* buffer, unsigned int numBytes)
{
  DALI_ASSERT_DEBUG(0 != filename.length());

  bool result = false;

  std::filebuf buf;
  buf.open(filename.c_str(), std::ios::out | std::ios_base::trunc | std::ios::binary);
  if(buf.is_open())
  {
    std::ostream stream(&buf);
    stream.imbue(std::locale::classic());

    // determine size of buffer
    int length = static_cast<int>(numBytes);

    // write contents of buffer to the file
    stream.write(reinterpret_cast<const char*>(buffer), length);

    if(!stream.bad())
    {
      result = true;
    }
    else
    {
      DALI_LOG_ERROR("std::ostream.write failed!\n");
    }
  }
  else
  {
    DALI_LOG_ERROR("std::filebuf.open failed!\n");
  }

  if(!result)
  {
    DALI_LOG_ERROR("Can't write to %s. buffer pointer : %p, length : %u\n", filename.c_str(), buffer, numBytes);
    DALI_PRINT_SYSTEM_ERROR_LOG();
  }

  return result;
}

}
