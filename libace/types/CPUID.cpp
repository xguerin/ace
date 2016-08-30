/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <ace/types/CPUID.h>
#include <ace/tree/Primitive.h>
#include <string>
#include <thread>

namespace {

static bool checkFormat(const long id) {
  return id >= -1 and id < std::thread::hardware_concurrency();
}

} // namespace

namespace ace {
namespace model {

CPUIDFormatChecker::CPUIDFormatChecker(const BasicType * o) : FormatChecker(o) { }

bool
CPUIDFormatChecker::operator() (tree::Object const & r, tree::Value const & v) const {
  if (not FormatChecker<long>::operator()(r, v)) return false;
  int score = 0;
  v.each([&](tree::Value const & w) {
    tree::Primitive const & p = static_cast<tree::Primitive const &>(w);
    if (not checkFormat(p.value<long>())) {
      ERROR_O(m_owner, ERR_UNAVAILABLE_CPUID(p.value<long>()));
      score += 1;
    }
  });
  return score == 0;
}

CPUID::CPUID()
    : Type(BasicType::Kind::CPUID),
    RangedType(BasicType::Kind::CPUID),
    EnumeratedType(BasicType::Kind::CPUID) { }

bool
CPUID::validateModel() {
  if (not RangedType::validateModel()) return false;
  if (not EnumeratedType::validateModel()) return false;
  int score = 0;
  if (hasEitherAttribute()) for (auto & e : eitherAttribute().values()) {
    if (not checkFormat(e)) {
      ERROR(ERR_INVALID_EITHER_CPUID(e));
      score += 1;
    }
  }
  return score == 0;
}

bool
CPUID::checkInstance(tree::Object const & r, tree::Value const & v) const {
  return RangedType::checkInstance(r, v) and EnumeratedType::checkInstance(r, v);
}

BasicType::Ref
CPUID::clone(std::string const & n) const {
  CPUID * it = new CPUID(*this);
  it->setName(n);
  return BasicType::Ref(it);
}

} // namespace model
} // namespace ace

