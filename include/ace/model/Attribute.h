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

#ifndef ACE_MODEL_ATTRIBUTE_H_
#define ACE_MODEL_ATTRIBUTE_H_

#include "Instance.h"
#include "Object.h"
#include <ace/tree/Checker.h>
#include <ace/tree/Value.h>
#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <utility>

namespace ace {
namespace model {

// Existence comparator

template<typename T>
struct TrueCompare : public std::binary_function<T, T, bool> {
  bool operator() (T const & a, T const & b) const;
};

template<typename T>
bool
TrueCompare<T>::operator() (T const & a, T const & b) const {
  return true;
}

// Attribute

class Attribute : public Object {
 public:

   /**
    * @brief Unique reference to an Attribute
    */
  using Ref = std::shared_ptr<Attribute>;

  Attribute() = delete;
  explicit Attribute(Attribute const &) = default;
  virtual ~Attribute() { }

  tree::Path path() const;

  using Object::loadModel;
  using Object::checkModel;

  virtual void load(Attribute const & a) = 0;
  virtual bool validate(tree::Object const & r, tree::Value const & v) const = 0;

  virtual bool merge(Attribute const & b);
  virtual bool override(Attribute const & b);

  virtual operator tree::Checker::Pattern() const = 0;

  virtual void print(std::ostream & o, int l) const;
  virtual operator std::string() const = 0;

  virtual Attribute::Ref clone() const = 0;

  bool optional() const;
  bool overridable() const;

 protected:

  Attribute(std::string const & n, bool opt, bool ovr = false);

  bool  m_optional;
  bool  m_override;
};

} // namespace model
} // namespace ace

#endif  // ACE_MODEL_ATTRIBUTE_H_