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

#include <ace/model/Object.h>
#include <string>

namespace ace {
namespace model {

Object::Object() : m_id(0), m_name(), m_parent(nullptr) {
  static int s_id = 0;
  m_id = __sync_add_and_fetch(& s_id, 1);
}

Object::Object(Object const & o) : Object() {
  m_name = o.m_name;
  m_parent = o.m_parent;
}

int Object::id() const {
  return m_id;
}

std::string
Object::name() const {
  if (m_name.empty()) return "Object_" + std::to_string(m_id);
  return m_name;
}

void
Object::setName(std::string const & n) {
  m_name = n;
}

Object *
Object::owner() {
  return m_parent == nullptr ? nullptr : m_parent->owner();
}

const Object *
Object::owner() const {
  return m_parent == nullptr ? nullptr : m_parent->owner();
}

Object * Object::parent() {
  return m_parent;
}

const Object * Object::parent() const {
  return m_parent;
}

void Object::setParent(const Object * p) {
  m_parent = const_cast<Object *>(p);
};

tree::Path
Object::path(const bool local) const {
  tree::Path result;
  if (m_parent == nullptr) {
    result.push(tree::path::Item::build(tree::path::Item::Type::Global));
  } else if (local) {
    result.push(tree::path::Item::build(tree::path::Item::Type::Local));
    result.push(tree::path::Item::build(tree::path::Item::Type::Named, m_name));
  } else {
    auto item = tree::path::Item::build(tree::path::Item::Type::Named, m_name);
    result = m_parent->path().push(item);
  }
  return result;
}

bool Object::flattenModel() {
  return true;
}

bool Object::validateModel() {
  return true;
}

bool
Object::injectInherited(tree::Object const & r, Object const & o, tree::Value & v) const {
  if (m_parent == nullptr) return false;
  return m_parent->injectInherited(r, o, v);
}

} // namespace model
} // namespace ace

