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

#include <ace/tree/Object.h>
#include <ace/tree/Array.h>
#include <ace/tree/Primitive.h>
#include <ace/common/Log.h>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>

namespace ace {
namespace tree {

Object::Object(std::string const & n)
    : Value(n, Type::Object), m_content() { }

Object::Ref
Object::build(std::string const & n) {
  return Ref(new Object(n));
}

void
Object::merge(Value const & o) {
  if (o.type() != Value::Type::Object) return;
  Object const & obj = dynamic_cast<Object const &>(o);
  for (auto & e : obj) if (m_content.count(e.first) == 0) {
    m_content.insert(e);
  } else {
    m_content[e.first]->merge(*e.second);
  }
}

bool
Object::has(std::string const & k) const {
  return m_content.find(k) != m_content.end();
}

bool
Object::has(Path const & p, Path::const_iterator const & i) const {
  if (i == p.end()) return true;
  size_t success = 0;
  switch ((*i)->type()) {
    case path::Item::Type::Named: {
      if (m_content.count((*i)->value()) != 0) {
        success += m_content.at((*i)->value())->has(p, p.down(i)) ? 1 : 0;
      }
    } break;
    case path::Item::Type::Any: {
      if (not m_content.empty()) for (auto & e : m_content) {
        success += e.second->has(p, p.down(i)) ? 1 : 0;
      } else {
        success += 1;
      }
    } break;
    default: break;
  }
  if ((*i)->recursive()) for (auto & e : m_content) {
    success += e.second->has(p, i) ? 1 : 0;
  }
  return success != 0;
}

size_t
Object::size() const {
  return m_content.size();
}

Value::Ref const &
Object::at(std::string const & k) const  {
  if (not has(k)) throw std::invalid_argument(k + ": no such key");
  return m_content.at(k);
}

void
Object::put(Value::Ref const & r) {
  m_content[r->name()] = r;
  r->m_parent = this;
}

void
Object::put(std::string const & k, Value::Ref const & r) {
  r->setName(k);
  r->m_parent = this;
  m_content[k] = r;
}

bool
Object::put(Path const & p, Value::Ref const & r) {
  return put(p, ++p.begin(), r);
}

Value &
Object::operator[](std::string const & k) {
  return get(k);
}

Value const &
Object::operator[](std::string const & k) const  {
  return get(k);
}

Value &
Object::get(std::string const & k) {
  if (not has(k)) throw std::invalid_argument(k + ": no such key");
  return *m_content.at(k);
}

Value const &
Object::get(std::string const & k) const  {
  if (not has(k)) throw std::invalid_argument(k + ": no such key");
  return *m_content.at(k);
}

void
Object::get(Path const & p, Path::const_iterator const & i, std::vector<Value::Ref> & r) {
  if (i == p.end()) return;
  switch ((*i)->type()) {
    case path::Item::Type::Named: {
      if (m_content.count((*i)->value()) != 0) {
        if (p.down(i) == p.end()) {
          r.push_back(m_content.at((*i)->value()));
        } else {
          m_content.at((*i)->value())->get(p, p.down(i), r);
        }
      }
    } break;
    case path::Item::Type::Any: {
      for (auto & e : m_content) if (p.down(i) == p.end()) {
        r.push_back(e.second);
      } else {
        e.second->get(p, p.down(i), r);
      }
    } break;
    default: break;
  }
  if ((*i)->recursive()) for (auto & e : m_content) {
    e.second->get(p, i, r);
  }
}

void
Object::get(Path const & p, Path::const_iterator const & i, std::vector<Value::Ref> & r) const {
  if (i == p.end()) return;
  switch ((*i)->type()) {
    case path::Item::Type::Named: {
      if (m_content.count((*i)->value()) != 0) {
        if (p.down(i) == p.end()) {
          r.push_back(m_content.at((*i)->value()));
        } else {
          m_content.at((*i)->value())->get(p, p.down(i), r);
        }
      }
    } break;
    case path::Item::Type::Any: {
      for (auto & e : m_content) if (p.down(i) == p.end()) {
        r.push_back(e.second);
      } else {
        e.second->get(p, p.down(i), r);
      }
    } break;
    default: break;
  }
  if ((*i)->recursive()) for (auto & e : m_content) {
    e.second->get(p, i, r);
  }
}

void
Object::erase(std::string const & k) {
  if (not has(k)) throw std::invalid_argument(k + ": no such key");
  m_content.erase(k);
}

void
Object::erase(Path const & p, Path::const_iterator const & i) {
  if (i == p.end()) return;
  switch ((*i)->type()) {
    case path::Item::Type::Named: {
      if (m_content.count((*i)->value()) != 0) {
        if (p.down(i) == p.end()) {
          m_content.erase((*i)->value());
        } else {
          m_content.at((*i)->value())->erase(p, p.down(i));
        }
      }
    } break;
    case path::Item::Type::Any: {
      if (p.down(i) == p.end()) {
        m_content.clear();
      } else {
        for (auto & e : m_content) e.second->erase(p, p.down(i));
      }
    } break;
    default: break;
  }
  if ((*i)->recursive()) for (auto & e : m_content) {
    e.second->erase(p, i);
  }
}

tree::Object::const_iterator
Object::begin() const {
  return m_content.begin();
}

tree::Object::const_iterator
Object::end() const {
  return m_content.end();
}

Path
Object::path() const {
  Path apath;
  if (m_name.empty()) {
    apath.push(path::Item::build(path::Item::Type::Global));
  } else {
    apath = Value::path();
  }
  return apath;
}

bool
Object::put(Path const & p, Path::const_iterator const & i, Value::Ref const & r) {
  if (i == p.end()) return false;
  if ((*i)->type() != path::Item::Type::Named) {
    ACE_LOG(Error, "Only named path items are supported (", (*i)->toString(), ")");
    return false;
  }
  std::string const & id = (*i)->value();
  if (p.down(i) != p.end()) {
    if (m_content.find(id) == m_content.end()) {
      tree::Object::Ref tmp = Object::build(id);
      put(tmp);
    }
    Value::Ref vr = m_content[id];
    if (vr->type() != Value::Type::Object) {
      ACE_LOG(Error, "Cannot put a value to \"", p, "\", ", id, " is not an object");
      return false;
    }
    Object::Ref oref = std::static_pointer_cast<Object>(vr);
    return oref->put(p, p.down(i), r);
  } else {
    if (r == nullptr) {
      m_content.erase(id);
    } else if (m_content.find(id) == m_content.end()) {
      put(id, r);
    } else {
      Value::Ref vr = m_content[id];
      if (vr->type() == Value::Type::Array) {
        Array::Ref aref = std::static_pointer_cast<Array>(vr);
        aref->push_back(r);
      } else {
        Array::Ref aref = Array::build(id);
        aref->push_back(vr);
        aref->push_back(r);
        put(id, aref);
      }
    }
    return true;
  }
}

} // namespace tree
} // namespace ace

