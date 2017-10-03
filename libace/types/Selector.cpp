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

#include <ace/types/Selector.h>
#include <ace/model/Body.h>
#include <ace/model/Errors.h>
#include <ace/model/Model.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <set>
#include <string>
#include <vector>

namespace ace {
namespace model {

// Selector format checker

SelectorFormatChecker::SelectorFormatChecker(const BasicType * o) : FormatChecker(o) { }

bool
SelectorFormatChecker::operator() (tree::Object const & r, tree::Value const & v) const {
  if (v.type() != tree::Value::Type::Object) {
    ERROR_O(m_owner, ERR_INSTANCE_NOT_AN_OBJECT);
    return false;
  }
  return true;
}

// Selector class

Selector::Selector()
    : Type(BasicType::Kind::Selector, "?1"),
    EnumeratedType(BasicType::Kind::Selector, "?1"),
    m_template(),
    m_types(),
    m_instances() {
  m_attributes.define<TemplateAttributeType>("template", false);
  m_attributes.define<SizeAttributeType>("size", false);
}

bool
Selector::flattenModel() {
  if (not EnumeratedType::flattenModel()) return false;
  const Model * m = static_cast<const Model *>(owner());
  std::string const & tn = templateAttribute().head();
  if (not m->templates().has(tn)) {
    ERROR(ERR_TEMPLATE_NOT_FOUND(tn));
    return false;
  }
  m_template = m->templates().get(tn).clone("template");
  m_template->setParent(this);
  if (not m_template->flattenModel()) return false;
  if (hasEitherAttribute()) for (auto & e : eitherAttribute().values()) {
    BasicType::Ref bt = m_template->clone(e);
    bt->setParent(this);
    m_types[e] = bt;
  }
  return true;
}

bool
Selector::validateModel() {
  if (not EnumeratedType::validateModel()) return false;
  if (not m_template->validateModel()) return false;
  if (hasEitherAttribute()) for (auto & e : eitherAttribute().values()) {
    if (m_types.count(e) == 0) {
      ERROR(ERR_SELECT_UNEXPECTED_EITHER(e));
      return false;
    } else if (not m_types[e]->validateModel()) {
      return false;
    }
  }
  return true;
}

bool
Selector::injectInherited(tree::Object const & r, Object const & o, tree::Value & v) const {
  if (m_parent == nullptr) return false;
  tree::Object const & h = *static_cast<const tree::Object *>(r.parent());
  return m_parent->injectInherited(h, o, v);
}

bool
Selector::checkInstance(tree::Object const & r, tree::Value const & v) const {
  if (not EnumeratedType::checkInstance(r, v)) return false;
  if (v.type() != tree::Value::Type::Object) {
    ERROR(ERR_INSTANCE_NOT_AN_OBJECT);
    return false;
  }
  int score = 0;
  const Model * m = static_cast<const Model *>(owner());
  tree::Object const & o = static_cast<tree::Object const &>(v);
  std::string const & tn = templateAttribute().head();
  for (auto & e : o) {
    BasicType::Ref bt = m->templates().get(tn).clone(e.first);
    bt->setParent(this);
    if (not bt->checkInstance(r, *e.second)) {
      ERROR(ERR_FAILED_CHECKING_INSTANCE(e.first));
      score += 1;
    }
  }
  return score == 0;
}

void
Selector::expandInstance(tree::Object & r, tree::Value & v) {
  EnumeratedType::expandInstance(r, v);
  tree::Object & o = static_cast<tree::Object &>(v);
  for (auto & e : o) {
    if (m_instances.count(e.first) == 0) {
      if (m_types.find(e.first) != m_types.end()) {
        m_instances[e.first] = m_types[e.first];
      } else {
        m_instances[e.first] = m_template->clone(e.first);
        m_instances[e.first]->setParent(this);
      }
    }
    m_instances[e.first]->expandInstance(r, *e.second);
  }
}

bool
Selector::flattenInstance(tree::Object & r, tree::Value & v) {
  if (not EnumeratedType::flattenInstance(r, v)) return false;
  int score = 0;
  tree::Object & o = static_cast<tree::Object &>(v);
  /**
   * We check the size of the objec there, and not in checkInstance because objects
   * can be automatically defaulted to empty objects when all there values can be
   * defaulted, and this step happens in Body::flattenInstance.
   */
  if (not size().check(o.size())) {
    ERROR(ERR_INVALID_SELECT_SIZE(o.size()));
    return false;
  }
  for (auto & e : o) if (not m_instances[e.first]->flattenInstance(r, *e.second)) {
    score += 1;
  }
  return score == 0;
}

bool
Selector::resolveInstance(tree::Object const & r, tree::Value const & v) const {
  if (not EnumeratedType::resolveInstance(r, v)) return false;
  int score = 0;
  tree::Object const & o = static_cast<tree::Object const &>(v);
  for (auto & e : o) if (not m_instances.at(e.first)->resolveInstance(r, *e.second)) {
    score += 1;
  }
  return score == 0;
}

void
Selector::display(Coach::Branch const & br) const {
  Type::display(br);
  const Model * model = static_cast<const Model *>(owner());
  std::string const & n = templateAttribute().head();
  return model->templates().get(n).display(br.push(Coach::Branch::Corner));
}

bool
Selector::explain(tree::Path const & p, tree::Path::const_iterator const & i) const {
  if (i == p.end()) return EnumeratedType::explain(p, i);
  const Model * model = static_cast<const Model *>(owner());
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named : {
      if (hasEitherAttribute()) for (auto & e : eitherAttribute().values()) {
        if (e == (*i)->value()) {
          return m_types.at(e)->explain(p, p.down(i));
        }
      } else {
        std::string const & n = templateAttribute().head();
        return model->templates().get(n).explain(p, p.down(i));
      }
    } break;
    case tree::path::Item::Type::Any : {
      std::string const & n = templateAttribute().head();
      return model->templates().get(n).explain(p, p.down(i));
    } break;
    default: break;
  }
  return false;
}

void
Selector::collectInterfaceIncludes(std::set<std::string> & i) const {
  const Model * m = static_cast<const Model *>(owner());
  std::string const & n = templateAttribute().head();
  BasicType::collectInterfaceIncludes(i);
  i.insert("<map>");
  m->templates().get(n).collectInterfaceIncludes(i);
  i.insert("<string>");
}

void
Selector::collectImplementationIncludes(std::set<std::string> & i) const {
  const Model * m = static_cast<const Model *>(owner());
  std::string const & n = templateAttribute().head();
  BasicType::collectImplementationIncludes(i);
  i.insert("<map>");
  m->templates().get(n).collectImplementationIncludes(i);
  i.insert("<string>");
}

void
Selector::doBuildDefinition(std::string const & s, std::string const & v, std::string const & e,
                                 std::ostream & o, int l) const {
  const Model * m = static_cast<const Model *>(owner());
  std::string const & n = templateAttribute().head();
  auto tmpPath = tempName();
  auto tmpObj = tempName();
  auto tmpIdx = tempName();
  auto tmpTgtPath = tempName();
  std::string path(e);
  if (*--path.end() == '"') {
    path.erase(--path.end());
    path = path + ".\" + ";
  } else {
    path = path + " + \".\" + ";
  }
  BasicType const & bt = m->templates().get(n);
  indent(o, l)      << "auto " << tmpPath << " = ace::tree::Path::parse(" << e << ");" << std::endl;
  indent(o, l)      << "if (r.has(" << tmpPath << ")) {" << std::endl;
  indent(o, l + 2)  << "auto const & " << tmpObj << " =" << std::endl;
  indent(o, l + 4)  << "static_cast<ace::tree::Object const &>(r.get(" << tmpPath << "));";
  o                 << std::endl;
  indent(o, l + 2)  << "for (auto & " << tmpIdx << " : " << tmpObj << ") {" << std::endl;
  indent(o, l + 4)  << "auto " << tmpTgtPath << " = " << path << tmpIdx << ".first;" << std::endl;
  if (not bt.multiple() and bt.optional()) {
    auto tmpHas = tempName();
    auto tmpVal = tempName();
    indent(o, l + 4)  << "bool " << tmpHas << ";" << std::endl;
    indent(o, l + 4)  << bt.typeName() << " " << tmpVal << ";" << std::endl;
    bt.doBuildDefinition(tmpHas, tmpVal, tmpTgtPath, o, l + 4);
    indent(o, l + 4)  << "if (" << tmpHas << ") ";
    o                 << v + "[" + tmpIdx + ".first] = " << tmpVal << ";";
    o                 << std::endl;
  } else {
    bt.doBuildDefinition(s, v + "[" + tmpIdx + ".first]", tmpTgtPath, o, l + 4);
  }
  indent(o, l + 2) << "}" << std::endl;
  indent(o, l)     << "}" << std::endl;
}

void
Selector::doSerializerDefinition(std::string const & c, std::string const & n,
                                 std::string const & v, const bool b,
                                 std::ostream & o, int l) const {
  const Model * m = static_cast<const Model *>(owner());
  std::string const & tm = templateAttribute().head();
  BasicType const & bt = m->templates().get(tm);
  auto tmpObj = tempName();
  auto tmpIdx = tempName();
  int off  = 0;
  if (optional() and not b) {
    off = 2;
    indent(o, l) << "if (m_has_" << m_declName << ") {" << std::endl;
  }
  indent(o, l + off)  << "auto " << tmpObj << " = ace::tree::Object::build(" << n << ");";
  o                   << std::endl;
  indent(o, l + off)  << "for (auto & " << tmpIdx << " : " << v << ") {" << std::endl;
  bt.doSerializerDefinition(tmpObj, tmpIdx + ".first", tmpIdx + ".second", true, o, l + off + 2);
  indent(o, l + off)  << "}" << std::endl;
  indent(o, l + off)  << c << "->put(" << tmpObj << ");" << std::endl;
  if (optional() and not b) {
    indent(o, l) << "}" << std::endl;
  }
}

void
Selector::doGetterInterface(std::ostream & o, int l) const {
  std::string tn = decorateType(typeName()) + " const & ";
  indent(o, l) << "virtual " << tn << m_declName << "() const = 0;" << std::endl;
}

void
Selector::doGetterDeclaration(std::ostream & o, int l) const {
  std::string tn = decorateType(typeName()) + " const & ";
  indent(o, l) << tn << m_declName << "() const;" << std::endl;
}

void
Selector::doGetterDefinition(std::ostream & o, int l) const {
  const Model * m = static_cast<const Model *>(owner());
  std::string const & h = m->normalizedName();
  std::string tn = decorateType(typeName()) + " const & ";
  indent(o, l)      << tn << h << "::" << m_declName << "() const {" << std::endl;
  if (optional()) {
    indent(o, l + 2) << "if (m_" << m_declName << ".empty()) ";
    o << "ace::tree::utils::illegalValueAccess(__PRETTY_FUNCTION__);" << std::endl;
  }
  indent(o, l + 2)  << "return m_" << m_declName << ";" << std::endl;
  indent(o, l)      << "}" << std::endl;
}

BasicType::Ref
Selector::clone(std::string const & n) const {
  Selector * sel = new Selector(*this);
  sel->setName(n);
  for (auto & e : m_types) {
    BasicType::Ref bt = e.second->clone(e.first);
    bt->setParent(sel);
    sel->m_types[e.first] = bt;
  }
  return BasicType::Ref(sel);
}

std::string
Selector::typeName() const {
  std::string const & n = templateAttribute().head();
  const Model * m = static_cast<const Model *>(owner());
  BasicType const & bt = m->templates().get(n);
  std::string tn = "std::map<std::string, " + bt.decorateType(bt.typeName()) + ">";
  return tn;
}

bool
Selector::has(tree::Path const & p, tree::Path::const_iterator const & i) const {
  if (i == p.end()) return true;
  size_t success = 0;
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (m_instances.find((*i)->value()) != m_instances.end()) {
        success += m_instances.at((*i)->value())->has(p, p.down(i)) ? 1 : 0;
      }
    } break;
    case tree::path::Item::Type::Any: {
      if (p.down(i) == p.end() or m_instances.empty()) {
        success += 1;
      } else {
        for (auto & e: m_instances) {
          success += e.second->has(p, p.down(i)) ? 1 : 0;
        }
      }
    } break;
    default: return false;
  }
  if ((*i)->recursive()) for (auto & e : m_instances) {
    success += e.second->has(p, i) ? 1 : 0;
  }
  return success != 0;
}

