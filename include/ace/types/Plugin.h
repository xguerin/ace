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

#include <ace/model/FlagAttribute.h>
#include <ace/model/FormatChecker.h>
#include <ace/model/Model.h>
#include <ace/types/Class.h>
#include <functional>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace ace { namespace model {

class Plugin : public Type<void, FormatChecker<void>>
{
public:
  using ModelAttributeType = FlagAttribute<std::string>;

  Plugin();

  bool match(tree::Path const& path) const;
  Class const& getClassFor(tree::Path const& path) const;

  // Object

  bool checkModel(tree::Value const& t) const;
  void loadModel(tree::Value const& t);
  bool flattenModel();
  bool validateModel();

  bool injectInherited(tree::Object const& r, Object const& o,
                       tree::Value& v) const;

  // Instance

  bool checkInstance(tree::Object const& r, tree::Value const& v) const;
  void expandInstance(tree::Object& r, tree::Value& v);
  bool flattenInstance(tree::Object& r, tree::Value& v);
  bool resolveInstance(tree::Object const& r, tree::Value const& v) const;

  // Coach

  void display(Coach::Branch const& br) const;
  bool explain(tree::Path const& p, tree::Path::const_iterator const& i) const;

  // Generator

  void collectModelFileDependencies(std::set<std::string>& d) const;

  void collectInterfaceIncludes(std::set<std::string>& i) const;
  void collectImplementationIncludes(std::set<std::string>& i) const;

  using Type::doBuildDefinition;
  void doBuildDefinition(std::string const& s, std::string const& v,
                         std::string const& e, std::ostream& o, int l) const;

  using Type::doSerializerDefinition;
  void doSerializerDefinition(std::string const& c, std::string const& n,
                              std::string const& v, const bool b,
                              std::ostream& o, int l) const;

  void doGetterInterface(std::ostream& o, int l = 0) const;
  void doGetterDeclaration(std::ostream& o, int l = 0) const;
  void doGetterDefinition(std::ostream& o, int l = 0) const;

  // Basic Type

  bool merge(BasicType const& b);
  BasicType::Ref clone(std::string const& n) const;
  std::string typeName() const;

  bool has(tree::Path const& path, tree::Path::const_iterator const& i) const;

  void get(tree::Path const& p, tree::Path::const_iterator const& i,
           std::list<BasicType::Ref>& r) const;

  void promoteArity(tree::Path const& p, tree::Path::const_iterator const& i);
  void disable(tree::Path const& p, tree::Path::const_iterator const& i);

  bool isObject() const;

  std::vector<std::string> values(tree::Object const& r) const;

  // Accessors

  ModelAttributeType const& modelAttribute() const;

  Model& model();
  Model const& model() const;

  std::map<tree::Path, Class::Ref>& plugins();
  std::map<tree::Path, Class::Ref> const& plugins() const;

private:
  Model::Ref m_model;
  Arity m_targetArity;
  std::map<tree::Path, Class::Ref> m_plugins;
  std::map<tree::Path, Class::Ref> m_instances;
};

}}
