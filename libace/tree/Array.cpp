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

#include <ace/tree/Array.h>
#include <ace/tree/Object.h>
#include <ace/tree/Primitive.h>
#include <ace/common/String.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace ace {
namespace tree {

Array::Array(std::string const & n)
    : Value(n, Type::Array), m_content() { }

Array::Ref
Array::build(std::string const & n) {
  return Ref(new Array(n));
}

void
Array::merge(Value const & o) {
  if (o.type() != Value::Type::Array) return;
  Array const & ary = dynamic_cast<Array const &>(o);
  size_t index = m_content.size();
  for (auto & e : ary) {
    e->setName(std::to_string(index));
    m_content.push_back(e);
    index += 1;
  }
}

void
Array::each(Callback c) {
  for (auto & e : m_content) c(*e);
}

void
Array::each(ConstCallback c) const {
  for (auto & e : m_content) c(*e);
}

bool
Array::has(std::string const & k) const {
  if (k.empty()) return false;
  size_t i = atol(k.c_str());
  if (i == 0 and k != "0") return false;
  return i < m_content.size();
}

bool
Array::has(Path const & p, Path::const_iterator const & i) const {
  if (i == p.end()) return true;
  size_t success = 0;
  switch ((*i)->type()) {
    case path::Item::Type::Indexed: {
      for (auto & idx : (*i)->indexes()) if (idx < m_content.size()) {
        success += m_content[idx]->has(p, p.down(i)) ? 1 : 0;
      }
    } break;
    case path::Item::Type::Ranged: {
      auto const & r = (*i)->range();
      if (r.low < m_content.size() and r.high <= m_content.size()) {
        for (size_t idx = r.low; idx < r.high; idx += r.steps) {
          success += m_content[idx]->has(p, p.down(i)) ? 1 : 0;
        }
      }
    } break;
    case path::Item::Type::Any: {
      if (not m_content.empty()) for (auto & idx : m_content) {
        success += idx->has(p, p.down(i)) ? 1 : 0;
      } else {
        success += 1;
      }
    } break;
    default: break;
  }
  if ((*i)->recursive()) for (auto & e : m_content) {
    success += e->has(p, i) ? 1 : 0;
  }
  return success != 0;
}

size_t
Array::size() const {
  return m_content.size();
}

Value::Ref const &
Array::at(const size_t idx) const {
  return m_content[idx];
}

Value &
Array::operator[](std::string const & k) {
  return get(k);
}

Value const &
Array::operator[](std::string const & k) const  {
  return get(k);
}

Value &
Array::get(std::string const & v) {
  if (not has(v)) throw std::invalid_argument(v + ": invalid index");
  int i = atoi(v.c_str());
  return *m_content[i];
}

Value const &
Array::get(std::string const & v) const {
  if (not has(v)) throw std::invalid_argument(v + ": invalid index");
  int i = atoi(v.c_str());
  return *m_content[i];
}

void
Array::get(Path const & p, Path::const_iterator const & i, std::vector<Value::Ref> & r) {
  if (i == p.end()) return;
  switch ((*i)->type()) {
    case path::Item::Type::Indexed: {
      for (auto & idx : (*i)->indexes()) if (idx < m_content.size()) {
        if (p.down(i) == p.end()) {
          r.push_back(m_content[idx]);
        } else {
          m_content[idx]->get(p, p.down(i), r);
        }
      }
    } break;
    case path::Item::Type::Ranged: {
      auto const & rng = (*i)->range();
      if (rng.low < m_content.size() and rng.high <= m_content.size()) {
        for (size_t idx = rng.low; idx < rng.high; idx += rng.steps) {
          if (p.down(i) == p.end()) {
            r.push_back(m_content[idx]);
          } else {
            m_content[idx]->get(p, p.down(i), r);
          }
        }
      }
    } break;
    case path::Item::Type::Any: {
      for (auto & e : m_content) if (p.down(i) == p.end()) {
        r.push_back(e);
      } else {
        e->get(p, p.down(i), r);
      }
    } break;
    default: break;
  }
  if ((*i)->recursive()) for (auto & e : m_content) {
    e->get(p, i, r);
  }
}

void
Array::get(Path const & p, Path::const_iterator const & i, std::vector<Value::Ref> & r) const {
  if (i == p.end()) return;
  switch ((*i)->type()) {
    case path::Item::Type::Indexed: {
      for (auto & idx : (*i)->indexes()) if (idx < m_content.size()) {
        if (p.down(i) == p.end()) {
          r.push_back(m_content[idx]);
        } else {
          m_content[idx]->get(p, p.down(i), r);
        }
      }
    } break;
    case path::Item::Type::Ranged: {
      auto const & rng = (*i)->range();
      if (rng.low < m_content.size() and rng.high <= m_content.size()) {
        for (size_t idx = rng.low; idx < rng.high; idx += rng.steps) {
          if (p.down(i) == p.end()) {
            r.push_back(m_content[idx]);
          } else {
            m_content[idx]->get(p, p.down(i), r);
          }
        }
      }
    } break;
    case path::Item::Type::Any: {
      for (auto & e : m_content) if (p.down(i) == p.end()) {
        r.push_back(e);
      } else {
        e->get(p, p.down(i), r);
      }
    } break;
    default: break;
  }
  if ((*i)->recursive()) for (auto & e : m_content) {
    e->get(p, i, r);
  }
}

void
Array::erase(std::string const & v) {
  if (not has(v)) return;
  size_t idx = atoi(v.c_str());
  m_content.erase(m_content.begin() + idx);
  for (; idx < m_content.size(); idx += 1) {
    m_content[idx]->setName(std::to_string(idx));
  }
}

void
Array::erase(Path const & p, Path::const_iterator const & i) {
  if (i == p.end()) return;
  switch ((*i)->type()) {
    case path::Item::Type::Indexed: {
      if (p.down(i) == p.end()) {
        for (auto & idx : (*i)->indexes()) if (idx < m_content.size()) {
          m_content[idx] = nullptr;
        }
        Content temp;
        for (auto & e : m_content) if (e != nullptr) {
          temp.push_back(e);
        }
        m_content = temp;
      } else {
        for (auto & idx : (*i)->indexes()) if (idx < m_content.size()) {
            m_content[idx]->erase(p, p.down(i));
          }
        }
    } break;
    case path::Item::Type::Ranged: {
      auto const & rng = (*i)->range();
      if (rng.low < m_content.size() and rng.high <= m_content.size()) {
        for (size_t idx = rng.low; idx < rng.high; idx += rng.steps) {
          if (p.down(i) == p.end()) {
            m_content.erase(m_content.begin() + idx);
          } else {
            m_content[idx]->erase(p, p.down(i));
          }
        }
      }
    } break;
    case path::Item::Type::Any: {
      if (p.down(i) == p.end()) {
        m_content.clear();
      } else {
        for (auto & e : m_content) e->erase(p, p.down(i));
      }
    } break;
    default: break;
  }
  if ((*i)->recursive()) for (auto & e : m_content) {
    e->erase(p, i);
  }
}

void
Array::push_back(Value::Ref const & r) {
  size_t index = m_content.size();
  r->setName(std::to_string(index));
  r->m_parent = this;
  m_content.push_back(r);
}

tree::Array::const_iterator
Array::begin() const {
  return m_content.begin();
}

tree::Array::const_iterator
Array::end() const {
  return m_content.end();
}

} // namespace tree
} // namespace ace

