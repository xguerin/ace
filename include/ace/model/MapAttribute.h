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
#include "FormatChecker.h"
#include "Generator.h"
#include <ace/tree/Primitive.h>
#include <iomanip>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>

namespace ace {
namespace model {

template<typename T, bool O = false>
class MapAttribute : public Attribute {
 public:

  MapAttribute() = delete;
  MapAttribute(std::string const & n, bool o);

  // Object

  bool checkModel(tree::Value const & t) const;
  void loadModel(tree::Value const & t);

  // Instance

  bool checkInstance(tree::Object const & r, tree::Value const & v) const;
  bool resolveInstance(tree::Object const & r, tree::Value const & v) const;

  void load(Attribute const & a);
  bool check(std::string const & v) const;

  // Attribute

  bool merge(Attribute const & b);

  operator tree::Checker::Pattern() const;

  void print(std::ostream & o, int l) const;
  operator std::string() const;

  Attribute::Ref clone() const;

  // Accessors

  std::map<std::string, T> const & values() const;

 private:

  bool validate(tree::Object const & r, tree::Value const & v) const;

  std::map<std::string, T>  m_values;
};

template<typename T, bool O>
MapAttribute<T, O>::MapAttribute(std::string const & n, bool o)
  : Attribute(n, o, O), m_values() { }

template<typename T, bool O>
bool MapAttribute<T, O>::checkModel(tree::Value const & t) const {
  if (t.type() != tree::Value::Type::Object) {
    ERROR(ERR_MAP_NOT_AN_OBJECT);
    return false;
  }
  for (auto & a : static_cast<tree::Object const &>(t)) {
    if (not a.second->isPrimitive()) {
      ERROR(ERR_MAP_ITEM_TYPE_MISMATCH);
      return false;
    }
    if (not static_cast<tree::Primitive const &>(*a.second).is<T>()) {
      ERROR(ERR_MAP_ITEM_TYPE_MISMATCH);
      return false;
    }
  }
  return true;
}

template<typename T, bool O>
void MapAttribute<T, O>::loadModel(tree::Value const & t) {
  for (auto & m : static_cast<tree::Object const &>(t)) {
    m_values[m.first] = static_cast<tree::Primitive const &>(*m.second).value<T>();
  }
}

template<typename T, bool O>
bool
MapAttribute<T, O>::checkInstance(tree::Object const & r, tree::Value const & v) const {
  return validate(r, v);
}

template<typename T, bool O>
bool
MapAttribute<T, O>::resolveInstance(tree::Object const & r, tree::Value const & v) const {
  return validate(r, v);
}

template<typename T, bool O>
void
MapAttribute<T, O>::load(Attribute const & a) {
  MapAttribute<T, O> const & ra = static_cast<MapAttribute<T, O> const &>(a);
  m_values = ra.m_values;
}

template<typename T, bool O>
bool MapAttribute<T, O>::validate(tree::Object const & r, tree::Value const & v) const {
  int score = 0;
  v.each([&](tree::Value const & w) {
    if (w.type() != tree::Value::Type::Object) {
      ERROR(ERR_MAP_INSTANCE_NOT_AN_OBJECT);
      score += 1;
    } else {
      tree::Object const & p = static_cast<tree::Object const &>(w);
      for (auto & e : p) if (not check(e.first)) {
        ERROR(ERR_UNSUPPORTED_VALUE(e.first));
        score += 1;
      }
    }
  });
  return score == 0;
}

template<typename T, bool O>
bool MapAttribute<T, O>::check(std::string const & v) const {
  return m_values.count(v) != 0;
}

template<typename T, bool O>
bool MapAttribute<T, O>::merge(Attribute const & b) {
  if (not Attribute::merge(b)) return false;
  MapAttribute<T, O> const & rb = static_cast<MapAttribute<T, O> const &>(b);
  for (auto & v : rb.m_values) if (m_values.find(v.first) == m_values.end()) {
    m_values[v.first] = v.second;
  } else if (m_values[v.first] != v.second) {
    ERROR(ERR_CONFLICTING_MAP_KEY(v.first));
    return false;
  }
  return true;
}

template<typename T, bool O>
MapAttribute<T, O>::operator tree::Checker::Pattern() const {
  return tree::Checker::Pattern(tree::Value::Type::Object, m_optional);
}

template<typename T, bool O>
void MapAttribute<T, O>::print(std::ostream & o, int l) const {
  size_t cnt = 0, w = 0;
  for (auto & v : m_values) if (v.first.length() > w) w = v.first.length();
  o << "{" << std::endl;
  for (auto & v : m_values) {
    cnt += 1;
    Generator::indent(o, l + 2) << std::setw(w) << v.first << " : " << v.second;
    if (cnt < m_values.size()) o << ", ";
    o << std::endl;
  }
  Generator::indent(o, l) << "}" << std::endl;
}

template<typename T, bool O>
MapAttribute<T, O>::operator std::string() const {
  size_t cnt = 0;
  std::ostringstream oss;
  oss << "{ ";
  for (auto & v : m_values) {
    cnt += 1;
    oss << v.first;
    if (cnt < m_values.size()) oss << ", ";
  }
  oss << " }";
  return oss.str();
}

template<typename T, bool O>
Attribute::Ref MapAttribute<T, O>::clone() const {
  return Attribute::Ref(new MapAttribute<T, O>(*this));
}

template<typename T, bool O>
std::map<std::string, T> const & MapAttribute<T, O>::values() const {
  return m_values;
}

} // namespace model
} // namespace ace
