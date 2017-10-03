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

#include <ace/tree/Value.h>
#include <ace/common/String.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace ace {
namespace tree {

Value::Value(std::string const & n, Type t) : m_name(n), m_type(t), m_parent(nullptr) { }

void
Value::each(Callback c) {
  c(*this);
}

void
Value::each(ConstCallback c) const {
  c(*this);
}

void
Value::dump(std::ostream & o, int l, bool i) const {
}

void
Value::merge(Value const & o) {
}

bool
Value::has(std::string const & k) const {
  return false;
}

bool
Value::has(common::Path const & p) const {
  common::Path::const_iterator n(p.begin());
  if (p.isAbsolute()) {
    if (parent() != nullptr) return false;
    n = p.down(n);
  }
  return has(p, n);
}

bool
Value::has(common::Path const & p, common::Path::const_iterator const & i) const {
  return i == p.end();
}

bool
Value::has(Path const & p) const {
  if (p.global() and m_parent != nullptr) {
    auto & parent = *m_parent;
    return parent.has(p);
  } else {
    Path::const_iterator n(p.begin());
    return has(p, p.down(n));
  }
}

bool
Value::has(Path const & p, Path::const_iterator const & i) const {
  return i == p.end();
}

Value &
Value::operator[](std::string const & k) {
  throw std::invalid_argument("tree::Value as no children");
}

Value const &
Value::operator[](std::string const & k) const {
  throw std::invalid_argument("tree::Value as no children");
}

Value &
Value::get(std::string const & k) {
  throw std::invalid_argument("tree::Value as no children");
}

Value const &
Value::get(std::string const & k) const {
  throw std::invalid_argument("tree::Value as no children");
}

Value &
Value::get(common::Path const & p) {
  common::Path::const_iterator n(p.begin());
  if (p.isAbsolute()) n = p.down(n);
  return get(p, n);
}

Value const &
Value::get(common::Path const & p) const {
  common::Path::const_iterator n(p.begin());
  if (p.isAbsolute()) n = p.down(n);
  return get(p, n);
}

void
Value::get(Path const & p, std::vector<Value::Ref> & r) {
  get(p, p.down(p.begin()), r);
}

void
Value::get(Path const & p, std::vector<Value::Ref> & r) const {
  get(p, p.down(p.begin()), r);
}

void
Value::get(Path const & p, callback op) {
  if (p.global() and m_parent != nullptr) {
    auto & parent = *m_parent;
    parent.get(p, op);
  } else {
    get(p, p.down(p.begin()), op);
  }
}

void
Value::get(Path const & p, const_callback op) const {
  if (p.global() and m_parent != nullptr) {
    auto const & parent = *m_parent;
    parent.get(p, op);
  } else {
    get(p, p.down(p.begin()), op);
  }
}

Value &
Value::get(common::Path const & p, common::Path::const_iterator const & i) {
  if (i == p.end()) return *this;
  throw std::invalid_argument("tree::Value as no children");
}

Value const &
Value::get(common::Path const & p, common::Path::const_iterator const & i) const {
  if (i == p.end()) return *this;
  throw std::invalid_argument("tree::Value as no children");
}

void
Value::get(Path const & p, Path::const_iterator const & i, std::vector<Value::Ref> & r) {
  // Nothing to get
}

void
Value::get(Path const & p, Path::const_iterator const & i, std::vector<Value::Ref> & r) const {
  // Nothing to get
}

void
Value::get(Path const & p, Path::const_iterator const & i, callback op) {
  std::vector<Value::Ref> result;
  get(p, i, result);
  for (auto & r: result) op(*r);
}

void
Value::get(Path const & p, Path::const_iterator const & i, const_callback op) const {
  std::vector<Value::Ref> result;
  get(p, i, result);
  for (auto & r: result) op(*r);
}

void
Value::erase(std::string const & k) {
  throw std::invalid_argument("tree::Value as no children");
}

void
Value::erase(common::Path const & p) {
  common::Path::const_iterator n(p.begin());
  if (p.isAbsolute()) n = p.down(n);
  erase(p, n);
}

void
Value::erase(common::Path const & p, common::Path::const_iterator const & i) {
  if (i == p.end()) return;
  throw std::invalid_argument("tree::Value as no children");
}

void
Value::erase(Path const & p) {
  Path::const_iterator n(p.begin());
  erase(p, p.down(n));
}

void
Value::erase(Path const & p, Path::const_iterator const & i) {
  if (i == p.end()) return;
  throw std::invalid_argument("tree::Value as no children");
}

void
Value::setName(std::string const & n) {
  m_name = n;
}

std::string const &
Value::name() const {
  return m_name;
}

bool
Value::isPrimitive() const {
  return m_type == Type::String
    or m_type == Type::Float
    or m_type == Type::Integer
    or m_type == Type::Boolean;
}

bool
Value::isArray() const {
  return m_type == Type::Array;
}

bool
Value::isObject() const {
  return m_type == Type::Object;
}

Value::Type
Value::type() const {
  return m_type;
}

Value *
Value::parent() {
  return m_parent;
}

const Value *
Value::parent() const {
  return m_parent;
}

Path
Value::path() const {
  Path apath;
  if (m_parent != nullptr) apath = m_parent->path();
  if (std::all_of(m_name.begin(), m_name.end(), ::isdigit)) {
    size_t index = common::String::value<size_t>(m_name);
    apath.push(path::Item::build(path::Item::Type::Indexed, std::vector<size_t> { index }));
  } else {
    apath.push(path::Item::build(path::Item::Type::Named, m_name));
  }
  return apath;
}

} // namespace tree
} // namespace ace
