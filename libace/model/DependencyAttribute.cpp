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

#include <ace/model/DependencyAttribute.h>
#include <ace/tree/Array.h>
#include <string>

namespace ace { namespace model {

bool
DependencyAttribute<>::Comparator::operator()(Dependency::Ref const& a,
                                              Dependency::Ref const& b) const
{
  return a->priority() < b->priority();
}

DependencyAttribute<>::DependencyAttribute(DependencyAttribute const& o)
  : Attribute(o), m_values()
{
  for (auto& d : o.m_values) {
    Dependency::Ref nd(d->clone());
    nd->setParent(this);
    m_values.insert(nd);
  }
}

DependencyAttribute<>::DependencyAttribute(std::string const& n, bool o)
  : Attribute(n, o), m_values()
{}

bool
DependencyAttribute<>::checkModel(tree::Value const& t) const
{
  if (t.type() != tree::Value::Type::Array) {
    ERROR(ERR_DEPS_NOT_AN_ARRAY);
    return false;
  }
  auto const& ary = static_cast<tree::Array const&>(t);
  if (ary.size() == 0) {
    ERROR(ERR_EMPTY_DEPS);
    return false;
  }
  for (auto& d : ary) {
    if (not checkDependency(*d)) {
      return false;
    }
  }
  return true;
}

void
DependencyAttribute<>::loadModel(tree::Value const& t)
{
  auto const& ary = static_cast<tree::Array const&>(t);
  for (auto& d : ary) {
    loadDependency(*d);
  }
}

bool
DependencyAttribute<>::checkDependency(tree::Value const& t) const
{
  return false;
}

void
DependencyAttribute<>::loadDependency(tree::Value const& t)
{}

bool
DependencyAttribute<>::validateModel()
{
  int score = 0;
  for (auto& d : m_values) {
    if (not d->validateModel()) {
      score += 1;
    }
  }
  return score == 0;
}

void
DependencyAttribute<>::load(Attribute const& a)
{
  DependencyAttribute<> const& ra =
    static_cast<DependencyAttribute<> const&>(a);
  m_values = ra.m_values;
}

bool
DependencyAttribute<>::merge(Attribute const& b)
{
  if (not Attribute::merge(b)) {
    return false;
  }
  Dependencies extra;
  DependencyAttribute<> const& rb =
    static_cast<DependencyAttribute<> const&>(b);
  for (auto& o : rb.m_values) {
    bool merged = false;
    for (auto& l : m_values) {
      if (l->canMerge(*o)) {
        l->merge(*o);
        merged = true;
        break;
      }
    }
    if (not merged) {
      extra.insert(o);
    }
  }
  for (auto& e : extra) {
    e->setParent(this);
    m_values.insert(e);
  }
  return true;
}

DependencyAttribute<>::operator tree::Checker::Pattern() const
{
  return tree::Checker::Pattern(tree::Value::Type::Array, m_optional);
}

void
DependencyAttribute<>::print(std::ostream& o, int l) const
{
  bool indent = false;
  for (auto& a : m_values) {
    Generator::indent(o, indent ? l : 0) << (std::string)*a << std::endl;
    if (not indent) {
      indent = true;
    }
  }
}

DependencyAttribute<>::operator std::string() const
{
  return std::string();
}

typename DependencyAttribute<>::Dependencies const&
DependencyAttribute<>::values() const
{
  return m_values;
}

}}