void
Selector::get(tree::Path const & p, tree::Path::const_iterator const & i,
              std::list<BasicType::Ref> & r) const {
  if (i == p.end()) return;
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (m_instances.find((*i)->value()) != m_instances.end()) {
        if (p.down(i) == p.end()) {
          r.push_back(m_instances.at((*i)->value()));
        } else {
          m_instances.at((*i)->value())->get(p, p.down(i), r);
        }
      }
    } break;
    case tree::path::Item::Type::Any: {
      for (auto & e: m_instances) {
        if (p.down(i) == p.end()) {
          r.push_back(e.second);
        } else {
          e.second->get(p, p.down(i), r);
        }
      }
    } break;
    default: return;
  }
  if ((*i)->recursive()) for (auto & e : m_instances) {
    e.second->get(p, i, r);
  }
}

void
Selector::disable(tree::Path const & p, tree::Path::const_iterator const & i) {
  if (i == p.end()) {
    EnumeratedType::disable(p, i);
    return;
  }
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (m_instances.find((*i)->value()) != m_instances.end()) {
        m_instances.at((*i)->value())->disable(p, p.down(i));
      } else {
        m_template->disable(p, p.down(i));
      }
    } break;
    case tree::path::Item::Type::Any: {
      for (auto & e: m_instances) {
        e.second->disable(p, p.down(i));
      }
      m_template->disable(p, p.down(i));
    } break;
    default: break;
  }
  if ((*i)->recursive()) for (auto & e : m_instances) {
    e.second->disable(p, i);
  }
}

