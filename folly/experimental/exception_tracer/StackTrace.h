/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

#include <folly/Portability.h>

namespace folly {
namespace exception_tracer {

constexpr size_t kMaxFrames = 500;

struct StackTrace {
  StackTrace() : frameCount(0) {}

  size_t frameCount;
  uintptr_t addresses[kMaxFrames];
};

// note: no constructor so this can be __thread.
// A StackTraceStack MUST be placed in zero-initialized memory.
class StackTraceStack {
  class Node;

 public:
  /**
   * Push the current stack trace onto the stack.
   * Returns false on failure (not enough memory, getting stack trace failed),
   * true on success.
   */
  bool pushCurrent();

  /**
   * Pop the top stack trace from the stack.
   * Returns true on success, false on failure (stack was empty).
   */
  bool pop();

  /**
   * Move the top stack trace from other onto this.
   * Returns true on success, false on failure (other was empty).
   */
  bool moveTopFrom(StackTraceStack& other);

  /**
   * Clear the stack.
   */

  void clear();

  /**
   * Is the stack empty?
   */
  bool empty() const {
    return !state_[kTopIdx];
  }

  /**
   * Return the top stack trace, or nullptr if the stack is empty.
   */
  StackTrace* top();
  const StackTrace* top() const;

  /**
   * Return the stack trace following p, or nullptr if p is the bottom of
   * the stack.
   */
  StackTrace* next(StackTrace* p);
  const StackTrace* next(const StackTrace* p) const;

 private:
  static constexpr size_t kTopIdx = kIsDebug ? 1 : 0;

  // In debug mode, we assert that we're in zero-initialized memory by
  // checking that the two guards around the Node* from top() are zero.
  void checkGuard() const {
    assert(state_[0] == 0 && state_[2] == 0);
  }

  Node* state_[kIsDebug ? 3 : 1];
};
} // namespace exception_tracer
} // namespace folly
