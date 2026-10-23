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

#include <dali/public-api/common/dali-namespace.h>
#include <memory>

namespace DALI_NAMESPACE::Graphics::GLES
{
// Only the creating GL owner polls/deletes. Other threads can read published
// completion and request deletion; collection runs on the render owner.
class WebFence
{
public:
  enum class Status
  {
    Empty,
    Pending,
    Complete,
    Failed,
    Cancelled
  };
  WebFence();
  ~WebFence();
  WebFence(const WebFence&) = delete;
  WebFence& operator=(const WebFence&) = delete;
  bool Insert();
  bool Poll();
  Status GetStatus() const;
  void Release();
  static void Collect();
  static void Shutdown();
  static bool HasFailure();

private:
  struct State;
  std::shared_ptr<State> mState;
};
} // namespace DALI_NAMESPACE::Graphics::GLES
