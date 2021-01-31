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

#include <ace/model/BasicType.h>
#include <ace/model/Dependency.h>
#include <ace/model/Errors.h>
#include <ace/model/Model.h>
#include <ace/engine/Master.h>
#include <ace/tree/Checker.h>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace ace { namespace model {

BasicType::BasicType(const Kind k, std::string const& a)
  : m_declName()
  , m_kind(k)
  , m_arityMap(a)
  , m_mayInherit(false)
  , m_doc()
  , m_attributes()
{
  m_attributes.setParent(this);
  m_attributes.define<KindAttributeType>("kind", false);
  m_attributes.define<ArityAttributeType>("arity", false);
  m_attributes.define<DocAttributeType>("doc", false);
  m_attributes.define<DeprecatedAttributeType>("deprecated", true);
  m_attributes.define<HookAttributeType>("hook", true);
  m_attributes.define<InheritAttributeType>("inherit", true);
  m_attributes.exclude("hook", "either");
  m_attributes.exclude("hook", "map");
  m_attributes.exclude("hook", "range");
  m_attributes.exclude("hook", "size");
  m_attributes.exclude("inherit", "hook");
}

BasicType::BasicType(BasicType const& o)
  : Object(o)
  , Instance(o)
  , Coach(o)
  , Generator(o)
  , m_declName(o.m_declName)
  , m_kind(o.m_kind)
  , m_arityMap(o.m_arityMap)
  , m_mayInherit(o.m_mayInherit)
  , m_doc(o.m_doc)
  , m_attributes(o.m_attributes)
{
  m_attributes.setParent(this);
}

bool
BasicType::checkModel(tree::Value const& t) const
{
  tree::Checker chk(path(), t);
  tree::Checker::Schema schm = m_attributes.schema();
  if (not chk.validate(schm)) {
    return false;
  }
  if (not m_attributes.checkModel(t)) {
    return false;
  }
  if (static_cast<tree::Primitive const&>(t["doc"])
        .value<std::string>()
        .empty()) {
    ERROR(ERR_EMPTY_DOC);
    return false;
  }
  Arity arity;
  std::string value =
    static_cast<tree::Primitive const&>(t["arity"]).value<std::string>();
  Arity::parse(value, arity);
  if (m_arityMap.find_first_of(arity.marker()) == std::string::npos) {
    ERROR(ERR_INVALID_ARITY_FOR_TYPE(value, m_kind));
    return false;
  }
  value = static_cast<tree::Primitive const&>(t["kind"]).value<std::string>();
  if (kindOf(value) == Kind::Undefined) {
    ERROR(ERR_TYPE_NOT_KNOWN(value));
    return false;
  } else if (kindOf(value) != m_kind) {
    ERROR(ERR_UNEXPECTED_TYPE(value));
    return false;
  }
  return true;
}

void
BasicType::loadModel(tree::Value const& t)
{
  m_attributes.loadModel(t);
  m_doc = static_cast<DocAttributeType const&>(*m_attributes["doc"]).head();
  Attribute::Ref ar = m_attributes["arity"];
  if (m_attributes.has("inherit")) {
    ar = m_attributes["inherit"];
    m_mayInherit = static_cast<InheritAttributeType const&>(*ar).head();
  }
}

bool
BasicType::flattenModel()
{
  if (not m_attributes.flattenModel()) {
    return false;
  }
  return true;
}

bool
BasicType::validateModel()
{
  if (not m_attributes.validateModel()) {
    return false;
  }
  return true;
}

void
BasicType::display(Coach::Branch const& br) const
{
  br.print(std::cout) << arity() << " " << m_name << " ";
  std::cout << "(" << toString(m_kind) << ")";
  std::cout << std::endl;
}

bool
BasicType::explain(tree::Path const& p,
                   tree::Path::const_iterator const& i) const
{
  Coach::explain(p, i);
  if (i != p.end()) {
    return false;
  }
  std::cout << std::left;
  for (auto& a : m_attributes) {
    std::cout << std::setw(13) << a->name() << ": ";
    a->print(std::cout, 15);
  }
  std::cout << std::right;
  return true;
}

bool
BasicType::hasPrivateNamespaceDefinition() const
{
  return false;
}

void
BasicType::doPrivateNamespaceDefinition(std::ostream& o, int l) const
{}

bool
BasicType::hasTypeDeclaration() const
{
  return false;
}

void
BasicType::doTypeDeclaration(std::ostream& o, int l) const
{}

