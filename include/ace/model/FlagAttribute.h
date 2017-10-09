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
#include <ace/tree/Array.h>
#include <ace/tree/Primitive.h>
#include <functional>
#include <set>
#include <sstream>
#include <string>

namespace ace {
namespace model {

template<typename T, bool O = false, typename C = std::equal_to<T>>
class FlagAttribute : public Attribute {
 public:

  FlagAttribute() = delete;
  FlagAttribute(std::string const & name, bool o);

  bool checkModel(tree::Value const & t) const;
  void loadModel(tree::Value const & t);
  bool flattenModel();

  void load(Attribute const & a);
  bool validate(tree::Object const & r, tree::Value const & v) const;
  bool check(T const & v) const;

  bool merge(Attribute const & b);

  operator tree::Checker::Pattern() const;
  operator std::string() const;

  Attribute::Ref clone() const;

  T const & head() const;
  std::set<T> const & values() const;

  void setMultiple(const bool m);
  tree::Value::Ref materialize() const;

 private:

  std::set<T> m_values;
  C           m_comparator;
  bool        m_multiple;
};

template<typename T, bool O, typename C>
FlagAttribute<T, O, C>::FlagAttribute(std::string const & n, bool o)
  : Attribute(n, o, O), m_values(), m_comparator(), m_multiple(false) { }

template<typename T, bool O, typename C>
bool
FlagAttribute<T, O, C>::checkModel(tree::Value const & t) const {
  size_t score = 0;
  t.each([&](tree::Value const & v) {
    if (v.type() != tree::Value::typeOf<T>()) {
      ERROR(ERR_INVALID_VALUE(name()));
      score += 1;
    } else if (not static_cast<tree::Primitive const &>(v).is<T>()) {
      ERROR(ERR_INVALID_VALUE(name()));
      score += 1;
    }
  });
  return score == 0;
}

template<typename T, bool O, typename C>
void
FlagAttribute<T, O, C>::loadModel(tree::Value const & t) {
  t.each([&](tree::Value const & v) {
    m_values.insert(static_cast<tree::Primitive const &>(v).value<T>());
  });
}

template<typename T, bool O, typename C>
bool
FlagAttribute<T, O, C>::flattenModel() {
  if (not m_multiple and m_values.size() > 1) {
    ERROR(ERR_MULTIPLE_VALUES_NOT_ALLOWED(name()));
    return false;
  }
  return true;
}

template<typename T, bool O, typename C>
void
FlagAttribute<T, O, C>::load(Attribute const & a) {
  FlagAttribute<T, O, C> const & ra = static_cast<FlagAttribute<T, O, C> const &>(a);
  m_values = ra.m_values;
}

template<typename T, bool O, typename C>
bool
FlagAttribute<T, O, C>::validate(tree::Object const & r, tree::Value const & v) const {
  return true;
}

template<typename T, bool O, typename C>
bool FlagAttribute<T, O, C>::check(T const & v) const {
  return true;
}

template<typename T, bool O, typename C>
bool
FlagAttribute<T, O, C>::merge(Attribute const & b) {
  if (not Attribute::merge(b)) return false;
  FlagAttribute<T, O, C> const & rb = static_cast<FlagAttribute<T, O, C> const &>(b);
  if (m_values.size() != rb.m_values.size()) return false;
  size_t score = 0;
  auto i = m_values.begin();
  auto j = rb.m_values.begin();
  for (; i != m_values.end(); i++, j++) {
    if (not m_comparator(*i, *j)) score += 1;
  }
  return score == 0;
}

template<typename T, bool O, typename C>
FlagAttribute<T, O, C>::operator std::string() const {
  std::ostringstream oss;
  if (m_values.size() > 1) oss << "[ ";
  size_t count = 0;
  for (auto const & v : m_values) {
    oss << v;
    if (++count < m_values.size()) oss << ", ";
  }
  if (m_values.size() > 1) oss << "]";
  return oss.str();
}

template<typename T, bool O, typename C>
FlagAttribute<T, O, C>::operator tree::Checker::Pattern() const {
  return tree::Checker::Pattern({ tree::Value::typeOf<T>(), tree::Value::Type::Array }, m_optional);
}

template<typename T, bool O, typename C>
Attribute::Ref
FlagAttribute<T, O, C>::clone() const {
  return Attribute::Ref(new FlagAttribute<T, O, C>(*this));
}

template<typename T, bool O, typename C>
T const &
FlagAttribute<T, O, C>::head() const {
  return *m_values.begin();
}

template<typename T, bool O, typename C>
std::set<T> const &
FlagAttribute<T, O, C>::values() const {
  return m_values;
}

template<typename T, bool O, typename C>
void
FlagAttribute<T, O, C>::setMultiple(const bool m) {
  m_multiple = m;
}

template<typename T, bool O, typename C>
tree::Value::Ref
FlagAttribute<T, O, C>::materialize() const {
  if (m_values.size() > 1) {
    auto ary = tree::Array::build();
    for (auto const & v : m_values) {
      ary->push_back(tree::Primitive::build(v));
    }
    return ary;
  } else {
    return tree::Primitive::build(*m_values.begin());
  }
}

} // namespace model
} // namespace ace
