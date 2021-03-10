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

#include "Body.h"
#include "Coach.h"
#include "Header.h"
#include "Instance.h"
#include "Object.h"
#include <ace/filesystem/Path.h>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace ace { namespace model {

class Model
  : public Object
  , public Instance
  , public Coach
{
public:
  using Ref = std::shared_ptr<Model>;

public:
  Model() = delete;
  explicit Model(Model const& mdl);
  explicit Model(std::string const& fn);

  // Object

  tree::Path path(const bool local = false) const;

  bool checkModel(tree::Value const& t) const;
  void loadModel(tree::Value const& t);
  bool flattenModel();
  bool validateModel();

  // Instance

  bool checkInstance(tree::Value const& v) const;
  void expandInstance(tree::Value& v);
  bool flattenInstance(tree::Value& v);
  bool resolveInstance(tree::Value const& v) const;

  // Coach

  void display(Coach::Branch const& br) const;

  bool explain(tree::Path const& path) const;
  bool explain(tree::Path const& p, tree::Path::const_iterator const& i) const;

  // Code generation

  std::string interfaceHeaderName() const;
  std::string implementationHeaderName() const;
  std::string implementationSourceName() const;

  std::string interfaceHeaderPath() const;
  std::string implementationHeaderPath() const;
  std::string implementationSourcePath() const;

  std::string declarationType() const;
  std::string definitionType() const;

  std::string interfaceIncludeStatement(bool global) const;
  std::string implementationIncludeStatement(bool global) const;

  void generateInterface(fs::Path const& path) const;
  void generateImplementation(fs::Path const& path) const;

  void collectModelFileDependencies(std::set<std::string>& d) const;

  // Model checker

  static bool check(const Object* o, std::string const& n);
  static Model::Ref load(Object* o, std::string const& n);

  // Model loader and validator

  static Model::Ref load(std::string const& fn);
  tree::Value::Ref validate(std::string const& cfg, const int argc,
                            char** const argv);

  // Inheritence

  bool isAnAncestor(Model const& m) const;

  // Cloning

  Model::Ref clone() const;

  // Accessors

  Header const& header() const;

  std::string fileName() const;

  std::string filePath(std::string const& pkg) const;
  std::string filePath() const;

  Section& templates();
  Section const& templates() const;

  Body& body();
  Body const& body() const;

  std::string normalizedName() const;

  // Object

  Object* owner();
  const Object* owner() const;

private:
  class Context
  {
  public:
    Context() = delete;
    explicit Context(std::string const& path);
    ~Context();

  private:
    std::string m_path;
  };

  static void* nullBuilder(tree::Value const& v);

  std::string headerGuard(std::string const& n) const;

  bool checkInstance(tree::Object const& r, tree::Value const& v) const;
  void expandInstance(tree::Object& r, tree::Value& v);
  bool flattenInstance(tree::Object& r, tree::Value& v);
  bool resolveInstance(tree::Object const& r, tree::Value const& v) const;

  void generateInterfaceHeader(std::ostream& o) const;
  void generateImplementationHeader(std::ostream& o) const;
  void generateImplementationSource(std::ostream& o) const;

  std::string m_ext;
  std::string m_source;
  Header m_header;
  Section m_templates;
  Body m_body;
  std::vector<Ref> m_includes;
};

}}
