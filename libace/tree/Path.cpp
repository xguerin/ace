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

#include <ace/common/Log.h>
#include <ace/tree/Path.h>
#include <ace/tree/Lexer.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

namespace {

using namespace ace::tree;
using namespace ace::tree::path;

static bool
match(Path const& a, Path const& b, Path::const_iterator const& i,
      Path::const_iterator const& j)
{
  //
  // Match if both path are done
  //
  if (i == a.end() && j == b.end()) {
    return true;
  }
  //
  // Match if path a is done
  //
  if (i == a.end() && j != b.end()) {
    return true;
  }
  //
  // Not a match if path b is done
  //
  if (i != a.end() && j == b.end()) {
    ACE_LOG(Debug, "Matched path is too short");
    return false;
  }
  //
  // Any items
  //
  if ((*i)->type() == Item::Type::Any) {
    return match(a, b, a.down(i), b.down(j));
  }
  //
  // Named items
  //
  if ((*i)->type() == Item::Type::Named && (*j)->type() == Item::Type::Named) {
    if ((*i)->value() == (*j)->value()) {
      return match(a, b, a.down(i), b.down(j));
    } else if ((*i)->recursive()) {
      return match(a, b, i, b.down(j));
    } else {
      ACE_LOG(Debug, (*i)->value(), " != ", (*j)->value());
      return false;
    }
  }
  //
  // Ranged items
  //
  if ((*i)->type() == Item::Type::Ranged &&
      (*j)->type() == Item::Type::Ranged) {
    if ((*i)->range().contains((*j)->range().low)) {
      return match(a, b, a.down(i), b.down(j));
    } else {
      ACE_LOG(Debug, "Matched path out of range");
      return false;
    }
  }
  if ((*i)->type() == Item::Type::Indexed &&
      (*j)->type() == Item::Type::Ranged) {
    auto const& x = (*i)->indexes();
    if (std::find(x.begin(), x.end(), (*j)->range().low) != x.end()) {
      return match(a, b, a.down(i), b.down(j));
    } else {
      ACE_LOG(Debug, "Matched path not in the matching selection");
      return false;
    }
  }
  //
  // Indexed items
  //
  if ((*i)->type() == Item::Type::Indexed &&
      (*j)->type() == Item::Type::Indexed) {
    auto const& x = (*i)->indexes();
    if (std::find(x.begin(), x.end(), (*j)->indexes().front()) != x.end()) {
      return match(a, b, a.down(i), b.down(j));
    } else {
      ACE_LOG(Debug, "Matched path not in the matching selection");
      return false;
    }
  }
  if ((*i)->type() == Item::Type::Ranged &&
      (*j)->type() == Item::Type::Indexed) {
    if ((*i)->range().contains((*j)->indexes().front())) {
      return match(a, b, a.down(i), b.down(j));
    } else {
      ACE_LOG(Debug, "Matched path out of range");
      return false;
    }
  }
  //
  // Return default value
  //
  ACE_LOG(Debug, (*i)->toString(), " and ", (*j)->toString(),
          " are not compatible");
  return false;
}

}

namespace ace { namespace tree {

Path::Ref
Path::build(Path const& p)
{
  return Ref(new Path(p));
}

Path
Path::parse(std::string const& s)
{
  return path::Scan().parse(s);
}

Path&
Path::push(path::Item::Ref const& ir)
{
  m_items.push_back(ir);
  return *this;
}

Path
Path::sub(const_iterator const& from, const_iterator const& to) const
{
  Path result;
  if (from != begin() && !m_items.empty()) {
    result.m_items.push_back(m_items[0]);
  }
  for (auto it = from; it != to; it++) {
    result.m_items.push_back(*it);
  }
  return result;
}

Path
Path::merge(Path const& o) const
{
  if (m_items.empty()) {
    return o;
  }
  Path result(*this);
  for (auto& e : o.m_items) {
    if (!e->root()) {
      result.m_items.push_back(e);
    }
  }
  return result;
}

bool
Path::generative() const
{
  for (auto& e : m_items) {
    if (e->type() == path::Item::Type::Any or e->recursive()) {
      return true;
    }
    if (e->type() == path::Item::Type::Indexed and e->indexes().size() > 1) {
      return true;
    }
    if (e->type() == path::Item::Type::Ranged) {
      auto const& range = e->range();
      if ((range.high - range.low) / range.steps > 1) {
        return true;
      }
    }
  }
  return false;
}

bool
Path::global() const
{
  if (m_items.empty()) {
    return false;
  }
  return m_items.front()->type() == path::Item::Type::Global;
}

bool
Path::operator<(Path const& o) const
{
  return toString() < o.toString();
}

bool
Path::operator==(Path const& o) const
{
  if (m_items.size() != o.m_items.size()) {
    return false;
  }
  for (auto lit = m_items.begin(), rit = o.m_items.begin();
       lit != m_items.end(); lit++, rit++) {
    if (**lit != **rit) {
      return false;
    }
  }
  return true;
}

bool
Path::operator!=(Path const& o) const
{
  return not operator==(o);
}

bool
Path::match(Path const& p) const
{
  if (m_items.empty() || p.m_items.empty()) {
    ACE_LOG(Debug, "Matching paths cannot be empty");
    return false;
  }
  if (p.generative()) {
    ACE_LOG(Debug, "Matched path cannot be generative");
    return false;
  }
  return ::match(*this, p, down(begin()), p.down(p.begin()));
}

std::string
Path::toString(const bool useBrackets) const
{
  std::ostringstream o;
  for (size_t i = 0; i < m_items.size(); i += 1) {
    o << m_items[i]->toString(useBrackets);
  }
  return o.str();
}

Path::operator std::string() const
{
  return toString();
}

Path::iterator
Path::begin()
{
  return m_items.begin();
}

Path::iterator
Path::end()
{
  return m_items.end();
}

Path::reverse_iterator
Path::rbegin()
{
  return m_items.rbegin();
}

Path::reverse_iterator
Path::rend()
{
  return m_items.rend();
}

Path::const_iterator
Path::begin() const
{
  return m_items.begin();
}

Path::const_iterator
Path::end() const
{
  return m_items.end();
}

Path::const_reverse_iterator
Path::rbegin() const
{
  return m_items.rbegin();
}

Path::const_reverse_iterator
Path::rend() const
{
  return m_items.rend();
}

Path::iterator
Path::up(iterator const& it) const
{
  auto nit(it);
  return --nit;
}

Path::iterator
Path::down(iterator const& it) const
{
  auto nit(it);
  return ++nit;
}

Path::const_iterator
Path::up(const_iterator const& it) const
{
  auto nit(it);
  return --nit;
}

Path::const_iterator
Path::down(const_iterator const& it) const
{
  auto nit(it);
  return ++nit;
}

std::ostream&
operator<<(std::ostream& o, Path const& p)
{
  o << p.toString();
  return o;
}

}}
