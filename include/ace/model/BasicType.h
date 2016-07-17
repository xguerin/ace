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

#ifndef ACE_MODEL_BASICTYPE_H_
#define ACE_MODEL_BASICTYPE_H_

#include "ArityAttribute.h"
#include "Attribute.h"
#include "AttributeSet.h"
#include "Coach.h"
#include "Dependency.h"
#include "DependencyAttribute.h"
#include "FlagAttribute.h"
#include "Generator.h"
#include "Hook.h"
#include "Instance.h"
#include "Object.h"
#include <ace/common/String.h>
#include <ace/tree/Array.h>
#include <ace/tree/Primitive.h>
#include <functional>
#include <list>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>

namespace ace {
namespace model {

class BasicType : public Object, public Instance, public Coach, public Generator {
 public:

   /**
    * @brief The "kind" attribute type
    */
  using KindAttributeType = FlagAttribute<std::string>;

  /**
   * @brief The specific kind of type
   */
  enum class Kind {
    Boolean,
    CPUID,
    Class,
    Enum,
    Float,
    File,
    Integer,
    IPv4,
    Link,
    MAC,
    Plugin,
    Selector,
    String,
    URI,
    Undefined,
  };

  /**
   * @brief The "arity" attribute type
   */
  using ArityAttributeType = ArityAttribute;

  /**
   * @brief The "hook" attribute type
   */
  using HookAttributeType = FlagAttribute<std::string>;

  /**
   * @brief The "inherit" attribute type
   */
  using InheritAttributeType = FlagAttribute<bool, true>;

  /**
   * @brief The "doc" attribute type
   */
  using DocAttributeType = FlagAttribute<std::string, true>;

  /**
   * @brief The "doc" attribute type
   */
  using DeprecatedAttributeType = FlagAttribute<std::string>;

  /**
   * @brief The RC-compatible pointer to BasicType
   */
  using Ref = std::shared_ptr<BasicType>;

  /**
   * @brief Generic type for dependency attribute
   */
  using DepsAttributeType = DependencyAttribute<Dependency>;

  /**
   * @brief foreach callback
   */
  using Callback = std::function<void(tree::Value const &)>;

 public:

  BasicType() = delete;
  virtual ~BasicType() { }

  // Object

  void setName(std::string const & n);

  virtual bool checkModel(tree::Value const & t) const;
  virtual void loadModel(tree::Value const & t);
  virtual bool flattenModel();
  virtual bool validateModel();

  // Instance

  virtual bool checkInstance(tree::Object const & r, tree::Value const & v) const;
  virtual void expandInstance(tree::Object & r, tree::Value & v);
  virtual bool flattenInstance(tree::Object & r, tree::Value & v);
  virtual bool resolveInstance(tree::Object const & r, tree::Value const & v) const;

  // Coach

  virtual void display(Coach::Branch const & br) const;
  virtual bool explain(tree::Path const & p, tree::Path::const_iterator const & i) const;

  // Generator

  virtual void collectModelFileDependencies(std::set<std::string> & d) const;

  virtual void collectInterfaceIncludes(std::set<std::string> & i) const;
  virtual void collectImplementationIncludes(std::set<std::string> & i) const;

  virtual bool hasPrivateNamespaceDefinition() const;
  virtual void doPrivateNamespaceDefinition(std::ostream & o, int l) const;

  virtual bool hasTypeDeclaration() const;
  virtual void doTypeDeclaration(std::ostream & o, int l) const;
  virtual void doTypeDefinition(std::ostream & o, int l) const;

  virtual void doBuildDefinition(std::string const & e, std::ostream & o, int l) const;
  virtual void doBuildDefinition(std::string const & s, std::string const & v,
                                 std::string const & e, std::ostream & o, int l) const;

  virtual void doSerializerDefinition(std::string const & c, std::string const & n,
                                      std::ostream & o, int l) const;
  virtual void doSerializerDefinition(std::string const & c, std::string const & n,
                                      std::string const & v, const bool b,
                                      std::ostream & o, int l) const;

  virtual void doCheckerInterface(std::ostream & o, int l = 0) const;
  virtual void doCheckerDeclaration(std::ostream & o, int l = 0) const;
  virtual void doCheckerDefinition(std::ostream & o, int l = 0) const;

  virtual void doGetterInterface(std::ostream & o, int l = 0) const;
  virtual void doGetterDeclaration(std::ostream & o, int l = 0) const;
  virtual void doGetterDefinition(std::ostream & o, int l = 0) const;

  // Basic type

  /**
   * @brief   Merge compatible types together
   * @param b another type
   * @return  true in case of success
   */
  virtual bool merge(BasicType const & b);

  /**
   * @brief Override this type with another type
   * @param b another type
   * @return true in case of success
   */
  virtual bool override(BasicType const & b);

  /**
   * @brief   Get the default value for a primitive type
   * @param d the return default value
   * @return  true if the type has a default value
   */
  virtual bool injectDefault(tree::Object const & r, tree::Value & v) const;