void
Selector::promoteArity(tree::Path const & p, tree::Path::const_iterator const & i) {
  EnumeratedType::promoteArity(p, p.end());
  if (i == p.end()) return;
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (m_instances.find((*i)->value()) != m_instances.end()) {
        m_instances.at((*i)->value())->promoteArity(p, p.down(i));
      }
    } break;
    case tree::path::Item::Type::Any: {
      for (auto & e: m_instances) {
        e.second->promoteArity(p, p.down(i));
      }
    } break;
    default: break;
  }
  if ((*i)->recursive()) for (auto & e : m_instances) {
    e.second->promoteArity(p, i);
  }
}

bool
Selector::isObject() const {
  return true;
}

std::vector<std::string>
Selector::values(tree::Object const & r) const {
  std::vector<std::string> res;
  tree::Value const & v = r.get(m_name);
  tree::Object const & o = static_cast<tree::Object const &>(v);
  for (auto & e : o) res.push_back(e.first);
  return res;
}

Selector::SizeAttributeType const &
Selector::sizeAttribute() const {
  Attribute const & attr = *m_attributes["size"];
  return static_cast<SizeAttributeType const &>(attr);
}

Arity const &
Selector::size() const {
  return sizeAttribute().value();
}

Selector::TemplateAttributeType const &
Selector::templateAttribute() const {
  Attribute const & attr = *m_attributes["template"];
  return static_cast<TemplateAttributeType const &>(attr);
}

BasicType const &
Selector::templateType() const {
  std::string const & n = templateAttribute().head();
  const Model * m = static_cast<const Model *>(owner());
  return m->templates().get(n);
}

} // namespace model
} // namespace ace
