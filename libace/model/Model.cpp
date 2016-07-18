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

#include <ace/model/Model.h>
#include <ace/model/Errors.h>
#include <ace/engine/Master.h>
#include <ace/tree/Checker.h>
#include <ace/types/Class.h>
#include <ace/types/Plugin.h>
#include <ace/types/Selector.h>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using namespace ace::model;

void
expandModelDependencies(std::set<std::string> & decls, BasicType const & bt) {
  if (bt.kind() == BasicType::Kind::Class) {
    Class const & type = dynamic_cast<Class const &>(bt);
    decls.insert(type.modelAttribute().model().declarationType());
  } else if (bt.kind() == BasicType::Kind::Selector) {
    Selector const & type = dynamic_cast<Selector const &>(bt);
    expandModelDependencies(decls, type.templateType());
  } else if (bt.kind() == BasicType::Kind::Plugin) {
    Plugin const & type = dynamic_cast<Plugin const &>(bt);
    decls.insert(type.model().declarationType());
    for (auto & e : type.plugins()) expandModelDependencies(decls, *e.second);
  }
}

} // namespace

namespace ace {
namespace model {

/**
 * Context class.
 */

Model::Context::Context(std::string const & path)
  : m_path(path) {
  if (not MASTER.addModelPathIntoContext(m_path)) {
    throw std::runtime_error("Duplicate path in context: " + m_path);
  }
}

Model::Context::~Context() {
  MASTER.remModelPathFromContext(m_path);
}

/**
 * Model class.
 */

Model::Model(std::string const & fn)
  : m_ext()
  , m_source()
  , m_header()
  , m_templates()
  , m_body()
  , m_includes() {
  std::vector<std::string> elems;
  common::String::split(fn, '.', elems);
  m_ext = *elems.rbegin();
  setName(*(++elems.rbegin()));
  m_header.setParent(this);
  m_templates.setParent(this);
  m_body.setParent(this);
}

Model::Model(Model const & o)
  : Object(o), Instance(o), Coach(o)
  , m_ext(o.m_ext)
  , m_source(o.m_source)
  , m_header(o.m_header)
  , m_templates(o.m_templates)
  , m_body(o.m_body)
  , m_includes(o.m_includes) {
  m_header.setParent(this);
  m_templates.setParent(this);
  m_body.setParent(this);
}

tree::Path
Model::path() const {
  if (m_parent == nullptr) {
    auto item = tree::path::Item::build(tree::path::Item::Type::Global);
    tree::Path p = tree::Path().push(item);
    return p;
  }
  return m_parent->path();
}

bool
Model::checkModel(tree::Value const & t) const try {
  const ace::tree::Checker::Schema jsonSchema = {
    { "header"   , { ace::tree::Value::Type::Object, false } },
    { "templates", { ace::tree::Value::Type::Object, true  } },
    { "body"     , { ace::tree::Value::Type::Object, false } }
  };
  Context context(filePath());
  ace::tree::Checker chk(path(), t);
  if (not chk.validate(jsonSchema)) return false;
  if (not m_header.checkModel(t["header"])) return false;
  if (t.has("templates")) {
    DEBUG("Check model templates");
    if (not m_templates.checkModel(t["templates"])) return false;
  }
  DEBUG("Check model body");
  if (not m_body.checkModel(t["body"])) return false;
  return true;
}
catch (std::runtime_error const & e) {
  ERROR(ERR_MODEL_LOOP_DETECTED(filePath()));
  return false;
}

void
Model::loadModel(tree::Value const & t) {
  m_header.loadModel(t["header"]);
  for (auto & inc : m_header.include()) {
    MASTER.addChildForPath(filePath(), inc);
  }
  for (auto & trg : m_header.trigger()) {
    MASTER.addModelBuilder(filePath(), trg, nullBuilder);
  }
  if (t.has("templates")) {
    DEBUG("Load model templates");
    m_templates.loadModel(t["templates"]);
  }
  DEBUG("Load model body");
  m_body.loadModel(t["body"]);
  std::ostringstream oss;
  MASTER.scannerByName("json").dump(t, tree::Scanner::Format::Inlined, oss);
  m_source = oss.str();
}

bool
Model::flattenModel() try {
  Context context(filePath());
  if (not m_body.flattenModel()) return false;
  Ref om = nullptr;
  DEBUG("Merge ", m_header.include().size(), " included models");
  for (auto & fp : m_header.include()) {
    DEBUG("Check \"", fp, "\"");
    if (not check(nullptr, fp)) {
      ERROR(ERR_INVALID_MODEL(fp));
      return false;
    }
    DEBUG("Load \"", fp, "\"");
    Ref xm = load(nullptr, fp);
    DEBUG("Flatten \"", fp, "\"");
    if (not xm->flattenModel()) return false;
    if (om != nullptr) {
      if (not om->m_templates.merge(xm->m_templates)) {
        ERROR(ERR_FAILED_TEMPLATE_MERGE(xm->name()));
        return false;
      }
      if (not om->m_body.merge(xm->m_body)) {
        ERROR(ERR_FAILED_BODY_MERGE(xm->name()));
        return false;
      }
    } else {
      om = xm;
    }
    m_includes.push_back(xm);
  }
  if (om != nullptr) {
    if (not m_templates.override(om->m_templates)) {
      ERROR(ERR_FAILED_TEMPLATE_OVERRIDE);
      return false;
    }
    if (not m_body.override(om->m_body)) {
      ERROR(ERR_FAILED_BODY_OVERRIDE);
      return false;
    }
  }
  return true;
}
catch (std::runtime_error const & e) {
  ERROR(ERR_MODEL_LOOP_DETECTED(filePath()));
  return false;
}

bool
Model::validateModel() {
  if (not MASTER.hasModel(filePath())) {
    ERROR(ERR_INVALID_PACKAGE_PATH(filePath()));
    return false;
  }
  return m_body.validateModel();
}

bool
Model::checkInstance(tree::Object const & r, tree::Value const & v) const {
  return m_body.checkInstance(r, v);
}

void
Model::expandInstance(tree::Object & r, tree::Value & v) {
  m_body.expandInstance(r, v);
}

bool
Model::flattenInstance(tree::Object & r, tree::Value & v) {
  return m_body.flattenInstance(r, v);
}

bool
Model::resolveInstance(tree::Object const & r, tree::Value const & v) const {
  return m_body.resolveInstance(r, v);
}

void
Model::display(Coach::Branch const & br) const {
  m_body.display(br);
}

bool
Model::explain(tree::Path const & p, tree::Path::const_iterator const & i) const {
  Coach::explain(p, i);
  if (i == p.end()) {
    m_header.explain(p, i);
    return true;
  } else {
    return m_body.explain(p, i);
  }
}

std::string
Model::interfaceHeaderName() const {
  return "I" + m_name + ".ac.h";
}

std::string
Model::implementationHeaderName() const {
  return m_name + ".ac.h";
}

std::string
Model::implementationSourceName() const {
  return m_name + ".ac.cpp";
}

std::string
Model::interfaceHeaderPath() const {
  return m_header.package() + interfaceHeaderName();
}

std::string
Model::implementationHeaderPath() const {
  return m_header.package() + implementationHeaderName();
}

std::string
Model::implementationSourcePath() const {
  return m_header.package() + implementationSourceName();
}

std::string
Model::declarationType() const {
  std::ostringstream oss;
  for (auto & ns : m_header.nameSpace()) oss << ns << "::";
  oss << "I" << normalizedName();
  return oss.str();
}

std::string
Model::definitionType() const {
  std::ostringstream oss;
  for (auto & ns : m_header.nameSpace()) oss << ns << "::";
  oss << normalizedName();
  return oss.str();
}

std::string
Model::interfaceIncludeStatement(bool global) const {
  std::ostringstream oss;
  oss << (global ? "<" : "\"");
  if (global) oss << m_header.package();
  oss << interfaceHeaderName();
  oss << (global ? ">" : "\"");
  return oss.str();
}

std::string
Model::implementationIncludeStatement(bool global) const {
  std::ostringstream oss;
  oss << (global ? "<" : "\"");
  if (global) oss << m_header.package();
  oss << implementationHeaderName();
  oss << (global ? ">" : "\"");
  return oss.str();
}

std::string
Model::headerGuard(std::string const & n) const {
  std::ostringstream oss;
  common::Path p(m_header.package());
  for (auto & e : p) if (not e.empty()) oss << common::String::normalize(e) << "_";
  oss << n << "_AC_H_";
  std::string hdr(oss.str());
  std::transform(hdr.begin(), hdr.end(), hdr.begin(), ::toupper);
  return hdr;
}

void
Model::collectModelFileDependencies(std::set<std::string> & d) const {
  for (auto & e : m_includes) {
    d.insert(e->filePath());
    e->collectModelFileDependencies(d);
  }
  for (auto & e : m_templates) {
    e.second->collectModelFileDependencies(d);
  }
  for (auto & e : m_body) {
    e.second->collectModelFileDependencies(d);
  }
}

bool
Model::check(const Object * o, std::string const & n) {
  tree::Value::Ref root;
  if (not MASTER.hasScannerByExtension(n)) {
    ACE_LOG(Error, "Missing scanner for file type \"", n, "\"");
    return false;
  }
  if (MASTER.isInlinedModel(n)) {
    ACE_LOG(Debug, "Check inlined model \"", n, "\"");
    root = MASTER.scannerByExtension(n).parse(MASTER.modelSourceFor(n), 0, nullptr);
  } else if (MASTER.hasModel(n)) {
    common::Path path = MASTER.modelPathFor(n);
    ACE_LOG(Debug, "Check model \"", n, "\" @ PATH -> ", path);
    root = MASTER.scannerByExtension(n).open(path.toString(), 0, nullptr);
  }
  if (root == nullptr) {
    ACE_LOG(Warning, "Model \"", n, "\" not found, either inline or in the search path");
    return false;
  }
  Model aModel(*common::Path(n).rbegin());
  aModel.setParent(o);
  if (not aModel.checkModel(*root)) return false;
  return true;
}

Model::Ref
Model::load(Object * o, std::string const & n) {
  tree::Value::Ref root;
  if (MASTER.isInlinedModel(n)) {
    ACE_LOG(Debug, "Load inlined model \"", n, "\"");
    root = MASTER.scannerByExtension(n).parse(MASTER.modelSourceFor(n), 0, nullptr);
  } else if (MASTER.hasModel(n)) {
    common::Path path = MASTER.modelPathFor(n);
    ACE_LOG(Debug, "Load model \"", n, "\" @ PATH -> ", path);
    root = MASTER.scannerByExtension(n).open(path.toString(), 0, nullptr);
  }
  Model::Ref aModel(new Model(*common::Path(n).rbegin()));
  aModel->setParent(o);
  aModel->loadModel(*root);
  return aModel;
}

Model::Ref
Model::load(std::string const & fn) {
  model::Model::Ref mdl;
  if (not model::Model::check(nullptr, fn)) {
    ACE_LOG(Error, "Check model \"" + fn + "\" failed");
    return nullptr;
  }
  mdl = model::Model::load(nullptr, fn);
  if (not mdl->flattenModel()) {
    ACE_LOG(Error, "Flatten model \"" + fn + "\" failed");
    return nullptr;
  }
  if (not mdl->validateModel()) {
    ACE_LOG(Error, "Validate model \"" + fn + "\" failed");
    return nullptr;
  }
  return mdl;
}

tree::Value::Ref
Model::validate(std::string const & cfgName, const int argc, char ** const argv) {
  tree::Value::Ref svr;
  if (not MASTER.hasScannerByExtension(cfgName)) {
    ACE_LOG(Error, "Unsupported configuration file format: ", cfgName);
    return nullptr;
  }
  svr = MASTER.scannerByExtension(cfgName).open(cfgName, argc, argv);
  if (svr == nullptr) {
    ACE_LOG(Error, "Cannot open configuration file \"" + cfgName + "\"");
    return nullptr;
  }
  if (not checkInstance(*svr)) {
    ACE_LOG(Error, "Check configuration \"" + cfgName + "\" failed");
    return nullptr;
  }
  expandInstance(*svr);
  if (not flattenInstance(*svr)) {
    ACE_LOG(Error, "Flatten configuration \"" + cfgName + "\" failed");
    return nullptr;
  }
  if (not resolveInstance(*svr)) {
    ACE_LOG(Error, "Resolve configuration \"" + cfgName + "\" failed");
    return nullptr;
  }
  return svr;
}

bool
Model::isAnAncestor(Model const & m) const {
  for (auto & i : m_includes) if (i->filePath() == m.filePath()) return true;
  for (auto & i : m_includes) if (i->isAnAncestor(m)) return true;
  return false;
}

void
Model::generateInterface(common::Path const & p) const {
  fs::Directory dir(p);
  if (not dir.isValid()) dir = fs::Directory((common::Path(".", true)));
  std::string hp = (dir.path() / common::Path(interfaceHeaderName())).toString();
  std::ofstream hofs;
  hofs.open(hp, std::ofstream::trunc);
  generateInterfaceHeader(hofs);
  hofs.close();
}

void
Model::generateImplementation(common::Path const & p) const {
  fs::Directory dir(p);
  if (not dir.isValid()) dir = fs::Directory((common::Path(".", true)));
  std::string hp = (dir.path() / common::Path(implementationHeaderName())).toString();
  std::ofstream hofs;
  hofs.open(hp, std::ofstream::trunc);
  generateImplementationHeader(hofs);
  hofs.close();
  std::string cp = (dir.path() / common::Path(implementationSourceName())).toString();
  std::ofstream cofs;
  cofs.open(cp, std::ofstream::trunc);
  generateImplementationSource(cofs);
  cofs.close();
}

void
Model::generateInterfaceHeader(std::ostream & o) const {
  o << "#ifndef " << headerGuard("I" + normalizedName()) << std::endl;
  o << "#define " << headerGuard("I" + normalizedName()) << std::endl;
  o << std::endl;

  std::set<std::string> includes;
  includes.insert("<ace/tree/Object.h>");
  includes.insert("<memory>");
  includes.insert("<string>");
  for (auto & e : m_includes) includes.insert(e->interfaceIncludeStatement(true));
  for (auto & e : m_body) e.second->collectInterfaceIncludes(includes);

  for (auto & i : includes) o << "#include " << i << std::endl;
  if (not includes.empty()) o << std::endl;

  for (auto & ns : m_header.nameSpace()) o << "namespace " << ns << " {" << std::endl;
  if (not m_header.nameSpace().empty()) o << std::endl;

  o << "class I" << normalizedName();
  if (not m_includes.empty()) {
    o << std::endl;
    Generator::indent(o, 4) << ": ";
    for (size_t i = 0; i < m_includes.size(); i += 1) {
      o << "public virtual " << m_includes[i]->declarationType();
      if (i < m_includes.size() - 1) {
        o << "," << std::endl;
        Generator::indent(o, 4);
      }
    }
  }
  o << " {" << std::endl;

  std::string tn = "I" + normalizedName();
  Generator::indent(o, 1) << "public:" << std::endl << std::endl;
  Generator::indent(o, 2) << "using Ref = std::shared_ptr<" << tn << ">;" << std::endl;
  o << std::endl;

  Generator::indent(o, 2) << "static const std::string PATH;" << std::endl;
  Generator::indent(o, 2) << "static const std::string MODEL;" << std::endl;
  Generator::indent(o, 2) << "static const std::string VERSION;" << std::endl;
  Generator::indent(o, 2) << "static const bool REGISTERED;" << std::endl;
  o << std::endl;

  Generator::indent(o, 2) << "virtual ~I" << normalizedName() << "() { }" << std::endl;
  o << std::endl;

  Generator::indent(o, 2) << "virtual void serialize(ace::tree::Object::Ref & o) const = 0;";
  o << std::endl << std::endl;

  bool skip = true;
  DEBUG("Generate checker interface:");
  for (auto & e : m_body) if (e.second->optional()) {
    DEBUG("☑ ", e.second->path());
    skip = false;
    e.second->doCheckerInterface(o, 2);
  } else {
    DEBUG("☐ ", e.second->path());
  }
  if (not skip) o << std::endl;

  for (auto & e : m_body) e.second->doGetterInterface(o, 2);
  o << "};" << std::endl;
  o << std::endl;

  auto & ns = m_header.nameSpace();
  for (auto it = ns.rbegin(); it != ns.rend(); it++) o << "}  // namespace " << *it << std::endl;
  if (not ns.empty()) o << std::endl;

  o << std::endl;
  o << "#endif  // " << headerGuard("I" + normalizedName()) << std::endl;
}

void
Model::generateImplementationHeader(std::ostream & o) const {
  o << "#ifndef " << headerGuard(normalizedName()) << std::endl;
  o << "#define " << headerGuard(normalizedName()) << std::endl;
  o << std::endl;

  std::set<std::string> includes;
  includes.insert(interfaceIncludeStatement(false));
  includes.insert("<ace/tree/Object.h>");
  includes.insert("<string>");
  for (auto & e : m_body) e.second->collectInterfaceIncludes(includes);
  for (auto & i : includes) o << "#include " << i << std::endl;
  o << std::endl;

  for (auto & ns : m_header.nameSpace()) o << "namespace " << ns << " {" << std::endl;
  if (not m_header.nameSpace().empty()) o << std::endl;

  o << "class " << normalizedName() << " : public virtual " << declarationType() << " {";
  o << std::endl;

  Generator::indent(o, 1) << "public:" << std::endl << std::endl;

  for (auto & e : m_body) if (e.second->hasTypeDeclaration()) {
    e.second->doTypeDeclaration(o, 2);
    o << std::endl;
  }

  Generator::indent(o, 2) << "explicit " << normalizedName()
                          << "(ace::tree::Object const & r);"
                          << std::endl << std::endl;

  Generator::indent(o, 2) << "static " << normalizedName() << "::Ref "
                          << "build(ace::tree::Value const & v);"
                          << std::endl << std::endl;

  Generator::indent(o, 2) << "void serialize(ace::tree::Object::Ref & o) const;"
                          << std::endl << std::endl;

  bool skip = true;
  DEBUG("Generate checker declaration:");
  for (auto & e : m_body) if (e.second->optional()) {
    DEBUG("☑ ", e.second->path());
    skip = false;
    e.second->doCheckerDeclaration(o, 2);
  } else {
    DEBUG("☐ ", e.second->path());
  }
  if (not skip) o << std::endl;

  for (auto & e : m_body) e.second->doGetterDeclaration(o, 2);
  o << std::endl;

  Generator::indent(o, 1) << "protected:" << std::endl << std::endl;
  Generator::indent(o, 2) << normalizedName() << "() = default;" << std::endl << std::endl;

  for (auto & e : m_body) e.second->doTypeDeclaration(o, 2);
  for (auto & e : m_body) e.second->doTypeDefinition(o, 2);

  o << "};" << std::endl;
  o << std::endl;

  auto & ns = m_header.nameSpace();
  for (auto it = ns.rbegin(); it != ns.rend(); it++) o << "}  // namespace " << *it << std::endl;
  if (not ns.empty()) o << std::endl;

  o << "#endif  // " << headerGuard(normalizedName()) << std::endl;
}

void
Model::generateImplementationSource(std::ostream & o) const {
  o << "#include \"" << implementationHeaderName() << "\"" << std::endl;
  std::set<std::string> includes;
  includes.insert("<string>");
  includes.insert("<ace/engine/Master.h>");
  includes.insert("<ace/tree/Utils.h>");
  for (auto & e : m_body) e.second->collectImplementationIncludes(includes);
  for (auto & i : includes) o << "#include " << i << std::endl;
  o << std::endl;

  bool doPrivateNS = not m_header.trigger().empty();
  for (auto & e : m_body) {
    doPrivateNS = doPrivateNS or e.second->hasPrivateNamespaceDefinition();
  }
  if (doPrivateNS) {
    o << "namespace {" << std::endl << std::endl;
    if (not m_header.trigger().empty()) {
      o << "void * " << normalizedName() << "Builder(ace::tree::Value const & v) {";
      o << std::endl;
      Generator::indent(o, 2)
        << "ace::tree::Object const & r = static_cast<ace::tree::Object const &>(v);"
        << std::endl;
      Generator::indent(o, 2) << "return new " << definitionType() << "(r);";
      o << std::endl;
      o << "}" << std::endl << std::endl;
    }
    for (auto & e : m_body) {
      e.second->doPrivateNamespaceDefinition(o, 0);
      o << std::endl;
    }
    o << "} // namespace" << std::endl << std::endl;
  }

  for (auto & ns : m_header.nameSpace()) o << "namespace " << ns << " {" << std::endl;
  if (not m_header.nameSpace().empty()) o << std::endl;

  o << "const std::string I" << normalizedName() << "::PATH = \"" << filePath() << "\";";
  o << std::endl << std::endl;

  o << "const std::string I" << normalizedName() << "::MODEL = ";
  o << "\"" << m_source << "\"";
  o << std::endl << std::endl;

  o << "const std::string I" << normalizedName() << "::VERSION = \"" << ACE_VERSION << "\";";
  o << std::endl << std::endl;

  /**
   * @brief The following generates the calls that registered the model and its
   * builders, if any. These step are called first as they must not be optimized out
   * (see below for details).
   */
  o << "const bool I" << normalizedName() << "::REGISTERED = MASTER.addInlinedModel(PATH, MODEL)";
  for (auto & tr : m_header.trigger()) {
    o << std::endl;
    Generator::indent(o, 4) << "and " << "MASTER.addModelBuilder(PATH, \"";
    o << tr << "\", " << normalizedName() << "Builder)";
  }
  /**
   * @brief The following is generated simply to enforce link dependences. Apparently,
   * it does not enforce any order in the global construction list. However, only the
   * fact that the REGISTERED symbol is important, not its actual value.
   */
  std::set<std::string> decls;
  for (auto & e : m_includes) decls.insert(e->declarationType());
  for (auto & e : m_body) expandModelDependencies(decls, *e.second);
  for (auto & d : decls) {
    o << std::endl;
    Generator::indent(o, 4) << "and " << d << "::REGISTERED";
  }
  o << ";" << std::endl << std::endl;

  o << normalizedName() << "::" << normalizedName() << "(ace::tree::Object const & r) {";
  o << std::endl;
  for (auto & e : m_body) e.second->doBuildDefinition("\"" + e.first + "\"" , o, 2);
  o << "}" << std::endl;
  o << std::endl;

  o << normalizedName() << "::Ref " << normalizedName();
  o << "::build(ace::tree::Value const & v) {" << std::endl;
  Generator::indent(o, 2)
    << "ace::tree::Object const & r = static_cast<ace::tree::Object const &>(v);"
    << std::endl;
  Generator::indent(o, 2) << "return " << normalizedName() << "::Ref(" << "new ";
  o << normalizedName() << "(r));";
  o << std::endl;
  o << "}" << std::endl;
  o << std::endl;

  o << "void " << normalizedName() << "::serialize(ace::tree::Object::Ref & o) const {";
  o << std::endl;
  for (auto & e : m_body) e.second->doSerializerDefinition("o", "\"" + e.first + "\"" , o, 2);
  o << "}" << std::endl;
  o << std::endl;

  DEBUG("Generate checker definition:");
  for (auto & e : m_body) if (e.second->optional()) {
    DEBUG("☑ ", e.second->path());
    e.second->doCheckerDefinition(o, 0);
    o << std::endl;
  } else {
    DEBUG("☐ ", e.second->path());
  }

  for (auto & e : m_body) {
    e.second->doGetterDefinition(o, 0);
    o << std::endl;
  }

  auto & ns = m_header.nameSpace();
  for (auto it = ns.rbegin(); it != ns.rend(); it++) o << "}  // namespace " << *it << std::endl;
  o << std::endl;

  std::string fullName;
  for (auto & e : ns) fullName += e + "::";
  fullName += "I" + normalizedName();
  o << "bool isRegistered_";
  for (auto & e : ns) o << e << "_";
  o << "I" << normalizedName() << " = " << fullName << "::REGISTERED" << ";" << std::endl;
}

bool
Model::checkInstance(tree::Value const & v) const {
  tree::Object const & o = static_cast<tree::Object const &>(v);
  return checkInstance(o, v);
}

void
Model::expandInstance(tree::Value & v) {
  tree::Object & o = static_cast<tree::Object &>(v);
  expandInstance(o, v);
}

bool
Model::flattenInstance(tree::Value & v) {
  tree::Object & o = static_cast<tree::Object &>(v);
  return flattenInstance(o, v);
}

bool
Model::resolveInstance(tree::Value const & v) const {
  tree::Object const & o = static_cast<tree::Object const &>(v);
  return resolveInstance(o, v);
}

bool
Model::explain(tree::Path const & p) const {
  tree::Path::const_iterator n(p.begin());
  return explain(p, p.down(n));
}

Header const &
Model::header() const {
  return m_header;
}

std::string
Model::fileName() const {
  return name() + "." + m_ext;
}

std::string
Model::filePath() const {
  return m_header.package() + fileName();
}

Section &
Model::templates() {
  return m_templates;
}

Section const &
Model::templates() const {
  return m_templates;
}

Body &
Model::body() {
  return m_body;
}

Body const &
Model::body() const {
  return m_body;
}

Object *
Model::owner() {
  return this;
}

const Object *
Model::owner() const {
  return this;
}

void *
Model::nullBuilder(tree::Value const & v) {
  throw std::runtime_error("Default plugin NullBuilder should not be called");
}

std::string
Model::normalizedName() const {
  return common::String::normalize(m_name);
}

} // namespace model
} // namespace ace
