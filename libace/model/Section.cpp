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

#include <ace/model/Section.h>
#include <ace/tree/Checker.h>
#include <ace/types/Boolean.h>
#include <ace/types/Class.h>
#include <ace/types/CPUID.h>
#include <ace/types/Enum.h>
#include <ace/types/File.h>
#include <ace/types/Float.h>
#include <ace/types/Integer.h>
#include <ace/types/IPv4.h>
#include <ace/types/MAC.h>
#include <ace/types/Plugin.h>
#include <ace/types/Selector.h>
#include <ace/types/String.h>
#include <ace/types/URI.h>
#include <list>
#include <set>
#include <string>
#include <locale>

namespace ace { namespace model {

Section::Section() : m_types() {}

tree::Path
Section::path(const bool local) const
{
  return m_parent->path(local);
}

bool
Section::checkModel(tree::Value const& t) const
{
  size_t score = 0;
  for (auto& e : static_cast<tree::Object const&>(t)) {
    if (not checkName(e.first)) {
      ERROR(ERR_ILLEGAL_OPTION_NAME(e.first));
      score += 1;
    }
    if (not e.second->has("kind") or not checkType(e.first, *e.second)) {
      ERROR(ERR_FAILED_CHECKING_TYPE_MODEL(e.first));
      score += 1;
    }
  }
  return score == 0;
}

void
Section::loadModel(tree::Value const& t)
{
  for (auto& e : static_cast<tree::Object const&>(t)) {
    DEBUG("Load option \"", e.first, "\"");
    loadType(e.first, *e.second);
  }
}

bool
Section::flattenModel()
{
  size_t score = 0;
  for (auto& e : m_types) {
    DEBUG("Flatten option \"", e.first, "\"");
    if (not e.second->flattenModel()) {
      ERROR(ERR_FAILED_FLATTENING_MODEL(e.first));
      score += 1;
    }
  }
  return score == 0;
}

bool
Section::validateModel()
{
  int score = 0;
  for (auto& e : m_types) {
    DEBUG("Validate option \"", e.first, "\"");
    if (not e.second->validateModel()) {
      ERROR(ERR_FAILED_VALIDATING_TYPE_MODEL(e.first));
      score += 1;
    }
  }
  return score == 0;
}

bool
Section::merge(Section const& b)
{
  int score = 0;
  for (auto& e : b.m_types) {
    if (m_types.find(e.first) != m_types.end()) {
      if (not m_types[e.first]->merge(*e.second)) {
        ERROR(ERR_FAILED_MERGING_TYPE(e.first));
        score += 1;
      }
    } else {
      e.second->setParent(this);
      m_types[e.first] = e.second;
    }
  }
  return score == 0;
}

bool
Section::override(Section const& b)
{
  int score = 0;
  for (auto& e : b.m_types) {
    if (m_types.find(e.first) != m_types.end()) {
      if (not m_types[e.first]->override(*e.second)) {
        ERROR(ERR_FAILED_OVERRIDING_TYPE(e.first));
        score += 1;
      }
    } else {
      e.second->setParent(this);
      m_types[e.first] = e.second;
    }
  }
  return score == 0;
}

bool
Section::has(tree::Path const& p, tree::Path::const_iterator const& i) const
{
  if (i == p.end()) {
    return true;
  }
  size_t success = 0;
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (m_types.find((*i)->value()) != m_types.end()) {
        success += m_types.at((*i)->value())->has(p, p.down(i)) ? 1 : 0;
      }
    } break;
    case tree::path::Item::Type::Any: {
      if (p.down(i) != p.end()) {
        for (auto& e : m_types) {
          success += e.second->has(p, p.down(i)) ? 1 : 0;
        }
      } else {
        success += 1;
      }
    } break;
    default:
      return false;
  }
  if ((*i)->recursive()) {
    for (auto& e : m_types) {
      success += e.second->has(p, i) ? 1 : 0;
    }
  }
  return success != 0;
}

void
Section::get(tree::Path const& p, tree::Path::const_iterator const& i,
             std::list<BasicType::Ref>& r) const
{
  if (i == p.end()) {
    return;
  }
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      auto bt = m_types.at((*i)->value());
      if (p.down(i) == p.end()) {
        r.push_back(bt);
      } else {
        bt->get(p, p.down(i), r);
      }
    } break;
    case tree::path::Item::Type::Any: {
      for (auto& e : m_types) {
        if (p.down(i) == p.end()) {
          r.push_back(e.second);
        } else {
          e.second->get(p, p.down(i), r);
        }
      }
    } break;
    default:
      return;
  }
  if ((*i)->recursive()) {
    for (auto& e : m_types) {
      e.second->get(p, i, r);
    }
  }
}

void
Section::promoteArity(tree::Path const& p, tree::Path::const_iterator const& i)
{
  if (i == p.end()) {
    return;
  }
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (m_types.find((*i)->value()) != m_types.end()) {
        BasicType& bt = *m_types[(*i)->value()];
        bt.promoteArity(p, p.down(i));
      }
    } break;
    case tree::path::Item::Type::Any: {
      for (auto& e : m_types) {
        e.second->promoteArity(p, p.down(i));
      }
    } break;
    default:
      break;
  }
  if ((*i)->recursive()) {
    for (auto& e : m_types) {
      e.second->promoteArity(p, i);
    }
  }
}

