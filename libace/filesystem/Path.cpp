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

#include <ace/filesystem/Path.h>
#include <ace/common/String.h>
#include <sstream>
#include <string>
#include <vector>

namespace ace {
namespace fs {

Path::Path(std::string const & v, bool enforceDir) : Path() {
  std::vector<std::string> elems;
  common::String::split(v, '/', elems);
  for (size_t i = 0; i < elems.size(); i += 1) {
    if (not elems[i].empty() || (elems[i].empty() && (i == 0 or i == elems.size() -1))) {
      m_elements.push_back(elems[i]);
    }
  }
  if (not isDirectory() and enforceDir) {
    m_elements.push_back(std::string());
  }
}

Path::Path(std::vector<std::string> const & c) : m_elements(c) { }

Path
Path::operator-(Path const & o) const {
  if (m_elements.empty()) return Path();
  if (o.empty()) return Path(*this);
  std::vector<std::string> vals;
  auto ita = m_elements.begin();
  auto itb = o.m_elements.begin();
  while (ita != m_elements.end() and itb != o.m_elements.end()) {
    if (*ita != *itb) break;
    ita++, itb++;
  }
  if (ita == m_elements.end() || (itb != o.m_elements.end() && not itb->empty())) {
    return Path();
  }
  while (ita != m_elements.end()) vals.push_back(*ita++);
  return Path(vals);
}

Path
Path::operator/(Path const & o) const {
  if (o.isAbsolute()) return Path();
  if (m_elements.empty() or not isDirectory()) return Path();
  std::vector<std::string> elems(m_elements);
  elems.erase(--elems.end());
  for (auto & e : o.m_elements) elems.push_back(e);
  return Path(elems);
}

std::string
Path::toString() const {
  std::ostringstream oss;
  oss << *this;
  return oss.str();
}

Path
Path::prune() const {
  std::vector<std::string> elems(m_elements);
  if (not elems.empty()) {
    if ((--elems.end())->empty()) elems.erase(--elems.end());
    elems.erase(--elems.end());
    if (not elems.empty()) elems.push_back("");
  }
  return Path(elems);
}

Path
Path::compress() const {
  std::vector<std::string> elems;
  for (auto & e : m_elements) {
    if (e == ".") continue;
    elems.push_back(e);
  }
  return Path(elems);
}

bool
Path::isAbsolute() const {
  if (m_elements.empty()) return false;
  return (*m_elements.begin()).empty();
}

bool
Path::isDirectory() const {
  if (m_elements.empty()) return false;
  return (*m_elements.rbegin()).empty();
}

std::string
Path::prefix(std::string const & a, std::string const & b) {
  Path pa(a), pb(b);
  return prefix(pa, pb).toString();
}

Path
Path::prefix(Path const & a, Path const & b) {
  std::vector<std::string> vals;
  if (a.empty() or b.empty()) return Path();
  auto ita = a.begin();
  auto itb = b.begin();
  while (ita != a.end() and itb != b.end()) {
    if (*ita != *itb) break;
    vals.push_back(*ita);
    ita++, itb++;
  }
  if (ita == a.end() and itb == b.end()) {
    return Path(vals);
  } else if (ita != a.end() and itb != b.end()) {
    vals.push_back(std::string());
    return Path(vals);
  } else if ((ita != a.end() and itb == b.end()) || (ita == a.end() and itb != b.end())) {
    if (not vals.empty()) {
      vals.erase(--vals.end());
      vals.push_back(std::string());
      return Path(vals);
    }
  }
  return Path();
}

std::ostream &
operator<<(std::ostream & o, Path const & p) {
  for (size_t i = 0; i < p.m_elements.size(); i += 1) {
    o << p.m_elements[i];
    if (i < p.m_elements.size() - 1) o << "/";
  }
  return o;
}

bool
Path::operator==(Path const & o) const {
  return m_elements == o.m_elements;
}

bool
Path::operator!=(Path const & o) const {
  return m_elements != o.m_elements;
}

bool
Path::empty() const {
  return m_elements.empty();
}

Path::iterator
Path::begin() {
  return m_elements.begin();
}

Path::const_iterator
Path::begin() const {
  return m_elements.begin();
}

Path::iterator
Path::end() {
  return m_elements.end();
}

Path::const_iterator
Path::end() const {
  return m_elements.end();
}

Path::reverse_iterator
Path::rbegin() {
  return m_elements.rbegin();
}

Path::const_reverse_iterator
Path::rbegin() const {
  return m_elements.rbegin();
}

Path::reverse_iterator
Path::rend() {
  return m_elements.rend();
}

Path::const_reverse_iterator
Path::rend() const {
  return m_elements.rend();
}

Path::iterator
Path::up(iterator const & i) {
  Path::iterator n(i);
  return --n;
}

Path::const_iterator
Path::up(const_iterator const & i) const {
  Path::const_iterator n(i);
  return --n;
}

Path::iterator
Path::down(iterator const & i) {
  Path::iterator n(i);
  return ++n;
}

Path::const_iterator
Path::down(const_iterator const & i) const {
  Path::const_iterator n(i);
  return ++n;
}

Path::reverse_iterator
Path::up(reverse_iterator const & i) {
  Path::reverse_iterator n(i);
  return ++n;
}

Path::const_reverse_iterator
Path::up(const_reverse_iterator const & i) const {
  Path::const_reverse_iterator n(i);
  return ++n;
}

Path::reverse_iterator
Path::down(reverse_iterator const & i) {
  Path::reverse_iterator n(i);
  return --n;
}

Path::const_reverse_iterator
Path::down(const_reverse_iterator const & i) const {
  Path::const_reverse_iterator n(i);
  return --n;
}

} // namespace fs
} // namespace ace