  /**
   * @brief   Call the callback on each occurance of the type
   * @param r the root object
   * @param v the type instance
   * @param o the callback
   */
  void foreach(tree::Object const & r, Callback o) const;

  /**
   * @brief   Clone a basic type with a new name
   * @param n the new name
   * @return  a clone of the type
   */
  virtual BasicType::Ref clone(std::string const & n) const = 0;

  /**
   * @brief   Return the c++ name of the type
   * @return  the c++ name of the type
   */
  virtual std::string typeName() const = 0;

  /**
   * @brief   Decorate the type definition depending on the type's arity
   * @param n the type definition
   * @return  the decorated type
   */
  std::string decorateType(std::string const & n) const;

  /**
   * @brief Return whether the type has been disabled
   * @return true of the type has been disabled
   */
  bool disabled() const;

  /**
   * @brief   Return whether the arity makes the type optional
   * @return  true if the type is optional
   */
  bool optional() const;

  /**
   * @brief   Return whether the arity makes the type required
   * @return  true if the type is required
   */
  bool required() const;

  /**
   * @brief   Return whether the arity makes the type multiple
   * @return  true if the type is multiple
   */
  bool multiple() const;

  /**
   * @brief   Check if the type can resolve a given path
   * @param p the path to resolve
   * @param i the current position in the path
   * @return  true in case of success
   */
  virtual bool has(tree::Path const & p, tree::Path::const_iterator const & i) const;

  /**
   * @brief   Get a sub-element by path
   * @param p the full path
   * @param i the current iterator
   * @return  a reference to the element
   */
  virtual void get(tree::Path const & p, tree::Path::const_iterator const & i,
                   std::list<BasicType::Ref> & r) const;

  /**
   * @brief   Promote the arity of the type using a path object
   * @param p the path object
   * @param i the position in the path
   */
  virtual void promoteArity(tree::Path const & p, tree::Path::const_iterator const & i);

  /**
   * @brief Disable the type using a apth object
   * @param p the path object
   * @param i the position in the path
   */
  virtual void disable(tree::Path const & p, tree::Path::const_iterator const & i);

  /**
   * @brief Returns wether or not the type map to an object
   * @return  true if the type map to an object
   */
  virtual bool isObject() const;

  /**
   * @brief Tells if the type supports enumeration
   * @return true if so
   */
  virtual bool isEnumerated() const;

  /**
   * @brief Tells if the type supports range
   * @return true if so
   */
  virtual bool isRanged() const;

  /**
   * @brief Tells if the type supports map
   * @return true if so
   */
  virtual bool isMapped() const;

  /**
   * @brief Check if a set of constraints is valid
   * @param v the constraints
   * @return true if so
   */
  virtual bool checkValueConstraint(std::list<tree::Value::Ref> const & v) const;

  /**
   * @brief Check if a range constraint is valid
   * @param v the range constraint
   * @return return false
   */
  virtual bool checkRangeConstraint(std::string const & v) const;

  /**
   * @brief Constrain the target type using arbitrary values
   * @param v the values
   * @return true in case of success
   */
  virtual bool constrainByValue(std::list<tree::Value::Ref> const & v);

  /**
   * @brief Constrain the target type using arbitrary range
   * @param v the range
   * @return true in case of success
   */
  virtual bool constrainByRange(std::string const & v);

  // Operators

  /**
   * @brief   Test if two types have the same kind and compatible arities
   * @param o the other type
   * @return  true in case of success, false otherwise
   */
  bool operator<=(BasicType const & o) const;

  /**
   * @brief   Test if two types differ by kind or arities
   * @param o the other type
   * @return  true if they differ, false otherwise
   */
  bool operator>(BasicType const & o) const;

  // Accessors

  std::string const & declName() const;

  Kind kind() const;
  static Kind kindOf(std::string const & s);
  static std::string toString(const Kind k);
  static std::string toTypeString(const Kind k);

  Arity & arity();
  Arity const & arity() const;

  bool mayInherit() const;

  bool hasHook() const;
  Hook const & hook() const;

  bool hasDependencies() const;
  DepsAttributeType::Dependencies const & dependencies() const;

 private:

  bool isDeprecated() const;
  std::string const & deprecatedMessage() const;

 protected:

  BasicType(const Kind k, std::string const & a);
  explicit BasicType(BasicType const & o);

  std::string   m_declName;
  Kind          m_kind;
  std::string   m_arityMap;
  bool          m_mayInherit;
  Hook          m_hook;
  std::string   m_doc;
  AttributeSet  m_attributes;

  friend std::ostream & operator<<(std::ostream & o, BasicType const & k);
};

std::ostream &
operator<<(std::ostream & o, ace::model::BasicType const & k);

std::ostream &
operator<<(std::ostream & o, const BasicType::Kind k);

} // namespace model
} // namespace ace

#endif  // ACE_MODEL_BASICTYPE_H_
