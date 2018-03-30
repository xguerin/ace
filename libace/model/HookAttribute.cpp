/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without reexaction, including without limitation the rights
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

#include <ace/model/Errors.h>
#include <ace/model/HookAttribute.h>
#include <ace/model/Model.h>

namespace ace {
namespace model {

HookAttribute::HookAttribute(std::string const & n, bool o)
  : Attribute(n, o, false), m_hook() { }

bool HookAttribute::checkModel(tree::Value const & t) const {
  if (t.type() != tree::Value::Type::Object) {
    ACE_LOG(Debug, t.isArray(), " ", t.isPrimitive());
    ERROR(ERR_HOOK_INVALID_TYPE);
    return false;
  }
  auto const & o = static_cast<tree::Object const &>(t);
  if (not Hook::validate(o)) {
    return false;
  }
  return true;
}

void HookAttribute::loadModel(tree::Value const & t) {
  auto const & o = static_cast<tree::Object const &>(t);
  m_hook.load(o);
}

bool
HookAttribute::checkInstance(tree::Object const & r, tree::Value const & v) const {
  return true;
}

bool
HookAttribute::flattenInstance(tree::Object & r, tree::Value & v) {
  Model const & model = *static_cast<const Model *>(owner());
  tree::Path const & p = hook().path();
  if (not model.body().has(p)) {
    ERROR(ERR_INVALID_HOOK_SOURCE(p));
    return false;
  }
  std::list<BasicType::Ref> lasso;
  model.body().get(p, lasso);
  for (auto & e : lasso) {
    if (not e->isEnumerated() and not e->isMapped()) {
      ERROR(ERR_HOOKED_VALUE_NOT_ENUMERATED);
      return false;
    }
    if (e->optional() and not optional()) {
      ERROR(ERR_HOOKED_VALUE_ARITY_MISMATCH(e->path()));
      return false;
    }
  }
  return true;
}

bool
HookAttribute::resolveInstance(tree::Object const & r, tree::Value const & v) const {
  tree::Path const & p = hook().path();
  if (not r.has(p)) {
    ERROR(ERR_NO_HOOKED_VALUE_IN_INSTANCE(hook().path()));
    return false;
  }
  std::set<std::string> mv, hv;
  v.each([&](tree::Value const & w) {
    if (w.type() == tree::Value::Type::Object) {
      tree::Object const & o = static_cast<tree::Object const &>(w);
      for (auto & e : o) mv.insert(e.first);
    } else {
      tree::Primitive const & p = static_cast<tree::Primitive const &>(w);
      mv.insert(p.value());
    }
  });
  r.get(p, [&](tree::Value const & val) {
    val.each([&](tree::Value const & w) {
      if (w.type() == tree::Value::Type::Object) {
        tree::Object const & o = static_cast<tree::Object const &>(w);
        for (auto & e : o) hv.insert(e.first);
      } else {
        tree::Primitive const & p = static_cast<tree::Primitive const &>(w);
        hv.insert(p.value());
      }
    });
  });
  std::set<std::string> txv;
  for (auto & e : hv) {
    if (not hook().match(e)) {
      ERROR(ERR_EMPTY_HOOK_MATCH(name(), e));
      return false;
    }
    std::string to;
    if (not hook().transform(e, to)) {
      ERROR(ERR_EMPTY_HOOK_MATCH(name(), e));
      return false;
    }
    txv.insert(to);
  }
  for (auto & e: mv) if (txv.find(e) == txv.end()) {
    ERROR(ERR_NO_HOOKED_VALUE_MATCH(e));
    return false;
  }
  if (m_hook.exact()) {
    for (auto & e: txv) if (mv.count(e) == 0) {
      ERROR(ERR_MISSING_HOOKED_VALUE_MATCH(e));
      return false;
    }
  }
  return true;
}

void
HookAttribute::load(Attribute const & a) {
  HookAttribute const & ra = static_cast<HookAttribute const &>(a);
  m_hook = ra.m_hook;
}

bool
HookAttribute::merge(Attribute const & b) {
  if (not Attribute::merge(b)) return false;
  HookAttribute const & rb = static_cast<HookAttribute const &>(b);
  m_hook = rb.m_hook;
  return true;
}

HookAttribute::operator tree::Checker::Pattern() const {
  return tree::Checker::Pattern(tree::Value::Type::Object, true);
}

HookAttribute::operator std::string() const {
  std::ostringstream oss;
  oss << m_hook.path().toString();
  return oss.str();
}

Attribute::Ref HookAttribute::clone() const {
  return Attribute::Ref(new HookAttribute(*this));
}

Hook const &
HookAttribute::hook() const {
  return m_hook;
}

} // namespace model
} // namespace ace
