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

#include <ace/types/Enum.h>
#include <ace/model/Model.h>
#include <set>
#include <string>

namespace ace { namespace model {

Enum::Enum()
  : Type(BasicType::Kind::Enum), EnumeratedType(BasicType::Kind::Enum)
{
  m_attributes.define<BindAttributeType>("bind", false);
}

bool
Enum::hasPrivateNamespaceDefinition() const
{
  return true;
}

void
Enum::doPrivateNamespaceDefinition(std::string const& ns, std::ostream& o,
                                   int l) const
{
  std::string ftn = ns + "::" + typeName();
  auto const& b = bindAttribute().values();
  /*
   * Generate the single-value parser.
   */
  indent(o, l) << ftn << std::endl;
  indent(o, l) << typeName() << "Parse(std::string const & v) {" << std::endl;
  for (auto e = b.begin(); e != b.end(); e++) {
    indent(o, l + 2) << "if (v == \"" << e->first << "\") {" << std::endl;
    indent(o, l + 4) << "return " << ftn << "::" << e->first << ";";
    indent(o, l + 4) << std::endl;
    indent(o, l + 2) << "}";
  }
  o << std::endl;
  indent(o, l + 2) << "throw std::invalid_argument(\"" << typeName() << "\");"
                   << std::endl;
  indent(o, l) << "}" << std::endl << std::endl;
  /*
   * Generate the multi-value parser.
   */
  if (multiple()) {
    indent(o, l) << "std::vector<" << ftn << ">" << std::endl;
    indent(o, l) << typeName() << "Parse(std::vector<std::string> const & v) {"
                 << std::endl;
    indent(o, l + 2) << "std::vector<" << ftn << "> res;" << std::endl;
    indent(o, l + 2) << "for(auto const& e : v) {" << std::endl;
    indent(o, l + 4) << "res.push_back(" << typeName() << "Parse(e));"
                     << std::endl;
    indent(o, l + 2) << "}" << std::endl;
    indent(o, l + 2) << "return res;" << std::endl;
    indent(o, l) << "}" << std::endl << std::endl;
  }
  /*
   * Generate the single-value serializer.
   */
  indent(o, l) << "std::string" << std::endl;
  indent(o, l) << typeName() << "Serialize(const " << ftn << " v) {"
               << std::endl;
  indent(o, l + 2) << "switch(v) {" << std::endl;
  for (auto e = b.begin(); e != b.end(); e++) {
    indent(o, l + 4) << "case " << ftn << "::" << e->first << ": ";
    o << "return \"" << e->first << "\";" << std::endl;
  }
  indent(o, l + 4);
  o << "default: throw std::invalid_argument(\"" << typeName() << "\");";
  o << std::endl;
  indent(o, l + 2) << "}" << std::endl;
  indent(o, l) << "}" << std::endl;
  o << std::endl;
  /*
   * Generate the multi-value serializer.
   */
  if (multiple()) {
    indent(o, l) << "std::vector<std::string>" << std::endl;
    indent(o, l) << typeName() << "Serialize(std::vector<" << ftn
                 << "> const & v) {" << std::endl;
    indent(o, l + 2) << "std::vector<std::string> res;" << std::endl;
    indent(o, l + 2) << "for(auto const& e : v) {" << std::endl;
    indent(o, l + 4) << "res.push_back(" << typeName() << "Serialize(e));"
                     << std::endl;
    indent(o, l + 2) << "}" << std::endl;
    indent(o, l + 2) << "return res;" << std::endl;
    indent(o, l) << "}" << std::endl << std::endl;
  }
}

bool
Enum::hasTypeDeclaration() const
{
  return true;
}

void
Enum::doTypeDeclaration(std::ostream& o, int l) const
{
  indent(o, l) << "enum class " << typeName() << " {" << std::endl;
  for (auto const& e : bindAttribute().values()) {
    indent(o, l + 2) << e.first << " = " << e.second << "," << std::endl;
  }
  indent(o, l) << "};" << std::endl;
}

void
Enum::doBuildDefinition(std::string const& s, std::string const& v,
                        std::string const& e, std::ostream& o, int l) const
{
  indent(o, l);
  std::string tn = "std::string";
  if (multiple()) {
    tn = "std::vector<" + tn + ">";
  }
  o << tn << " " << declName() << ";" << std::endl;
  indent(o, l);
  if (optional() and not multiple()) {
    o << s << " = ";
  }
  o << "ace::tree::utils::parsePrimitive<std::string>";
  o << "(r, " << e << ", " << declName() << ");" << std::endl;
  indent(o, l);
  o << v << " = " << typeName() << "Parse(" << declName() << ");";
  o << std::endl;
}

void
Enum::doSerializerDefinition(std::string const& c, std::string const& n,
                             std::ostream& o, int l) const
{
  std::string p = typeName() + "Serialize(m_" + m_declName + ")";
  EnumeratedType::doSerializerDefinition(c, n, p, false, o, l);
}

void
Enum::doGetterInterface(std::ostream& o, int l) const
{
  std::string tn = typeName();
  if (multiple()) {
    tn = "std::vector<" + tn + ">";
  }
  indent(o, l) << "virtual " << tn << " const & ";
  o << m_declName << "() const = 0;" << std::endl;
}

void
Enum::doGetterDeclaration(std::ostream& o, int l) const
{
  std::string tn = typeName();
  if (multiple()) {
    tn = "std::vector<" + tn + ">";
  }
  indent(o, l) << tn << " const & " << m_declName << "() const;" << std::endl;
}

void
Enum::doGetterDefinition(std::ostream& o, int l) const
{
  const Model* m = static_cast<const Model*>(owner());
  std::string const& h = m->normalizedName();
  std::string tn = h + "::" + typeName();
  if (multiple()) {
    tn = "std::vector<" + tn + ">";
  }
  indent(o, l) << tn << " const & " << h << "::";
  o << m_declName << "() const {" << std::endl;
  indent(o, l + 2) << "return m_" << m_declName << ";" << std::endl;
  indent(o, l) << "}" << std::endl;
}

void
Enum::collectInterfaceIncludes(std::set<std::string>& i) const
{
  BasicType::collectInterfaceIncludes(i);
  i.insert("<string>");
}

void
Enum::collectImplementationIncludes(std::set<std::string>& i) const
{
  BasicType::collectInterfaceIncludes(i);
  i.insert("<string>");
}

BasicType::Ref
Enum::clone(std::string const& n) const
{
  Enum* str = new Enum(*this);
  str->setName(n);
  return BasicType::Ref(str);
}

std::string
Enum::typeName() const
{
  return common::String::camelify(name()) + "Type";
}

Enum::BindAttributeType&
Enum::bindAttribute()
{
  Attribute& attr = *m_attributes["bind"];
  return static_cast<BindAttributeType&>(attr);
}

Enum::BindAttributeType const&
Enum::bindAttribute() const
{
  Attribute const& attr = *m_attributes["bind"];
  return static_cast<BindAttributeType const&>(attr);
}

}}
