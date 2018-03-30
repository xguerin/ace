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

#include "Attribute.h"
#include "Errors.h"
#include <ace/common/Range.h>
#include <ace/tree/Primitive.h>
#include <list>
#include <sstream>
#include <string>
#include <vector>

namespace ace {
namespace model {

template<typename T, typename C, bool O = false>
class RangeAttribute : public Attribute {
 public:

  RangeAttribute() = delete;
  RangeAttribute(std::string const & n, bool o);

  // Object

  bool checkModel(tree::Value const & t) const;
  void loadModel(tree::Value const & t);

  // Instance

  bool checkInstance(tree::Object const & r, tree::Value const & v) const;
  bool resolveInstance(tree::Object const & r, tree::Value const & v) const;

  void load(Attribute const & a);
  bool check(T const & v) const;

  // Attribute

  bool merge(Attribute const & b);

  bool checkConstraint(std::string const & v) const;
  bool constrain(std::string const & v);

  operator tree::Checker::Pattern() const;
  operator std::string() const;

  Attribute::Ref clone() const;

 private:

  bool validate(tree::Object const & r, tree::Value const & v) const;

  common::Range<T, C> m_range;
};

template<typename T, typename C, bool O>
RangeAttribute<T, C, O>::RangeAttribute(std::string const & n, bool o)
  : Attribute(n, o, O), m_range() { }

template<typename T, typename C, bool O>
bool RangeAttribute<T, C, O>::checkModel(tree::Value const & t) const {
  if (t.type() != tree::Value::Type::String) {
    ERROR(ERR_RANGE_INVALID_TYPE);
    return false;
  }
  common::Range<T, C> r;
  auto const & pri = static_cast<tree::Primitive const &>(t);
  if (not common::Range<T, C>::parse(pri.value<std::string>(), r)) {
    ERROR(ERR_RANGE_FORMAT(pri.value<std::string>()));
    return false;
  }
  return true;
}

template<typename T, typename C, bool O>
void RangeAttribute<T, C, O>::loadModel(tree::Value const & t) {
  auto const & pri = static_cast<tree::Primitive const &>(t);
  common::Range<T, C>::parse(pri.value<std::string>(), m_range);
}

template<typename T, typename C, bool O>
bool
RangeAttribute<T, C, O>::checkInstance(tree::Object const & r, tree::Value const & v) const {
  return validate(r, v);
}

template<typename T, typename C, bool O>
bool
RangeAttribute<T, C, O>::resolveInstance(tree::Object const & r, tree::Value const & v) const {
  return validate(r, v);
}

template<typename T, typename C, bool O>
void
RangeAttribute<T, C, O>::load(Attribute const & a) {
  RangeAttribute<T, C, O> const & ra = static_cast<RangeAttribute<T, C, O> const &>(a);
  m_range = ra.m_range;
}

template<typename T, typename C, bool O>
bool
RangeAttribute<T, C, O>::validate(tree::Object const & r, tree::Value const & v) const {
  int score = 0;
  v.each([&](tree::Value const & w) {
    if (not w.isPrimitive()) {
      ERROR(ERR_RANGE_INSTANCE_NOT_A_PRIMITIVE);
      score += 1;
    }
    tree::Primitive const & p = static_cast<tree::Primitive const &>(w);
    if (not check(p.value<T>())) {
      ERROR(ERR_UNSUPPORTED_VALUE(p.value<T>()));
      score += 1;
    }
  });
  return score == 0;
}

template<typename T, typename C, bool O>
bool
RangeAttribute<T, C, O>::check(T const & v) const {
  return m_range.contains(v);
}

template<typename T, typename C, bool O>
bool
RangeAttribute<T, C, O>::merge(Attribute const & b) {
  if (not Attribute::merge(b)) return false;
  RangeAttribute<T, C, O> const & rb = static_cast<RangeAttribute<T, C, O> const &>(b);
  m_range = m_range.intersect(rb.m_range);
  return true;
}

template<typename T, typename C, bool O>
bool
RangeAttribute<T, C, O>::checkConstraint(std::string const & v) const {
  common::Range<T, C> range;
  if (not common::Range<T, C>::parse(v, range)) {
    ERROR(ERR_RANGE_FORMAT(v));
    return false;
  }
  return true;
}

template<typename T, typename C, bool O>
bool
RangeAttribute<T, C, O>::constrain(std::string const & v) {
  if (not common::Range<T, C>::parse(v, m_range)) {
    ERROR(ERR_RANGE_FORMAT(v));
    return false;
  }
  return true;
}

template<typename T, typename C, bool O>
RangeAttribute<T, C, O>::operator tree::Checker::Pattern() const {
  return tree::Checker::Pattern({ tree::Value::Type::String }, m_optional);
}

template<typename T, typename C, bool O>
RangeAttribute<T, C, O>::operator std::string() const {
  std::ostringstream oss;
  oss << m_range;
  return oss.str();
}

template<typename T, typename C, bool O>
Attribute::Ref RangeAttribute<T, C, O>::clone() const {
  return Attribute::Ref(new RangeAttribute<T, C, O>(*this));
}

} // namespace model
} // namespace ace
