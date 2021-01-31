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

#include <ace/model/AttributeSet.h>
#include <ace/model/Errors.h>
#include <ace/engine/Master.h>
#include <ace/tree/Object.h>
#include <iostream>
#include <string>

namespace ace { namespace model {

// Attribute ID less comparator

bool
AttributeLess::operator()(Attribute::Ref const& a,
                          Attribute::Ref const& b) const
{
  return a->id() < b->id();
}

// Attribute set

AttributeSet::AttributeSet()
  : m_schema(), m_schemaSet(), m_instances(), m_instanceSet()
{}

AttributeSet::AttributeSet(AttributeSet const& o)
  : Object(o)
  , Instance(o)
  , m_schema()
  , m_schemaSet()
  , m_instances()
  , m_instanceSet()
{
  for (auto& r : o.m_schemaSet) {
    Attribute::Ref ar(r->clone());
    ar->setParent(this);
    m_schema[ar->name()] = ar;
    m_schemaSet.insert(ar);
  }
  for (auto& r : o.m_instanceSet) {
    Attribute::Ref ar(r->clone());
    ar->setParent(this);
    m_instances[ar->name()] = ar;
    m_instanceSet.insert(ar);
  }
}

tree::Path
AttributeSet::path(const bool local) const
{
  return m_parent->path(local);
}

bool
AttributeSet::checkModel(tree::Value const& t) const
{
  size_t score = 0;
  if (not t.isObject()) {
    ERROR(ERR_WRONG_ATTRIBUTE_SET_TYPE);
    return false;
  }
  auto const& obj = static_cast<tree::Object const&>(t);
  for (auto& e : obj) {
    std::string const& k = e.first;
    ace::tree::Value const& v = *e.second;
    if (m_schema.find(k) == m_schema.end()) {
      ERROR(ERR_INVALID_ATTRIBUTE(k));
      score += 1;
    } else if (not m_schema.at(k)->checkModel(v)) {
      ERROR(ERR_INVALID_ATTRIBUTE_VALUE(k));
      score += 1;
    }
  }
  for (auto& e : m_schema) {
    if (not e.second->optional() and not t.has(e.first)) {
      ERROR(ERR_MISSING_ATTRIBUTE(e.first));
      score += 1;
    }
  }
  return score == 0;
}

void
AttributeSet::loadModel(tree::Value const& t)
{
  auto const& obj = static_cast<tree::Object const&>(t);
  for (auto& e : obj) {
    std::string const& n = e.first;
    m_instances[n] = m_schema[n];
    m_instances[n]->loadModel(*e.second);
    m_instanceSet.insert(m_schema[n]);
  }
}

bool
AttributeSet::flattenModel()
{
  int score = 0;
  for (auto& a : m_instanceSet) {
    if (not a->flattenModel()) {
      score += 1;
    }
  }
  return score == 0;
}

bool
AttributeSet::validateModel()
{
  if (not clearConflicts()) {
    return false;
  }
  int score = 0;
  for (auto& a : m_instanceSet) {
    if (not a->validateModel()) {
      score += 1;
    }
  }
  return score == 0;
}

bool
AttributeSet::checkInstance(tree::Object const& r, tree::Value const& v) const
{
  for (auto& a : m_instanceSet) {
    if (not a->checkInstance(r, v)) {
      return false;
    }
  }
  return true;
}

void
AttributeSet::expandInstance(tree::Object& r, tree::Value& v)
{
  for (auto& a : m_instanceSet) {
    a->expandInstance(r, v);
  }
}

bool
AttributeSet::flattenInstance(tree::Object& r, tree::Value& v)
{
  for (auto& a : m_instanceSet) {
    if (not a->flattenInstance(r, v)) {
      return false;
    }
  }
  return true;
}

bool
AttributeSet::resolveInstance(tree::Object const& r, tree::Value const& v) const
{
  for (auto& a : m_instanceSet) {
    if (not a->resolveInstance(r, v)) {
      return false;
    }
  }
  return true;
}

void
AttributeSet::activate(std::string const& n)
{
  if (m_instances.count(n) != 0) {
    return;
  }
  m_instances[n] = m_schema[n];
  m_instanceSet.insert(m_schema[n]);
}

bool
AttributeSet::merge(AttributeSet const& b)
{
  for (auto& a : m_instanceSet) {
    if (b.m_instances.find(a->name()) != b.m_instances.end()) {
      if (not a->merge(*b.m_instances.at(a->name()))) {
        return false;
      }
    }
  }
  for (auto& a : b.m_instanceSet) {
    if (m_instances.find(a->name()) == m_instances.end()) {
      a->setParent(this);
      m_instances[a->name()] = a;
      m_instanceSet.insert(a);
    }
  }
  for (auto& e : b.m_xorList) {
    for (auto& x : e.second) {
      m_xorList[e.first].insert(x);
    }
  }
  return clearConflicts();
}

bool
AttributeSet::override(AttributeSet const& b)
{
  for (auto& a : m_instanceSet) {
    if (b.m_instances.find(a->name()) != b.m_instances.end()) {
      if (a->override(*b.m_instances.at(a->name()))) {
        continue;
      }
      if (not a->merge(*b.m_instances.at(a->name()))) {
        return false;
      }
    }
  }
  for (auto& a : b.m_instanceSet) {
    if (m_instances.find(a->name()) == m_instances.end()) {
      a->setParent(this);
      m_instances[a->name()] = a;
      m_instanceSet.insert(a);
    }
  }
  for (auto& e : b.m_xorList) {
    for (auto& x : e.second) {
      m_xorList[e.first].insert(x);
    }
  }
  return clearConflicts();
}

tree::Checker::Schema
AttributeSet::schema() const
{
  tree::Checker::Schema s;
  for (auto& e : m_schema) {
    s[e.first] = *e.second;
  }
  return s;
}

bool
AttributeSet::clearConflicts() const
{
  int score = 0;
  for (auto& e : m_xorList) {
    for (auto& m : e.second) {
      if (m_instances.find(e.first) != m_instances.end() and
          m_instances.find(m) != m_instances.end()) {
        ERROR(ERR_CONFLICTING_ATTRIBUTE(e.first, m));
        score += 1;
      }
    }
  }
  return score == 0;
}

void
AttributeSet::exclude(std::string const& a, std::string const& b)
{
  m_xorList[a].insert(b);
  m_xorList[b].insert(a);
}

bool
AttributeSet::has(std::string const& n) const
{
  return m_instances.find(n) != m_instances.end();
}

Attribute::Ref const&
AttributeSet::operator[](std::string const& n) const
{
  return m_instances.at(n);
}

AttributeSet::iterator
AttributeSet::begin()
{
  return m_instanceSet.begin();
}

AttributeSet::const_iterator
AttributeSet::begin() const
{
  return m_instanceSet.begin();
}

AttributeSet::iterator
AttributeSet::end()
{
  return m_instanceSet.end();
}

AttributeSet::const_iterator
AttributeSet::end() const
{
  return m_instanceSet.end();
}

}}
