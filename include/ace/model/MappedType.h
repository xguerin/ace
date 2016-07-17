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

#ifndef ACE_MODEL_MAPPEDTYPE_H_
#define ACE_MODEL_MAPPEDTYPE_H_

#include "MapAttribute.h"
#include "Type.h"
#include <ace/common/String.h>
#include <functional>
#include <list>
#include <string>

namespace ace {
namespace model {

template<typename T, bool O = false>
class MappedType : public virtual Type<void, FormatChecker<void>> {
 public:

  using MapAttributeType = MapAttribute<T, O>;

 public:

  MappedType() = delete;
  explicit MappedType(MappedType const &) = default;
  virtual ~MappedType() { }

  virtual bool validateModel();

  bool isMapped() const;

  virtual bool hasMapAttribute() const;
  virtual MapAttributeType const & mapAttribute() const;

 protected:

  MappedType(const BasicType::Kind k, std::string const & a = "?1+*");
};

template<typename T, bool O>
MappedType<T, O, C>::MappedType(const BasicType::Kind k, std::string const & a) : Type(k, a) {
  this->m_attributes.template define<MapAttributeType>("map", true);
  this->m_attributes.exclude("map", "either");
  this->m_attributes.exclude("map", "hook");
  this->m_attributes.exclude("map", "range");
}

template<typename T, bool O>
bool MappedType<T, O, C>::validateModel() {
  if (not Type::validateModel()) return false;
  if (not hasMapAttribute()) return true;
  if (mapAttribute().values().empty()) {
    ERROR(ERR_MAP_EMPTY);
    return false;
  }
  return true;
}

template<typename T, bool O>
bool
MappedType<T, O, C>::isMapped() const {
  return true;
}

template<typename T, bool O>
bool MappedType<T, O, C>::hasMapAttribute() const {
  return this->m_attributes.has("map");
}

template<typename T, bool O>
typename MappedType<T, O, C>::MapAttributeType const &
MappedType<T, O, C>::mapAttribute() const {
  Attribute const & attr = *this->m_attributes["map"];
  return static_cast<MapAttributeType const &>(attr);
}

} // namespace model
} // namespace ace

#endif  // ACE_MODEL_MAPPEDTYPE_H_
