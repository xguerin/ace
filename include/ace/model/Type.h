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

#ifndef ACE_MODEL_TYPE_H_
#define ACE_MODEL_TYPE_H_

#include "BasicType.h"
#include "ConstraintGuard.h"
#include "DependencyAttribute.h"
#include "DisableDependency.h"
#include "Errors.h"
#include "FlagAttribute.h"
#include "FormatChecker.h"
#include "RequireDependency.h"
#include "ValueGuard.h"
#include <ace/engine/Master.h>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace ace {
namespace model {

// Generic templated Type

template<typename T, typename F = FormatChecker<T>>
class Type : public BasicType {
 public:

  using DefaultAttributeType = FlagAttribute<T, true>;
  using DisableDependencyType = ValueGuard<T, DisableDependency>;
  using RequireDependencyType = ValueGuard<T, ConstraintGuard<T, RequireDependency> >;
  using DepsAttributeType = DependencyAttribute<RequireDependencyType, DisableDependencyType>;

  Type() = delete;
  explicit Type(Type const & o);
  virtual ~Type() { }

  virtual void loadModel(tree::Value const & t);

  virtual bool checkInstance(tree::Object const & r, tree::Value const & v) const;
  using BasicType::expandInstance;
  using BasicType::flattenInstance;
  using BasicType::resolveInstance;

  bool hasDefaultAttribute() const;
  DefaultAttributeType & defaultAttribute();
  DefaultAttributeType const & defaultAttribute() const;
  std::set<T> defaultValues() const;

  virtual bool injectDefault(tree::Object const & r, tree::Value & v) const;
  std::vector<T> values(tree::Object const & r) const;

 protected:

  Type(const Kind k, std::string const & a = "?1+*");

  virtual std::string typeName() const;

 private:

  F     m_formatter;
};


template<typename T, typename F>
Type<T, F>::Type(Type const & o) : BasicType(o), m_formatter(this) { }

template<typename T, typename F>
Type<T, F>::Type(const BasicType::Kind k, std::string const & a)
  : BasicType(k, a), m_formatter(this) {
  m_attributes.define<DefaultAttributeType>("default", true);
  m_attributes.define<DepsAttributeType>("deps", true);
  m_attributes.exclude("default", "hook");
}

template<typename T, typename F>
void
Type<T, F>::loadModel(tree::Value const & t) {
  BasicType::loadModel(t);
  if (hasDefaultAttribute()) defaultAttribute().setMultiple(multiple());
}

template<typename T, typename F>
bool
Type<T, F>::checkInstance(tree::Object const & r, tree::Value const & v) const {
  if (not BasicType::checkInstance(r, v)) return false;
  if (not m_formatter(r, v)) return false;
  return true;
}

template<typename T, typename F>
bool
Type<T, F>::hasDefaultAttribute() const {
  return this->m_attributes.has("default");
}

template<typename T, typename F>
typename Type<T, F>::DefaultAttributeType &
Type<T, F>::defaultAttribute() {
  Attribute & attr = *this->m_attributes["default"];
  return static_cast<DefaultAttributeType &>(attr);
}

template<typename T, typename F>
typename Type<T, F>::DefaultAttributeType const &
Type<T, F>::defaultAttribute() const {
  Attribute const & attr = *this->m_attributes["default"];
  return static_cast<DefaultAttributeType const &>(attr);
}

template<typename T, typename F>
std::set<T>
Type<T, F>::defaultValues() const {
  return defaultAttribute().values();
}

template<typename T, typename F>
bool
Type<T, F>::injectDefault(tree::Object const & r, tree::Value & v) const {
  if (not hasDefaultAttribute()) return false;
  tree::Object & obj = static_cast<tree::Object &>(v);
  auto md = defaultAttribute().materialize();
  md->setName(m_name);
  obj.put(md);
  std::ostringstream oss;
  oss << std::boolalpha << static_cast<std::string>(defaultAttribute());
  DEBUG("Inject default value");
  MASTER.pushDefaulted(path(), oss.str());
  return true;
}

template<typename T, typename F>
std::vector<T>
Type<T, F>::values(tree::Object const & r) const {
  std::vector<T> result;
  tree::Value const & v = r.get(m_name);
  v.each([&](tree::Value const & w) {
    tree::Primitive const & spr = static_cast<tree::Primitive const &>(w);
    result.push_back(spr.value<T>());
  });
  return result;
}

template<typename T, typename F>
std::string
Type<T, F>::typeName() const {
  return Generator::nameFor<T>();
}

// void-specialized templated Type

template<typename F>
class Type<void, F> : public BasicType {
 public:

  using DisableDependencyType = DisableDependency;
  using RequireDependencyType = ConstraintGuard<void, RequireDependency>;
  using DepsAttributeType = DependencyAttribute<RequireDependencyType, DisableDependencyType>;

  Type() = delete;
  explicit Type(Type const &) = default;
  virtual ~Type() { }

  virtual bool checkInstance(tree::Object const & r, tree::Value const & v) const;
  using BasicType::expandInstance;
  using BasicType::flattenInstance;
  using BasicType::resolveInstance;

 protected:

  Type(const Kind k, std::string const & a = "?1+*");

 private:

  F m_formatter;
};

template<typename F>
Type<void, F>::Type(const BasicType::Kind k, std::string const & a)
  : BasicType(k, a), m_formatter(this) {
  m_attributes.define<DepsAttributeType>("deps", true);
}

template<typename F>
bool
Type<void, F>::checkInstance(tree::Object const & r, tree::Value const & v) const {
  if (not BasicType::checkInstance(r, v)) return false;
  if (not m_formatter(r, v)) return false;
  return true;
}

} // namespace model
} // namespace ace

#endif  // ACE_MODEL_TYPE_H_