void
BasicType::doTypeDefinition(std::ostream& o, int l) const
{
  if (multiple()) {
    indent(o, l) << "std::vector<" + typeName() + "> m_" + m_declName << ";";
    o << std::endl;
  } else {
    indent(o, l) << typeName() << " m_" << m_declName << ";" << std::endl;
    if (optional()) {
      indent(o, l) << "bool m_has_" << m_declName << ";" << std::endl;
    }
  }
}

void
BasicType::doBuildDefinition(std::string const& e, std::ostream& o, int l) const
{
  doBuildDefinition("m_has_" + m_declName, "m_" + m_declName, e, o, l);
  if (isDeprecated()) {
    indent(o, l) << "if (not m_" << m_declName << ".empty()) {" << std::endl;
    indent(o, l + 2) << "std::cerr << \"<ACE> DEPRECATED(" << path()
                     << "): \";";
    o << std::endl;
    indent(o, l + 2) << "std::cerr << \"" << deprecatedMessage() << "\";"
                     << std::endl;
    indent(o, l + 2) << "std::cerr << std::endl;" << std::endl;
    indent(o, l) << "}" << std::endl;
  }
}

void
BasicType::doBuildDefinition(std::string const& s, std::string const& v,
                             std::string const& e, std::ostream& o, int l) const
{
  indent(o, l);
  if (optional() and not multiple()) {
    o << s << " = ";
  }
  o << "ace::tree::utils::parsePrimitive<" << typeName() << ">(r, " << e << ", "
    << v << ");";
  o << std::endl;
}

void
BasicType::doSerializerDefinition(std::string const& c, std::string const& n,
                                  std::ostream& o, int l) const
{
  doSerializerDefinition(c, n, "m_" + m_declName, false, o, l);
}

void
BasicType::doSerializerDefinition(std::string const& c, std::string const& n,
                                  std::string const& v, const bool b,
                                  std::ostream& o, int l) const
{
  if (not multiple()) {
    int offset = 0;
    if (optional() and not b) {
      offset = 2;
      indent(o, l) << "if (m_has_" << m_declName << ") {" << std::endl;
    }
    indent(o, l + offset);
    o << c << "->put(ace::tree::Primitive::build(" << n << ", " << v << "));";
    o << std::endl;
    if (optional() and not b) {
      indent(o, l) << "}" << std::endl;
    }
  } else {
    indent(o, l);
    o << "if (not " << v << ".empty()) {" << std::endl;
    auto tmpArray = tempName();
    indent(o, l + 2);
    o << "auto " << tmpArray << " = ace::tree::Array::build(" << n << ");";
    o << std::endl;
    auto tmpIndex = tempName();
    indent(o, l + 2);
    o << "for (auto const & " << tmpIndex << " : " << v << ") {" << std::endl;
    indent(o, l + 4);
    o << tmpArray << "->push_back(ace::tree::Primitive::build(\"\", ";
    if (m_kind == Kind::Boolean) {
      o << "static_cast<bool>(";
    }
    o << tmpIndex;
    if (m_kind == Kind::Boolean) {
      o << ")";
    }
    o << "));";
    o << std::endl;
    indent(o, l + 2);
    o << "}" << std::endl;
    indent(o, l + 2);
    o << "o->put(" << tmpArray << ");" << std::endl;
    indent(o, l);
    o << "}" << std::endl;
  }
}

void
BasicType::doCheckerInterface(std::ostream& o, int l) const
{
  indent(o, l) << "virtual bool has_" << m_declName << "() const = 0;"
               << std::endl;
}

void
BasicType::doCheckerDeclaration(std::ostream& o, int l) const
{
  indent(o, l) << "bool has_" << m_declName << "() const;" << std::endl;
}

void
BasicType::doCheckerDefinition(std::ostream& o, int l) const
{
  const Model* m = static_cast<const Model*>(owner());
  std::string const& h = m->normalizedName();
  indent(o, l) << "bool " << h << "::has_" << m_declName << "() const {"
               << std::endl;
  if (multiple()) {
    indent(o, l + 2) << "return not m_" << m_declName << ".empty();"
                     << std::endl;
  } else {
    indent(o, l + 2) << "return m_has_" << m_declName << ";" << std::endl;
  }
  indent(o, l) << "}" << std::endl;
}

void
BasicType::doGetterInterface(std::ostream& o, int l) const
{
  std::string tn = decorateType(typeName());
  indent(o, l) << "virtual " << tn << " const & " << m_declName
               << "() const = 0;" << std::endl;
}

void
BasicType::doGetterDeclaration(std::ostream& o, int l) const
{
  std::string tn = decorateType(typeName());
  indent(o, l) << tn << " const & " << m_declName << "() const;" << std::endl;
}

