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

#ifndef ACE_MODEL_CONSTRAINTGUARD_H_
#define ACE_MODEL_CONSTRAINTGUARD_H_

#include "BasicType.h"
#include "Model.h"
#include <ace/common/Algorithms.h>
#include <ace/common/String.h>
#include <ace/tree/Array.h>
#include <ace/tree/Primitive.h>
#include <functional>
#include <iostream>
#include <list>
#include <set>
#include <string>

namespace ace {
namespace model {

template<typename T, typename D>
class ConstraintGuard : public D {
 public:

  ConstraintGuard();

  bool checkModel(tree::Value const & t) const;
  void loadModel(tree::Value const & t);

  bool flattenInstance(tree::Object & r, tree::Value & v);

  typename D::Ref clone() const;
  operator std::string() const;

 private:

  bool check(tree::Path const & p);
  bool apply(tree::Path const & p);

  std::list<tree::Value::Ref> m_either;
  std::string                 m_range;
};

template<typename T, typename D>
ConstraintGuard<T, D>::ConstraintGuard() : D() {
  this->m_schm["either"] = { tree::Value::Type::Array, true };
  this->m_schm["range"] = { tree::Value::Type::String, true };
}

template<typename T, typename D>
bool
ConstraintGuard<T, D>::checkModel(tree::Value const & t) const {
  if (not D::checkModel(t)) return false;
  if (t.has("either") and t.has("range")) {
    ERROR(ERR_CONSTRAIN_RANGE_OR_EITHER);
    return false;
  }
  if (t.has("either")) {
    auto const & ary = static_cast<tree::Array const &>(t["either"]);
    if (ary.size() == 0) {
      ERROR(ERR_CONSTRAIN_EMPTY_LIST);
      return false;
    }
    for (auto const & e : ary) if (not e->isPrimitive()) {
      ERROR(ERR_CONSTRAIN_INVALID_TYPE);
      return false;
    }
  } else if (t.has("range") and
             static_cast<tree::Primitive const &>(t["range"]).value<std::string>().empty()) {
    ERROR(ERR_CONSTRAIN_EMPTY_RANGE);
    return false;
  }
  return true;
}

template<typename T, typename D>
void
ConstraintGuard<T, D>::loadModel(tree::Value const & t) {
  D::loadModel(t);
  if (t.has("either")) for (auto & vr : static_cast<tree::Array const &>(t["either"])) {
    m_either.push_back(vr);
  } else if (t.has("range")) {
    m_range = static_cast<tree::Primitive const &>(t["range"]).value<std::string>();
  }
}

template<typename T, typename D>
bool
ConstraintGuard<T, D>::flattenInstance(tree::Object & r, tree::Value & v) {
  for (auto & dep : this->m_deps) {
    tree::Path tgtPath;
    if (not this->buildModelPath(dep, r, tgtPath)) return false;
    if (not check(tgtPath)) return false;
    if (not apply(tgtPath)) return false;
  }
  return true;
}

template<typename T, typename D>
typename D::Ref
ConstraintGuard<T, D>::clone() const {
  return typename D::Ref(new ConstraintGuard<T, D>(*this));
}

template<typename T, typename D>
ConstraintGuard<T, D>::operator std::string() const {
  std::ostringstream oss;
  oss << D::operator std::string ();
  if (not m_either.empty()) {
    oss << ", constrained to [ ";
    size_t i = 0;
    for (auto & e : m_either) {
      oss << static_cast<tree::Primitive const &>(*e).value();
      if (i++ < m_either.size() -1) oss << ", ";
    }
    oss << " ]";
  } else if (not m_range.empty()) {
    oss << ", constrained to ";
    oss << m_range;
  }
  return oss.str();
}

template<typename T, typename D>
bool
ConstraintGuard<T, D>::check(tree::Path const & p) {
  Model * model = static_cast<ace::model::Model *>(this->owner());
  if (not model->body().has(p)) {
    ERROR(ERR_MODEL_PATH_NOT_FOUND(p, model->name()));
    return false;
  }
  std::list<BasicType::Ref> lasso;
  model->body().get(p, lasso);
  for (auto & v: lasso) if (not m_either.empty()) {
    if (not v->isEnumerated()) {
      ERROR(ERR_CONSTRAIN_TGT_NOT_ENUM(v->path()));
      return false;
    }
    return v->checkValueConstraint(m_either);
  } else if (not m_range.empty()) {
    if (not v->isRanged()) {
      ERROR(ERR_CONSTRAIN_TGT_NOT_RANG(v->path()));
      return false;
    }
    return v->checkRangeConstraint(m_range);
  }
  return true;
}

template<typename T, typename D>
bool
ConstraintGuard<T, D>::apply(tree::Path const & p) {
  Model * model = static_cast<ace::model::Model *>(this->owner());
  std::list<BasicType::Ref> lasso;
  model->body().get(p, lasso);
  for (auto & v: lasso) if (not m_either.empty()) {
    if (not v->isEnumerated()) {
      ERROR(ERR_CONSTRAIN_TGT_NOT_ENUM(v->path()));
      return false;
    }
    INFO("Constrain ", v->path(), " by value");
    return v->constrainByValue(m_either);
  } else if (not m_range.empty()) {
    if (not v->isRanged()) {
      ERROR(ERR_CONSTRAIN_TGT_NOT_RANG(v->path()));
      return false;
    }
    INFO("Constrain ", v->path(), " by range");
    return v->constrainByRange(m_range);
  }
  return true;
}

} // namespace model
} // namespace ace

#endif  // ACE_MODEL_CONSTRAINTGUARD_H_
