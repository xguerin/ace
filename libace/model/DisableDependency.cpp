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

#include <ace/model/DisableDependency.h>
#include <ace/model/Body.h>
#include <ace/model/Errors.h>
#include <ace/model/ListAttribute.h>
#include <ace/model/Model.h>
#include <ace/common/Algorithms.h>
#include <ace/common/String.h>
#include <ace/tree/Checker.h>
#include <ace/types/String.h>
#include <list>
#include <string>

namespace {

const ace::model::BasicType *
basicTypeFor(const ace::model::Object * o) {
  return static_cast<const ace::model::BasicType *>(o->parent()->parent()->parent());
}

ace::model::Model *
modelFor(ace::model::Object * o) {
  return static_cast<ace::model::Model *>(o->owner());
}

ace::model::Body *
bodyFor(ace::model::Object * o) {
  return & modelFor(o)->body();
}

} // namespace

namespace ace {
namespace model {

DisableDependency::DisableDependency() {
  m_schm["disable"] = { ace::tree::Value::Type::Array , false };
}

bool
DisableDependency::match(tree::Value const & t) {
  return t.has("disable");
}

bool
DisableDependency::checkModel(tree::Value const & t) const {
  if (not Dependency::checkModel(t)) return false;
  auto const & ary = static_cast<tree::Array const &>(t["disable"]);
  if (ary.size() == 0) {
    ERROR(ERR_EMPTY_REQS);
    return false;
  }
  for (auto & e : ary) if (e->type() != tree::Value::Type::String) {
    ERROR(ERR_WRONG_REQ_TYPE);
    return false;
  }
  return true;
}

void
DisableDependency::loadModel(tree::Value const & t) {
  auto const & ary = static_cast<tree::Array const &>(t["disable"]);
  for (auto & e : ary) {
    m_deps.insert(static_cast<tree::Primitive const &>(*e).value<std::string>());
  }
  Dependency::loadModel(t);
}

void
DisableDependency::expandInstance(tree::Object & r, tree::Value & v) {
  Dependency::expandInstance(r, v);
  Body * body = bodyFor(this);
  for (auto & dep : m_deps) {
    tree::Path dest;
    buildModelPath(dep, r, dest);
    if (body->has(dest)) {
      INFO("Disable ", dest);
      body->disable(dest);
    }
  }
}

bool
DisableDependency::flattenInstance(tree::Object & r, tree::Value & v) {
  int score = 0;
  if (not Dependency::flattenInstance(r, v)) return false;
  Body * body = bodyFor(this);
  for (auto & dep : m_deps) {
    tree::Path dest;
    buildModelPath(dep, r, dest);
    std::list<BasicType::Ref> lasso;
    body->get(dest, lasso);
    for (auto & e : lasso) if (not e->disabled()) {
      ERROR(ERR_FAILED_DISABLE_TYPE(dest));
      score += 1;
    }
  }
  return score == 0;
}

int
DisableDependency::priority() const {
  return 1;
}

DisableDependency::Ref
DisableDependency::clone() const {
  return DisableDependency::Ref(new DisableDependency(*this));
}

DisableDependency::operator std::string() const {
  std::ostringstream o;
  const BasicType * type = basicTypeFor(this);
  size_t cur = 0;
  o << "disable ";
  for (auto & e : m_deps) {
    if (hasPlaceHolder(e)) {
      const String * strType = dynamic_cast<const String *>(type);
      if (strType != nullptr) {
        if (strType->hasEitherAttribute()) {
          o << (std::string)strType->eitherAttribute();
        } else {
          o << "{ instance values }";
        }
      }
    } else {
      o << e;
    }
    if (cur++ < m_deps.size() - 1) o << ", ";
  }
  return o.str();
}

} // namespace model
} // namespace ace