void
BasicType::doGetterDefinition(std::ostream& o, int l) const
{
  const Model* m = static_cast<const Model*>(owner());
  std::string const& h = m->normalizedName();
  std::string tn = decorateType(typeName());
  indent(o, l) << tn << " const & " << h << "::" << m_declName << "() const {"
               << std::endl;
  if (optional()) {
    if (multiple()) {
      indent(o, l + 2) << "if (m_" << m_declName << ".empty()) ";
    } else {
      indent(o, l + 2) << "if (not m_has_" << m_declName << ") ";
    }
    o << "ace::tree::utils::illegalValueAccess(__PRETTY_FUNCTION__);"
      << std::endl;
  }
  indent(o, l + 2) << "return m_" << m_declName << ";" << std::endl;
  indent(o, l) << "}" << std::endl;
}

bool
BasicType::merge(BasicType const& b)
{
  return m_attributes.merge(b.m_attributes);
}

bool
BasicType::override(BasicType const& b)
{
  return m_attributes.override(b.m_attributes);
}

void
BasicType::promoteArity(tree::Path const& p,
                        tree::Path::const_iterator const& i)
{
  if (i == p.end() and arity().promote()) {
    MASTER.pushPromoted(path());
  }
}

void
BasicType::disable(tree::Path const& p, tree::Path::const_iterator const& i)
{
  if (optional()) {
    arity().disable();
  }
}

bool
BasicType::isObject() const
{
  return false;
}

bool
BasicType::isEnumerated() const
{
  return false;
}

bool
BasicType::isRanged() const
{
  return false;
}

bool
BasicType::isMapped() const
{
  return false;
}

bool
BasicType::checkValueConstraint(std::list<tree::Value::Ref> const& v) const
{
  ERROR(ERR_CANNOT_CONSTRAIN);
  return false;
}

bool
BasicType::checkRangeConstraint(std::string const& v) const
{
  ERROR(ERR_CANNOT_CONSTRAIN);
  return false;
}

bool
BasicType::constrainByValue(std::list<tree::Value::Ref> const& v)
{
  ERROR(ERR_CANNOT_CONSTRAIN);
  return false;
}

bool
BasicType::constrainByRange(std::string const& v)
{
  ERROR(ERR_CANNOT_CONSTRAIN);
  return false;
}

BasicType::Kind
BasicType::kindOf(std::string const& s)
{
  const std::map<std::string, ace::model::BasicType::Kind> stringToKind = {
    { "boolean", ace::model::BasicType::Kind::Boolean },
    { "class", ace::model::BasicType::Kind::Class },
    { "cpuid", ace::model::BasicType::Kind::CPUID },
    { "enum", ace::model::BasicType::Kind::Enum },
    { "file", ace::model::BasicType::Kind::File },
    { "float", ace::model::BasicType::Kind::Float },
    { "integer", ace::model::BasicType::Kind::Integer },
    { "ipv4", ace::model::BasicType::Kind::IPv4 },
    { "mac", ace::model::BasicType::Kind::MAC },
    { "plugin", ace::model::BasicType::Kind::Plugin },
    { "select", ace::model::BasicType::Kind::Selector },
    { "string", ace::model::BasicType::Kind::String },
    { "uri", ace::model::BasicType::Kind::URI },
  };
  if (stringToKind.find(s) == stringToKind.end()) {
    return Kind::Undefined;
  }
  return stringToKind.at(s);
}

std::string
BasicType::toString(const Kind k)
{
  const std::map<ace::model::BasicType::Kind, std::string> kindToString = {
    { ace::model::BasicType::Kind::Boolean, "boolean" },
    { ace::model::BasicType::Kind::CPUID, "cpuid" },
    { ace::model::BasicType::Kind::Class, "class" },
    { ace::model::BasicType::Kind::Enum, "enum" },
    { ace::model::BasicType::Kind::File, "file" },
    { ace::model::BasicType::Kind::Float, "float" },
    { ace::model::BasicType::Kind::Integer, "integer" },
    { ace::model::BasicType::Kind::IPv4, "ipv4" },
    { ace::model::BasicType::Kind::MAC, "mac" },
    { ace::model::BasicType::Kind::Plugin, "plugin" },
    { ace::model::BasicType::Kind::Selector, "select" },
    { ace::model::BasicType::Kind::String, "string" },
    { ace::model::BasicType::Kind::URI, "uri" },
  };
  if (kindToString.find(k) == kindToString.end()) {
    return "undefined";
  }
  return kindToString.at(k);
}

void
BasicType::foreach (tree::Object const& r, Callback const& op) const
{
  tree::Value const& v = r.get(m_name);
  switch (v.type()) {
    case tree::Value::Type::Object:
    case tree::Value::Type::Boolean:
    case tree::Value::Type::Float:
    case tree::Value::Type::Integer:
    case tree::Value::Type::String:
      op(v);
      break;
    case tree::Value::Type::Array:
      for (auto& e : static_cast<tree::Array const&>(v)) {
        op(*e);
      }
      break;
    default:
      break;
  }
}

