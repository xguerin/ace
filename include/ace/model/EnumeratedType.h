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

#pragma once

#include "ListAttribute.h"
#include "Type.h"
#include <ace/common/String.h>
#include <functional>
#include <list>
#include <string>

namespace ace {
namespace model {

template<typename T, bool O = false, typename C = std::equal_to<T>, typename F = FormatChecker<T>>
class EnumeratedType : public virtual Type<T, F> {
 public:

  using EitherAttributeType = ListAttribute<T, C, O>;

 public:

  EnumeratedType() = delete;
  explicit EnumeratedType(EnumeratedType const &) = default;
  virtual ~EnumeratedType() { }

  virtual bool validateModel();

  bool isEnumerated() const;

  bool checkValueConstraint(std::list<tree::Value::Ref> const & v) const;
  bool constrainByValue(std::list<tree::Value::Ref> const & v);

  virtual bool hasEitherAttribute() const;
  virtual EitherAttributeType const & eitherAttribute() const;

 protected:

  EnumeratedType(const BasicType::Kind k, std::string const & a = "?1+*");
};

template<typename T, bool O, typename C, typename F>
EnumeratedType<T, O, C, F>::EnumeratedType(const BasicType::Kind k, std::string const & a)
    : Type<T, F>(k, a) {
  this->m_attributes.template define<EitherAttributeType>("either", true);
  this->m_attributes.exclude("either", "hook");
  this->m_attributes.exclude("either", "map");
  this->m_attributes.exclude("either", "range");
}

template<typename T, bool O, typename C, typename F>
bool
EnumeratedType<T, O, C, F>::validateModel() {
  if (not Type<T, F>::validateModel()) return false;
  if (not this->hasDefaultAttribute()) return true;
  if (not hasEitherAttribute()) return true;
  if (eitherAttribute().values().empty()) {
    ERROR(ERR_EITHER_EMPTY);
    return false;
  }
  for (auto const & dv : this->defaultValues()) if (not eitherAttribute().check(dv)) {
    ERROR(ERR_EITHER_INVALID_DEFAULT);
    return false;
  }
  return true;
}

template<typename T, bool O, typename C, typename F>
bool
EnumeratedType<T, O, C, F>::isEnumerated() const {
  return true;
}

template<typename T, bool O, typename C, typename F>
bool
EnumeratedType<T, O, C, F>::checkValueConstraint(std::list<tree::Value::Ref> const & v) const {
  EitherAttributeType attr("either", true);
  attr.setParent(this);
  return attr.checkConstraint(v);
}

template<typename T, bool O, typename C, typename F>
bool
EnumeratedType<T, O, C, F>::constrainByValue(std::list<tree::Value::Ref> const & v) {
  if (not hasEitherAttribute()) this->m_attributes.activate("either");
  Attribute & attr = *this->m_attributes["either"];
  return static_cast<EitherAttributeType &>(attr).constrain(v);
}

template<typename T, bool O, typename C, typename F>
bool
EnumeratedType<T, O, C, F>::hasEitherAttribute() const {
  return this->m_attributes.has("either");
}

template<typename T, bool O, typename C, typename F>
typename EnumeratedType<T, O, C, F>::EitherAttributeType const &
EnumeratedType<T, O, C, F>::eitherAttribute() const {
  Attribute const & attr = *this->m_attributes["either"];
  return static_cast<EitherAttributeType const &>(attr);
}

} // namespace model
} // namespace ace
