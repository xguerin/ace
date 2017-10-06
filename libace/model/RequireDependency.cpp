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

#include <ace/model/RequireDependency.h>
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

RequireDependency::RequireDependency() {
  m_schm["require"] = { ace::tree::Value::Type::Array , false };
}

bool
RequireDependency::match(tree::Value const & t) {
  return t.has("require");
}

bool
RequireDependency::checkModel(tree::Value const & t) const {
  if (not Dependency::checkModel(t)) return false;
  auto const & ary = static_cast<tree::Array const &>(t["require"]);
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
RequireDependency::loadModel(tree::Value const & t) {
  auto const & ary = static_cast<tree::Array const &>(t["require"]);
  for (auto & e : ary) {
    std::string ev = static_cast<tree::Primitive const &>(*e).value<std::string>();
    m_deps.insert(ev);
  }
  Dependency::loadModel(t);
}

void
RequireDependency::expandInstance(tree::Object & r, tree::Value & v) {
  Dependency::expandInstance(r, v);
  Body * body = bodyFor(this);
  for (auto & dep : m_deps) {
    tree::Path dest;
    if (buildModelPath(dep, r, dest) and body->has(dest)) {
      INFO("Promote ", dest);
      body->promoteArity(dest);
    }
  }
}

bool
RequireDependency::resolveInstance(tree::Object const & r, tree::Value const & v) const {
  if (not Dependency::resolveInstance(r, v)) return false;
  int score = 0;
  for (auto & dep : m_deps) {
    tree::Path p;
    if (not buildModelPath(dep, r, p)) {
      return false;
    }
    if (not r.has(p)) {
      ERROR(ERR_INSTANCE_PATH_NOT_FOUND(p));
      score += 1;
    }
  }
  return score == 0;
}

int
RequireDependency::priority() const {
  return 5;
}

RequireDependency::Ref
RequireDependency::clone() const {
  return RequireDependency::Ref(new RequireDependency(*this));
}

RequireDependency::operator std::string() const {
  std::ostringstream o;
  const BasicType * type = basicTypeFor(this);
  size_t cur = 0;
  o << "require ";
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

