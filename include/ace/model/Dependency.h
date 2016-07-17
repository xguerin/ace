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

#ifndef ACE_MODEL_DEPENDENCY_H_
#define ACE_MODEL_DEPENDENCY_H_

#include "Instance.h"
#include "Object.h"
#include <ace/tree/Checker.h>
#include <memory>
#include <ostream>
#include <set>
#include <string>

namespace ace {
namespace model {

class Dependency : public Object, public Instance {
 public:

   /**
    * @brief Unique reference to an Dependency
    */
  using Ref = std::shared_ptr<Dependency>;

  static constexpr char PlaceHolder = '%'; // NOLINT

 public:

  Dependency() = default;
  explicit Dependency(Dependency const &) = default;
  virtual ~Dependency() { }

  // Object

  tree::Path path() const;

  virtual bool checkModel(tree::Value const & t) const;
  virtual void loadModel(tree::Value const & t);
  virtual bool validateModel();

  // Instance

  virtual bool checkInstance(tree::Object const & r, tree::Value const & v) const;
  virtual void expandInstance(tree::Object & r, tree::Value & v);
  virtual bool flattenInstance(tree::Object & r, tree::Value & v);
  virtual bool resolveInstance(tree::Object const & r, tree::Value const & v) const;

  // Local

  virtual int priority() const = 0;

  virtual Dependency::Ref clone() const = 0;

  virtual bool canMerge(Dependency const & o) const;
  virtual void merge(Dependency const & o);

  virtual operator std::string() const = 0;

 protected:

  bool hasPlaceHolder(std::string const & d) const;
  std::string expandPlaceHolder(std::string const & d, std::string const & v) const;

  bool buildModelPath(std::string const & d, tree::Path & r) const;

  tree::Checker::Schema m_schm;
  std::set<std::string> m_deps;
};

} // namespace model
} // namespace ace

#endif  // ACE_MODEL_DEPENDENCY_H_
