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

#include <ace/model/Dependency.h>
#include <ace/model/Body.h>
#include <ace/model/Errors.h>
#include <ace/model/ListAttribute.h>
#include <ace/model/Model.h>
#include <ace/common/Algorithms.h>
#include <ace/common/String.h>
#include <ace/types/String.h>
#include <list>
#include <string>

namespace {

ace::model::BasicType *
basicTypeFor(ace::model::Object * o) {
  return static_cast<ace::model::BasicType *>(o->parent()->parent()->parent());
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

tree::Path
Dependency::path(const bool local) const {
  return m_parent->path(local);
}

bool
Dependency::checkModel(tree::Value const & t) const {
  tree::Checker chk(path(), t);
  if (not chk.validate(m_schm)) {
    return false;
  }
  return true;
}

void
Dependency::loadModel(tree::Value const & t) {
}

bool
Dependency::validateModel() {
  BasicType * type = basicTypeFor(this);
  for (auto & dep : m_deps) {
    /**
     * Check if the path is valid
     */
    std::string ev = dep;
    auto c = PlaceHolder;
    std::replace(ev.begin(), ev.end(), c, 'a');
    tree::Path p;
    try {
      p = tree::Path::parse(ev);
    } catch (std::invalid_argument const &) {
      ERROR(ERR_INVALID_PATH_FORMAT(ev));
      return false;
    }
    if (p.global()) {
      ERROR(ERR_NO_GLOBAL_PATH_IN_DEPS);
      return false;
    }
    /**
     * If the dep has a placeholder, check the type
     */
    if (hasPlaceHolder(dep)) {
      if (type->kind() != BasicType::Kind::String) {
        ERROR(ERR_WRONG_TYPE_FOR_VALUE_EXP);
        return false;
      }
      String * st = dynamic_cast<String *>(type);
      if (st != nullptr and not st->hasEitherAttribute()) {
        ERROR(ERR_UNBOUND_VALUE_EXP);
        return false;
      }
    }
  }
  return true;
}

bool
Dependency::checkInstance(tree::Object const & r, tree::Value const & v) const {
  return true;
}

void
Dependency::expandInstance(tree::Object & r, tree::Value & v) {
}

bool
Dependency::flattenInstance(tree::Object & r, tree::Value & v) {
  int score = 0;
  Body * body = bodyFor(this);
  Model * model = modelFor(this);
  for (auto & dep : m_deps) {
    tree::Path dest;
    if (not buildModelPath(dep, dest)) {
      return false;
    }
    if (not body->has(dest)) {
      ERROR(ERR_MODEL_PATH_NOT_FOUND(dest, model->name()));
      score += 1;
    }
  }
  return score == 0;
}

bool
Dependency::resolveInstance(tree::Object const & r, tree::Value const & v) const {
  return true;
}

bool
Dependency::canMerge(Dependency const & o) const {
  return true;
}

void
Dependency::merge(Dependency const & o) {
  for (auto & e : o.m_deps) m_deps.insert(e);
}

bool
Dependency::hasPlaceHolder(std::string const & d) const {
  return d.find(PlaceHolder) != std::string::npos;
}

std::string
Dependency::expandPlaceHolder(std::string const & d, std::string const & v) const {
  return common::String::expand(d, PlaceHolder, v);
}

bool
Dependency::buildModelPath(std::string const & d, tree::Path & r) const {
  if (not d.empty()) try {
    r = tree::Path::parse(d);
  } catch (std::invalid_argument const &) {
    ERROR(ERR_INVALID_PATH_FORMAT(d));
    return false;
  }
  return true;
}

} // namespace model
} // namespace ace

