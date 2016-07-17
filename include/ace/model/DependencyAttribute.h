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

#ifndef ACE_MODEL_DEPENDENCYATTRIBUTE_H_
#define ACE_MODEL_DEPENDENCYATTRIBUTE_H_

#include "Attribute.h"
#include "Dependency.h"
#include "Errors.h"
#include "Generator.h"
#include <ace/tree/Primitive.h>
#include <functional>
#include <list>
#include <set>
#include <sstream>
#include <string>

namespace ace {
namespace model {

/**
  * @brief General case
  *
  * @tparam typename...DEPTYPES
  */
template<typename...DEPTYPES> class DependencyAttribute;

/**
 * @brief Base Case
 */
template<>
class DependencyAttribute<> : public Attribute {
 private:

  struct Comparator : public std::binary_function<Dependency::Ref, Dependency::Ref, bool> {
    bool operator()(Dependency::Ref const & a, Dependency::Ref const & b) const;
  };

 public:

  using Dependencies = std::multiset<Dependency::Ref, Comparator>;

  DependencyAttribute() = delete;
  explicit DependencyAttribute(DependencyAttribute const & o);
  DependencyAttribute(std::string const & name, bool o);

  // Object

  bool checkModel(tree::Value const & t) const;
  void loadModel(tree::Value const & t);
  bool validateModel();

  virtual bool checkDependency(tree::Value const & t) const;
  virtual void loadDependency(tree::Value const & t);

  // Instance

  void load(Attribute const & a);
  bool validate(tree::Object const & r, tree::Value const & v) const;

  // Attribute

  bool merge(Attribute const & b);
  operator tree::Checker::Pattern() const;

  void print(std::ostream & o, int l) const;
  operator std::string() const;

  virtual Attribute::Ref clone() const = 0;

  // Accessors

  Dependencies const & values() const;

 protected:

  Dependencies m_values;
};

/**
 * @brief Generic case
 *
 * @tparam T
 * @tparam typename...DEPTYPES
 */
template<typename T, typename...DEPTYPES>
class DependencyAttribute<T, DEPTYPES...> : public DependencyAttribute<DEPTYPES...> {
 public:

  explicit DependencyAttribute(DependencyAttribute const & o);
  DependencyAttribute(std::string const & name, bool o);

  bool checkDependency(tree::Value const & t) const;
  void loadDependency(tree::Value const & t);

  // Attribute

  virtual Attribute::Ref clone() const;
};

template<typename T, typename...DEPTYPES>
DependencyAttribute<T, DEPTYPES...>::DependencyAttribute(DependencyAttribute const & o)
  : DependencyAttribute<DEPTYPES...>(o) { }

template<typename T, typename...DEPTYPES>
DependencyAttribute<T, DEPTYPES...>::DependencyAttribute(std::string const & n, bool o)
  : DependencyAttribute<DEPTYPES...>(n, o) { }

template<typename T, typename...DEPTYPES>
bool
DependencyAttribute<T, DEPTYPES...>::checkDependency(tree::Value const & t) const {
  if (T::match(t)) {
    T vd;
    vd.setParent(this);
    if (not vd.checkModel(t)) return false;
    return true;
  } else {
    return DependencyAttribute<DEPTYPES...>::checkDependency(t);
  }
}

template<typename T, typename...DEPTYPES>
void
DependencyAttribute<T, DEPTYPES...>::loadDependency(tree::Value const & t) {
  if (T::match(t)) {
    Dependency::Ref dr(new T);
    dr->setParent(this);
    dr->loadModel(t);
    this->m_values.insert(dr);
  } else {
    DependencyAttribute<DEPTYPES...>::loadDependency(t);
  }
}

template<typename T, typename...DEPTYPES>
Attribute::Ref
DependencyAttribute<T, DEPTYPES...>::clone() const {
  return Attribute::Ref(new DependencyAttribute<T, DEPTYPES...>(*this));
}

} // namespace model
} // namespace ace

#endif  // ACE_MODEL_DEPENDENCYATTRIBUTE_H_
