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

#include "RangeAttribute.h"
#include "Type.h"
#include <ace/common/String.h>
#include <functional>
#include <list>
#include <string>

namespace ace {
namespace model {

template<typename T, bool O = false, typename C = std::less<T>, typename F = FormatChecker<T>>
class RangedType : public virtual Type<T, F> {
 public:

  using RangeAttributeType = RangeAttribute<T, C, O>;

 public:

  RangedType() = delete;
  explicit RangedType(RangedType const &) = default;
  virtual ~RangedType() { }

  virtual bool validateModel();

  bool isRanged() const;

  bool checkRangeConstraint(std::string const & v) const;
  bool constrainByRange(std::string const & v);

  virtual bool hasRangeAttribute() const;
  virtual RangeAttributeType const & rangeAttribute() const;

 protected:

  RangedType(const BasicType::Kind k, std::string const & a = "?1+*");
};

template<typename T, bool O, typename C, typename F>
RangedType<T, O, C, F>::RangedType(const BasicType::Kind k, std::string const & a)
    : Type<T, F>(k, a) {
  this->m_attributes.template define<RangeAttributeType>("range", true);
  this->m_attributes.exclude("range", "either");
  this->m_attributes.exclude("range", "map");
}

template<typename T, bool O, typename C, typename F>
bool
RangedType<T, O, C, F>::validateModel() {
  if (not Type<T, F>::validateModel()) return false;
  if (not this->hasDefaultAttribute()) return true;
  if (not hasRangeAttribute()) return true;
  for (auto const & dv : this->defaultValues()) if (not rangeAttribute().check(dv)) {
    ERROR(ERR_RANGE_INVALID_DEFAULT);
    return false;
  }
  return true;
}

template<typename T, bool O, typename C, typename F>
bool
RangedType<T, O, C, F>::isRanged() const {
  return true;
}

template<typename T, bool O, typename C, typename F>
bool
RangedType<T, O, C, F>::checkRangeConstraint(std::string const & v) const {
  RangeAttributeType attr("range", true);
  attr.setParent(this);
  return attr.checkConstraint(v);
}

template<typename T, bool O, typename C, typename F>
bool
RangedType<T, O, C, F>::constrainByRange(std::string const & v) {
  if (not hasRangeAttribute()) this->m_attributes.activate("range");
  Attribute & attr = *this->m_attributes["range"];
  return static_cast<RangeAttributeType &>(attr).constrain(v);
}

template<typename T, bool O, typename C, typename F>
bool
RangedType<T, O, C, F>::hasRangeAttribute() const {
  return this->m_attributes.has("range");
}

template<typename T, bool O, typename C, typename F>
typename RangedType<T, O, C, F>::RangeAttributeType const &
RangedType<T, O, C, F>::rangeAttribute() const {
  Attribute const & attr = *this->m_attributes["range"];
  return static_cast<RangeAttributeType const &>(attr);
}

} // namespace model
} // namespace ace