void
Section::disable(tree::Path const& p, tree::Path::const_iterator const& i)
{
  if (i == p.end()) {
    return;
  }
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (m_types.find((*i)->value()) != m_types.end()) {
        BasicType& bt = *m_types[(*i)->value()];
        bt.disable(p, p.down(i));
      }
    } break;
    case tree::path::Item::Type::Any: {
      for (auto& e : m_types) {
        e.second->disable(p, p.down(i));
      }
    } break;
    default:
      break;
  }
  if ((*i)->recursive()) {
    for (auto& e : m_types) {
      e.second->disable(p, i);
    }
  }
}

bool
Section::checkName(std::string const& n) const
{
  std::locale loc;
  if (n.empty()) {
    return false;
  }
  if (std::isdigit(n[0], loc)) {
    return false;
  }
  for (auto& c : n) {
    if (not std::isalnum(c, loc) and c != '_') {
      return false;
    }
  }
  return true;
}

bool
Section::checkType(std::string const& n, tree::Value const& t) const
{
  auto value =
    static_cast<tree::Primitive const&>(t["kind"]).value<std::string>();
  switch (BasicType::kindOf(value)) {
    case BasicType::Kind::Boolean:
      return checkType<Boolean>(n, t);
    case BasicType::Kind::CPUID:
      return checkType<CPUID>(n, t);
    case BasicType::Kind::Enum:
      return checkType<Enum>(n, t);
    case BasicType::Kind::Class:
      return checkType<Class>(n, t);
    case BasicType::Kind::File:
      return checkType<File>(n, t);
    case BasicType::Kind::Float:
      return checkType<Float>(n, t);
    case BasicType::Kind::Integer:
      return checkType<Integer>(n, t);
    case BasicType::Kind::IPv4:
      return checkType<IPv4>(n, t);
    case BasicType::Kind::MAC:
      return checkType<MAC>(n, t);
    case BasicType::Kind::Plugin:
      return checkType<Plugin>(n, t);
    case BasicType::Kind::Selector:
      return checkType<Selector>(n, t);
    case BasicType::Kind::String:
      return checkType<String>(n, t);
    case BasicType::Kind::URI:
      return checkType<URI>(n, t);
    default:
      break;
  }
  ERROR(ERR_UNEXPECTED_TYPE(value));
  return false;
}

void
Section::loadType(std::string const& n, tree::Value const& t)
{
  BasicType::Ref tr;
  auto value =
    static_cast<tree::Primitive const&>(t["kind"]).value<std::string>();
  switch (BasicType::kindOf(value)) {
    case BasicType::Kind::Boolean:
      tr = BasicType::Ref(new Boolean());
      break;
    case BasicType::Kind::CPUID:
      tr = BasicType::Ref(new CPUID());
      break;
    case BasicType::Kind::Class:
      tr = BasicType::Ref(new Class());
      break;
    case BasicType::Kind::Enum:
      tr = BasicType::Ref(new Enum());
      break;
    case BasicType::Kind::File:
      tr = BasicType::Ref(new File());
      break;
    case BasicType::Kind::Float:
      tr = BasicType::Ref(new Float());
      break;
    case BasicType::Kind::Integer:
      tr = BasicType::Ref(new Integer());
      break;
    case BasicType::Kind::IPv4:
      tr = BasicType::Ref(new IPv4());
      break;
    case BasicType::Kind::MAC:
      tr = BasicType::Ref(new MAC());
      break;
    case BasicType::Kind::Plugin:
      tr = BasicType::Ref(new Plugin());
      break;
    case BasicType::Kind::Selector:
      tr = BasicType::Ref(new Selector());
      break;
    case BasicType::Kind::String:
      tr = BasicType::Ref(new String());
      break;
    case BasicType::Kind::URI:
      tr = BasicType::Ref(new URI());
      break;
    default:
      break;
  }
  tr->setName(n);
  tr->setParent(this);
  tr->loadModel(t);
  m_types[n] = tr;
}

bool
Section::has(std::string const& k) const
{
  return m_types.find(k) != m_types.end();
}

bool
Section::has(tree::Path const& p) const
{
  if (p.global() and owner()->parent() != nullptr) {
    auto const* up = static_cast<const Model*>(owner()->parent()->owner());
    return up->body().has(p);
  } else {
    tree::Path::const_iterator n(p.begin());
    return has(p, p.down(n));
  }
}

BasicType&
Section::get(std::string const& k)
{
  return *m_types[k];
}

BasicType const&
Section::get(std::string const& k) const
{
  return *m_types.at(k);
}

void
Section::get(tree::Path const& p, std::list<BasicType::Ref>& r) const
{
  if (p.global() and owner()->parent() != nullptr) {
    auto const* up = static_cast<const Model*>(owner()->parent()->owner());
    return up->body().get(p, r);
  } else {
    tree::Path::const_iterator n(p.begin());
    return get(p, p.down(n), r);
  }
}

void
Section::promoteArity(tree::Path const& p)
{
  tree::Path::const_iterator n(p.begin());
  promoteArity(p, p.down(n));
}

void
Section::disable(tree::Path const& p)
{
  tree::Path::const_iterator n(p.begin());
  disable(p, p.down(n));
}

Section::iterator
Section::begin()
{
  return m_types.begin();
}

Section::iterator
Section::end()
{
  return m_types.end();
}

Section::const_iterator
Section::begin() const
{
  return m_types.begin();
}

Section::const_iterator
Section::end() const
{
  return m_types.end();
}

}}
