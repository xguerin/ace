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
#include "Generator.h"
#include <ace/tree/Array.h>
#include <ace/tree/Primitive.h>
#include <list>
#include <sstream>
#include <string>

namespace ace { namespace model {

template<typename T, typename C, bool O = false>
class ListAttribute : public Attribute
{
public:
  ListAttribute() = delete;
  ListAttribute(std::string const& name, bool o);

  // Object

  void loadModel(tree::Value const& t);
  bool checkModel(tree::Value const& t) const;

  // Instance

  bool checkInstance(tree::Object const& r, tree::Value const& v) const;
  bool resolveInstance(tree::Object const& r, tree::Value const& v) const;

  void load(Attribute const& a);
  bool check(T const& v) const;

  // Attribute

  bool merge(Attribute const& b);

  bool checkConstraint(std::list<tree::Value::Ref> const& v) const;
  bool constrain(std::list<tree::Value::Ref> const& v);

  operator tree::Checker::Pattern() const;

  void print(std::ostream& o, int l) const;
  operator std::string() const;

  Attribute::Ref clone() const;

  // Accessors

  std::list<T> const& values() const;

private:
  bool validate(tree::Object const& r, tree::Value const& v) const;

  std::list<T> m_values;
  C m_comparator;
};

template<typename T, typename C, bool O>
ListAttribute<T, C, O>::ListAttribute(std::string const& n, bool o)
  : Attribute(n, o, O), m_values()
{}

template<typename T, typename C, bool O>
bool
ListAttribute<T, C, O>::checkModel(tree::Value const& t) const
{
  if (t.type() != tree::Value::Type::Array) {
    ERROR(ERR_LIST_NOT_AN_ARRAY);
    return false;
  }
  for (auto& a : static_cast<tree::Array const&>(t)) {
    if (not a->isPrimitive()) {
      ERROR(ERR_LIST_ITEM_TYPE_MISMATCH);
      return false;
    }
    if (not static_cast<tree::Primitive const&>(*a).is<T>()) {
      ERROR(ERR_LIST_ITEM_TYPE_MISMATCH);
      return false;
    }
  }
  return true;
}

template<typename T, typename C, bool O>
void
ListAttribute<T, C, O>::loadModel(tree::Value const& t)
{
  for (auto& m : static_cast<tree::Array const&>(t)) {
    m_values.push_back(static_cast<tree::Primitive const&>(*m).value<T>());
  }
}

template<typename T, typename C, bool O>
bool
ListAttribute<T, C, O>::checkInstance(tree::Object const& r,
                                      tree::Value const& v) const
{
  return validate(r, v);
}

template<typename T, typename C, bool O>
bool
ListAttribute<T, C, O>::resolveInstance(tree::Object const& r,
                                        tree::Value const& v) const
{
  return validate(r, v);
}

template<typename T, typename C, bool O>
void
ListAttribute<T, C, O>::load(Attribute const& a)
{
  ListAttribute<T, C, O> const& ra =
    static_cast<ListAttribute<T, C, O> const&>(a);
  m_values = ra.m_values;
}

template<typename T, typename C, bool O>
bool
ListAttribute<T, C, O>::validate(tree::Object const& r,
                                 tree::Value const& v) const
{
  int score = 0;
  v.each([&](tree::Value const& w) {
    if (w.type() == tree::Value::Type::Object) {
      tree::Object const& o = static_cast<tree::Object const&>(w);
      for (auto& e : o) {
        if (not check(common::String::value<T>(e.first))) {
          ERROR(ERR_UNSUPPORTED_VALUE(e.first));
          score += 1;
        }
      }
    } else {
      tree::Primitive const& p = static_cast<tree::Primitive const&>(w);
      if (not check(p.value<T>())) {
        ERROR(ERR_UNSUPPORTED_VALUE(p.value<T>()));
        score += 1;
      }
    }
  });
  return score == 0;
}

template<typename T, typename C, bool O>
bool
ListAttribute<T, C, O>::check(T const& v) const
{
  for (auto& l : m_values) {
    if (m_comparator(v, l)) {
      return true;
    }
  }
  return false;
}

template<typename T, typename C, bool O>
bool
ListAttribute<T, C, O>::merge(Attribute const& b)
{
  if (not Attribute::merge(b)) {
    return false;
  }
  ListAttribute<T, C, O> const& rb =
    static_cast<ListAttribute<T, C, O> const&>(b);
  std::list<T> extra;
  for (auto& v : rb.m_values) {
    for (auto& l : m_values) {
      if (not m_comparator(v, l)) {
        extra.push_back(v);
      }
    }
  }
  for (auto& v : extra) {
    m_values.push_back(v);
  }
  return true;
}

template<typename T, typename C, bool O>
bool
ListAttribute<T, C, O>::checkConstraint(
  std::list<tree::Value::Ref> const& v) const
{
  for (auto& a : v) {
    if (not a->isPrimitive()) {
      ERROR(ERR_LIST_ITEM_TYPE_MISMATCH);
      return false;
    }
    if (not static_cast<tree::Primitive const&>(*a).is<T>()) {
      ERROR(ERR_LIST_ITEM_TYPE_MISMATCH);
      return false;
    }
  }
  return true;
}

template<typename T, typename C, bool O>
bool
ListAttribute<T, C, O>::constrain(std::list<tree::Value::Ref> const& v)
{
  for (auto& a : v) {
    if (not a->isPrimitive()) {
      ERROR(ERR_LIST_ITEM_TYPE_MISMATCH);
      return false;
    }
    if (not static_cast<tree::Primitive const&>(*a).is<T>()) {
      ERROR(ERR_LIST_ITEM_TYPE_MISMATCH);
      return false;
    }
    m_values.clear();
    for (auto& a : v) {
      m_values.push_back(static_cast<tree::Primitive const&>(*a).value<T>());
    }
  }
  return true;
}

template<typename T, typename C, bool O>
ListAttribute<T, C, O>::operator tree::Checker::Pattern() const
{
  return tree::Checker::Pattern(tree::Value::Type::Array, m_optional);
}

template<typename T, typename C, bool O>
void
ListAttribute<T, C, O>::print(std::ostream& o, int l) const
{
  size_t cnt = 0;
  bool expand = m_values.size() > 4 and m_values.size() != 0;
  o << "[ ";
  for (auto& v : m_values) {
    if (cnt % 4 == 0 and expand) {
      o << std::endl;
      Generator::indent(o, l + 2);
    }
    o << v;
    cnt += 1;
    if (cnt < m_values.size()) {
      o << ", ";
    }
  }
  if (expand) {
    o << std::endl;
  }
  Generator::indent(o, expand ? l : 1) << "]" << std::endl;
}

template<typename T, typename C, bool O>
ListAttribute<T, C, O>::operator std::string() const
{
  size_t cnt = 0;
  std::ostringstream oss;
  oss << "[ ";
  for (auto& v : m_values) {
    cnt += 1;
    oss << v;
    if (cnt < m_values.size()) {
      oss << ", ";
    }
  }
  oss << " ]";
  return oss.str();
}

template<typename T, typename C, bool O>
Attribute::Ref
ListAttribute<T, C, O>::clone() const
{
  return Attribute::Ref(new ListAttribute<T, C, O>(*this));
}

template<typename T, typename C, bool O>
std::list<T> const&
ListAttribute<T, C, O>::values() const
{
  return m_values;
}

}}
