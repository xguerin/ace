/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "BasicType.h"
#include "Generator.h"
#include <ace/tree/Array.h>
#include <ace/tree/Primitive.h>
#include <ace/tree/Value.h>
#include <string>
#include <type_traits>

namespace ace {
namespace model {

// Generic format checker

template<typename T>
class FormatChecker {
 public:

  FormatChecker() = delete;
  explicit FormatChecker(const BasicType * o);
  explicit FormatChecker(FormatChecker<T> const &) = default;
  virtual ~FormatChecker() { }

  virtual bool operator()(tree::Object const & r, tree::Value const & v) const;

 protected:

  const BasicType * m_owner;
};

template<typename T>
FormatChecker<T>::FormatChecker(const BasicType * o) : m_owner(o) { }

template<typename T>
bool
FormatChecker<T>::operator()(tree::Object const & r, tree::Value const & v) const {
  int score = 0;
  v.each([&](tree::Value const & w) {
    if (not w.isPrimitive()) {
      ERROR_O(m_owner, ERR_VALUE_NOT_PRIMITIVE);
      score += 1;
    } else {
      tree::Primitive const & p = static_cast<tree::Primitive const &>(w);
      if (p.value().empty()) {
        ERROR_O(m_owner, ERR_EMPTY_VALUE);
        score += 1;
      } else if (not p.is<T>()) {
        ERROR_O(m_owner, ERR_WRONG_VALUE_TYPE(Generator::nameFor<T>()));
        score += 1;
      }
    }
  });
  return score == 0;
}

// Specialized format checker

template<>
class FormatChecker<void> {
 public:

  FormatChecker() = delete;
  explicit FormatChecker(const BasicType * o);
  explicit FormatChecker(FormatChecker<void> const &) = default;
  virtual ~FormatChecker() { }

  virtual bool operator()(tree::Object const & r, tree::Value const & v) const;

 protected:

  const BasicType * m_owner;
};

} // namespace model
} // namespace ace
