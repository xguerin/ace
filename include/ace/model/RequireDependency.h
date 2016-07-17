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

#ifndef ACE_MODEL_REQUIREDEPENDENCY_H_
#define ACE_MODEL_REQUIREDEPENDENCY_H_

#include "Dependency.h"
#include <memory>
#include <ostream>
#include <set>
#include <string>

namespace ace {
namespace model {

class RequireDependency : public Dependency {
 public:

  RequireDependency();
  explicit RequireDependency(RequireDependency const &) = default;
  virtual ~RequireDependency() { }

  // Match

  static bool match(tree::Value const & t);

  // Object

  virtual bool checkModel(tree::Value const & t) const;
  virtual void loadModel(tree::Value const & t);

  // Instance

  virtual void expandInstance(tree::Object & r, tree::Value & v);
  virtual bool resolveInstance(tree::Object const & r, tree::Value const & v) const;

  // Local

  virtual int priority() const;

  virtual RequireDependency::Ref clone() const;
  virtual operator std::string() const;
};

} // namespace model
} // namespace ace

#endif  // ACE_MODEL_REQUIREDEPENDENCY_H_
