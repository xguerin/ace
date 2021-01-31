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

#include "Dependency.h"
#include "Errors.h"
#include <ace/common/Algorithms.h>
#include <ace/common/String.h>
#include <ace/tree/Array.h>
#include <ace/tree/Object.h>
#include <ace/tree/Primitive.h>
#include <ace/tree/Value.h>
#include <iostream>
#include <set>
#include <string>

namespace ace { namespace model {

template<typename T, typename D>
class ValueGuard : public D
{
public:
  ValueGuard();

  bool checkModel(tree::Value const& t) const;
  void loadModel(tree::Value const& t);

  bool checkInstance(tree::Object const& r, tree::Value const& v) const;
  void expandInstance(tree::Object& r, tree::Value& v);
  bool flattenInstance(tree::Object& r, tree::Value& v);
  bool resolveInstance(tree::Object const& r, tree::Value const& v) const;

  typename D::Ref clone() const;

  bool canMerge(Dependency const& o) const;
  void merge(Dependency const& o);

  operator std::string() const;

private:
  std::set<T> collectValues(tree::Value const& v) const;
  void expandValues(std::set<T> const& v);
  bool filterValues(std::set<T> const& v) const;

  std::set<T> m_filters;
};

template<typename T, typename D>
ValueGuard<T, D>::ValueGuard() : D(), m_filters()
{
  this->m_schm["when"] = { tree::Value::Type::Array, true };
}

template<typename T, typename D>
bool
ValueGuard<T, D>::checkModel(tree::Value const& t) const
{
  if (not D::checkModel(t)) {
    return false;
  }
  if (t.has("when")) {
    for (auto& e : static_cast<tree::Array const&>(t["when"])) {
      if (not e->isPrimitive()) {
        ERROR(ERR_WRONG_WHEN_FORMAT);
        return false;
      }
      if (not static_cast<tree::Primitive const&>(*e).is<T>()) {
        ERROR(ERR_WHEN_TYPE_MISMATCH);
        return false;
      }
    }
  }
  return true;
}

template<typename T, typename D>
void
ValueGuard<T, D>::loadModel(tree::Value const& t)
{
  D::loadModel(t);
  if (t.has("when")) {
    for (auto& e : static_cast<tree::Array const&>(t["when"])) {
      m_filters.insert(static_cast<tree::Primitive const&>(*e).value<T>());
    }
  }
}

template<typename T, typename D>
bool
ValueGuard<T, D>::checkInstance(tree::Object const& r,
                                tree::Value const& v) const
{
  std::set<T> values = collectValues(v);
  if (filterValues(values)) {
    return D::checkInstance(r, v);
  }
  return true;
}

template<typename T, typename D>
void
ValueGuard<T, D>::expandInstance(tree::Object& r, tree::Value& v)
{
  std::set<T> values = collectValues(v);
  expandValues(values);
  if (filterValues(values)) {
    D::expandInstance(r, v);
  }
}

template<typename T, typename D>
bool
ValueGuard<T, D>::flattenInstance(tree::Object& r, tree::Value& v)
{
  std::set<T> values = collectValues(v);
  if (filterValues(values)) {
    return D::flattenInstance(r, v);
  }
  return true;
}

template<typename T, typename D>
bool
ValueGuard<T, D>::resolveInstance(tree::Object const& r,
                                  tree::Value const& v) const
{
  std::set<T> values = collectValues(v);
  if (filterValues(values)) {
    return D::resolveInstance(r, v);
  }
  return true;
}

template<typename T, typename D>
typename D::Ref
ValueGuard<T, D>::clone() const
{
  return typename D::Ref(new ValueGuard<T, D>(*this));
}

template<typename T, typename D>
bool
ValueGuard<T, D>::canMerge(Dependency const& o) const
{
  if (not D::canMerge(o)) {
    return false;
  }
  auto const& vdo = dynamic_cast<ValueGuard<T, D> const&>(o);
  return is_permutation(m_filters, vdo.m_filters);
}

template<typename T, typename D>
void
ValueGuard<T, D>::merge(Dependency const& o)
{
  D::merge(o);
  auto const& vdo = dynamic_cast<ValueGuard<T, D> const&>(o);
  for (auto& e : vdo.m_filters) {
    m_filters.insert(e);
  }
}

template<typename T, typename D>
ValueGuard<T, D>::operator std::string() const
{
  size_t cnt = 0;
  std::ostringstream oss;
  if (not m_filters.empty()) {
    oss << "when [ ";
    for (auto& v : m_filters) {
      cnt += 1;
      oss << std::boolalpha << v;
      if (cnt < m_filters.size()) {
        oss << ", ";
      }
    }
    oss << " ] -> " << (std::string)D::operator std::string();
  } else {
    oss << D::operator std::string();
  }
  return oss.str();
}

template<typename T, typename D>
std::set<T>
ValueGuard<T, D>::collectValues(tree::Value const& v) const
{
  std::set<T> result;
  switch (v.type()) {
    case tree::Value::Type::Boolean:
    case tree::Value::Type::Float:
    case tree::Value::Type::Integer:
    case tree::Value::Type::String: {
      tree::Primitive const& d = static_cast<tree::Primitive const&>(v);
      result.insert(d.value<T>());
      break;
    }
    case tree::Value::Type::Array: {
      tree::Array const& a = static_cast<tree::Array const&>(v);
      for (auto& d : a) {
        tree::Primitive const& p = static_cast<tree::Primitive const&>(*d);
        result.insert(p.value<T>());
      }
      break;
    }
    default:
      break;
  }
  return result;
}

template<typename T, typename D>
void
ValueGuard<T, D>::expandValues(std::set<T> const& v)
{
  std::set<std::string> templ;
  for (auto& d : this->m_deps) {
    if (this->hasPlaceHolder(d)) {
      templ.insert(d);
    }
  }
  for (auto& t : templ) {
    this->m_deps.erase(t);
    for (auto& e : v) {
      std::ostringstream oss;
      oss << e;
      this->m_deps.insert(this->expandPlaceHolder(t, oss.str()));
    }
  }
}

template<typename T, typename D>
bool
ValueGuard<T, D>::filterValues(std::set<T> const& v) const
{
  if (m_filters.empty()) {
    return true;
  }
  for (auto& e : v) {
    if (m_filters.find(e) != m_filters.end()) {
      return true;
    }
  }
  return false;
}

}}
