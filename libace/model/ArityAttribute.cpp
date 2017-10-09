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

#include <ace/model/ArityAttribute.h>
#include <ace/common/Log.h>
#include <ace/tree/Array.h>
#include <string>
#include <vector>

namespace ace {
namespace model {

ArityAttribute::ArityAttribute(std::string const & n, bool o)
  : Attribute(n, o, false), m_arity() { }

bool ArityAttribute::checkModel(tree::Value const & t) const {
  if (t.type() != tree::Value::Type::String) {
    ERROR(ERR_ARITY_INVALID_TYPE);
    return false;
  }
  Arity r;
  return Arity::parse(static_cast<tree::Primitive const &>(t).value<std::string>(), r);
}

void
ArityAttribute::loadModel(tree::Value const & t) {
  Arity::parse(static_cast<tree::Primitive const &>(t).value<std::string>(), m_arity);
}

void
ArityAttribute::load(Attribute const & a) {
  ArityAttribute const & ra = static_cast<ArityAttribute const &>(a);
  m_arity = ra.m_arity;
}

bool
ArityAttribute::validate(tree::Object const & r, tree::Value const & v) const {
  if (v.type() == tree::Value::Type::Array) {
    auto const & ary = static_cast<tree::Array const &>(v);
    return check(ary.size());
  }
  return check(1);
}

bool
ArityAttribute::check(const size_t v) const {
  return m_arity.check(v);
}

Arity &
ArityAttribute::value() {
  return m_arity;
}

Arity const &
ArityAttribute::value() const {
  return m_arity;
}

bool
ArityAttribute::merge(Attribute const & b) {
  if (not Attribute::merge(b)) return false;
  ArityAttribute const & rb = static_cast<ArityAttribute const &>(b);
  m_arity = m_arity.intersect(rb.m_arity);
  return m_arity.isValid();
}

ArityAttribute::operator tree::Checker::Pattern() const {
  std::vector<tree::Value::Type> types = { tree::Value::Type::String };
  return tree::Checker::Pattern(types, m_optional);
}

ArityAttribute::operator std::string() const {
  std::ostringstream oss;
  oss << m_arity;
  return oss.str();
}

Attribute::Ref ArityAttribute::clone() const {
  return Attribute::Ref(new ArityAttribute(*this));
}

} // namespace model
} // namespace ace