void
BasicType::setName(std::string const& s)
{
  Object::setName(s);
  m_declName = common::String::normalize(s);
}

bool
BasicType::checkInstance(tree::Object const& r, tree::Value const& v) const
{
  if (v.type() == tree::Value::Type::Undefined) {
    return false;
  }
  if (not multiple() and v.type() == tree::Value::Type::Array) {
    ERROR(ERR_WRONG_ARITY_COUNT);
    return false;
  }
  /**
   * Validate the attributes once to make sure that bounds are respected
   */
  return m_attributes.checkInstance(r, v);
}

void
BasicType::expandInstance(tree::Object& r, tree::Value& v)
{
  return m_attributes.expandInstance(r, v);
}

bool
BasicType::flattenInstance(tree::Object& r, tree::Value& v)
{
  return m_attributes.flattenInstance(r, v);
}

bool
BasicType::resolveInstance(tree::Object const& r, tree::Value const& v) const
{
  /**
   * Validate the attributes twice to make sure that dynamic constraints are
   * respected
   */
  return m_attributes.resolveInstance(r, v);
}

void
BasicType::collectModelFileDependencies(std::set<std::string>& d) const
{}

void
BasicType::collectInterfaceIncludes(std::set<std::string>& i) const
{
  i.insert("<vector>");
}

void
BasicType::collectImplementationIncludes(std::set<std::string>& i) const
{
  i.insert("<cstdlib>");
  i.insert("<vector>");
}

bool
BasicType::injectDefault(tree::Object const& r, tree::Value& v) const
{
  return false;
}

std::string
BasicType::decorateType(std::string const& n) const
{
  if (multiple()) {
    return "std::vector<" + n + ">";
  }
  return n;
}

bool
BasicType::disabled() const
{
  return arity() == Arity::Kind::Disabled;
}

bool
BasicType::optional() const
{
  return arity() == Arity::Kind::UpToOne or arity() == Arity::Kind::Any;
}

bool
BasicType::required() const
{
  return arity() == Arity::Kind::One or arity() == Arity::Kind::AtLeastOne;
}

bool
BasicType::multiple() const
{
  return arity() == Arity::Kind::AtLeastOne or arity() == Arity::Kind::Any;
}

bool
BasicType::has(tree::Path const& p, tree::Path::const_iterator const& i) const
{
  return i == p.end();
}

void
BasicType::get(tree::Path const& p, tree::Path::const_iterator const& i,
               std::list<BasicType::Ref>& r) const
{}

bool
BasicType::operator<=(BasicType const& o) const
{
  return m_kind == o.m_kind and arity() <= o.arity();
}

bool
BasicType::operator>(BasicType const& o) const
{
  return m_kind != o.m_kind or not(arity() <= o.arity());
}

std::string const&
BasicType::declName() const
{
  return m_declName;
}

BasicType::Kind
BasicType::kind() const
{
  return m_kind;
}

Arity&
BasicType::arity()
{
  return static_cast<ArityAttributeType&>(*m_attributes["arity"]).value();
}

Arity const&
BasicType::arity() const
{
  return static_cast<ArityAttributeType const&>(*m_attributes["arity"]).value();
}

bool
BasicType::mayInherit() const
{
  return m_mayInherit;
}

bool
BasicType::hasHook() const
{
  return m_attributes.has("hook");
}

Hook const&
BasicType::hook() const
{
  return static_cast<HookAttributeType const&>(*m_attributes["hook"]).hook();
}

bool
BasicType::hasDependencies() const
{
  return m_attributes.has("deps");
}

BasicType::DepsAttributeType::Dependencies const&
BasicType::dependencies() const
{
  return std::static_pointer_cast<DepsAttributeType>(m_attributes["deps"])
    ->values();
}

bool
BasicType::isDeprecated() const
{
  return m_attributes.has("deprecated");
}

std::string const&
BasicType::deprecatedMessage() const
{
  auto const& ar = *m_attributes["deprecated"];
  return static_cast<DeprecatedAttributeType const&>(ar).head();
}

std::ostream&
operator<<(std::ostream& o, BasicType const& k)
{
  o << "[" << k.arity() << "] ";
  o << std::setw(9) << BasicType::toString(k.m_kind);
  o << " " << k.m_doc;
  return o;
}

std::ostream&
operator<<(std::ostream& o, const BasicType::Kind k)
{
  o << BasicType::toString(k);
  return o;
}

}}
