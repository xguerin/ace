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

#include <ace/tree/Item.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ace {
namespace tree {
namespace path {

// Range

bool
Item::Range::operator==(Range const & o) const {
  return low == o.low and high == o.high and steps == o.steps;
}

bool
Item::Range::operator!=(Range const & o) const {
  return not operator==(o);
}

bool
Item::Range::contains(const size_t v) const {
  return low <= v && high > v && (low - v) % steps == 0;
}

// Item

Item::Ref
Item::build(const Type t) {
  return Ref(new Item(t));
}

Item::Ref
Item::build(const Type t, std::vector<size_t> const & idx) {
  return Ref(new Item(t, idx));
}

Item::Ref
Item::build(const Type t, std::string const & v) {
  return Ref(new Item(t, v));
}

Item::Ref
Item::build(const Type t, Range const & rng) {
  return Ref(new Item(t, rng));
}

Item::Item(const Type t)
  : m_type(t), m_rec(false), m_indexes(), m_value(), m_range() { }

Item::Item(const Type t, std::vector<size_t> const & idx)
  : m_type(t), m_rec(false), m_indexes(idx), m_value(), m_range() { }

Item::Item(const Type t, std::string const & v)
  : m_type(t), m_rec(false), m_indexes(), m_value(v), m_range() { }

Item::Item(const Type t, Range const & rng)
  : m_type(t), m_rec(false), m_indexes(), m_value(), m_range(rng) { }

void
Item::setRecursive() {
  m_rec = true;
}

bool
Item::recursive() const {
  return m_rec;
}

Item::Type
Item::type() const {
  return m_type;
}

std::vector<size_t> const &
Item::indexes() const {
  return m_indexes;
}

Item::Range const &
Item::range() const {
  return m_range;
}

bool
Item::root() const {
  return m_type == Type::Local or m_type == Type::Global;
}

std::string const &
Item::value() const {
  return m_value;
}

bool
Item::operator==(Item const & o) const {
  if (m_type != o.m_type) return false;
  if (m_rec != o.m_rec) return false;
  if (m_indexes.size() != o.m_indexes.size()) return false;
  for (auto li = m_indexes.begin(), ri = o.m_indexes.begin(); li != m_indexes.end(); li++, ri++) {
    if (*li != *ri) return false;
  }
  if (m_value != o.m_value) return false;
  if (m_range != m_range) return false;
  return true;
}

bool
Item::operator!=(Item const & o) const {
  return not operator==(o);
}

std::string
Item::toString(const bool useBrackets) const {
  std::ostringstream o;
  if (m_rec) o << "..";
  switch (m_type) {
    case Item::Type::Any:     o << (useBrackets ? "[*]" : (m_rec ? "*" : ".*"));
                              break;
    case Item::Type::Local:   o << "@";
                              break;
    case Item::Type::Global:  o << "$";
                              break;
    case Item::Type::Indexed: o << "[";
                              for (size_t i = 0; i < m_indexes.size(); i += 1) {
                                o << m_indexes[i];
                                if (i < m_indexes.size() - 1) o << ",";
                              }
                              o << "]";
                              break;
    case Item::Type::Named:   if (useBrackets) {
                                o << "['" + m_value + "']";
                              } else {
                                o << (m_rec ? m_value : "." + m_value);
                              }
                              break;
    case Item::Type::Ranged:  o << "[";
                              o << m_range.low << ":" << m_range.high << ":" << m_range.steps;
                              o << "]";
                              break;
  }
  return o.str();
}

std::ostream &
operator<<(std::ostream & o, Item const & p) {
  o << p.toString();
  return o;
}

} // namespace path
} // namespace tree
} // namespace ace

