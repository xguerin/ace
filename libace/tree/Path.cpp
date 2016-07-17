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

#include <ace/tree/Path.h>
#include <ace/tree/Lexer.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

namespace ace {
namespace tree {

Path::Ref
Path::build(Path const & p) {
  return Ref(new Path(p));
}

Path
Path::parse(std::string const & s) {
  std::string v(s);
  return path::Scan().parse(v);
}

Path &
Path::push(path::Item::Ref const & ir) {
  m_items.push_back(ir);
  return *this;
}

Path
Path::sub(const_iterator const & from, const_iterator const & to) const {
  Path result;
  for (auto it = from; it != to; it++) {
    result.m_items.push_back(*it);
  }
  return result;
}

Path
Path::merge(Path const & o) const {
  if (m_items.empty()) {
    return o;
  }
  Path result(*this);
  for (auto & e: o.m_items) if (e->root()) {
    result.m_items.push_back(e);
  }
  return result;
}

bool
Path::generative() const {
  for (auto & e : m_items) if (e->type() == path::Item::Type::Any or e->recursive()) {
    return true;
  }
  return false;
}

bool
Path::global() const {
  if (m_items.empty()) return false;
  return m_items.front()->type() == path::Item::Type::Global;
}

bool
Path::operator<(Path const & o) const {
  return toString() < o.toString();
}

bool
Path::operator==(Path const & o) const {
  if (m_items.size() != o.m_items.size()) return false;
  for (auto lit = m_items.begin(), rit = o.m_items.begin(); lit != m_items.end(); lit++, rit++) {
    if (**lit != **rit) return false;
  }
  return true;
}

bool
Path::operator!=(Path const & o) const {
  return not operator==(o);
}

std::string
Path::toString(const bool useBrackets) const {
  std::ostringstream o;
  for (size_t i = 0; i < m_items.size(); i += 1) {
    o << m_items[i]->toString(useBrackets);
  }
  return o.str();
}

Path::operator std::string() const {
  return toString();
}

Path::iterator
Path::begin() {
  return m_items.begin();
}

Path::iterator
Path::end() {
  return m_items.end();
}

Path::reverse_iterator
Path::rbegin() {
  return m_items.rbegin();
}

Path::reverse_iterator
Path::rend() {
  return m_items.rend();
}

Path::const_iterator
Path::begin() const {
  return m_items.begin();
}

Path::const_iterator
Path::end() const {
  return m_items.end();
}

Path::const_reverse_iterator
Path::rbegin() const {
  return m_items.rbegin();
}

Path::const_reverse_iterator
Path::rend() const {
  return m_items.rend();
}

Path::iterator
Path::up(iterator const & it) const {
  auto nit(it);
  return --nit;
}

Path::iterator
Path::down(iterator const & it) const {
  auto nit(it);
  return ++nit;
}

Path::const_iterator
Path::up(const_iterator const & it) const {
  auto nit(it);
  return --nit;
}

Path::const_iterator
Path::down(const_iterator const & it) const {
  auto nit(it);
  return ++nit;
}

std::ostream &
operator<<(std::ostream & o, Path const & p) {
  o << p.toString();
  return o;
}

} // namespace tree
} // namespace ace

