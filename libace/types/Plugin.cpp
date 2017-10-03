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

#include <ace/types/Plugin.h>
#include <ace/types/Class.h>
#include <ace/common/Regex.h>
#include <ace/engine/Master.h>
#include <ace/model/Errors.h>
#include <functional>
#include <iomanip>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace ace {
namespace model {

Plugin::Plugin()
    : Type(BasicType::Kind::Plugin, "?1"),
    m_model(), m_targetArity(Arity::Kind::One, 1, 1), m_plugins(), m_instances() {
  m_attributes.define<ModelAttributeType>("model", false);
  m_attributes.define<ArityAttributeType>("target-arity", true);
}

bool
Plugin::match(tree::Path const & path) const {
  for (auto & e : m_plugins) {
    if (common::Regex::match(path.toString(true), e.first)) {
      return true;
    }
  }
  return false;
}

Class const &
Plugin::getClassFor(tree::Path const & path) const {
  for (auto & e : m_plugins) if (common::Regex::match(path.toString(true), e.first)) {
    return *e.second;;
  }
  throw std::invalid_argument(path);
}

bool
Plugin::checkModel(tree::Value const & t) const {
  if (not Type::checkModel(t)) return false;
  std::string const & n = static_cast<tree::Primitive const &>(t["model"]).value<std::string>();
  DEBUG("Check \"", n, "\"");
  if (not Model::check(nullptr, n)) {
    ERROR(ERR_INVALID_MODEL(n));
    return false;
  }
  Arity arity(Arity::Kind::One, 1, 1);
  if (t.has("target-arity")) {
    auto value = static_cast<tree::Primitive const &>(t["target-arity"]).value<std::string>();
    Arity::parse(value, arity);
    switch (arity) {
      case Arity::Kind::Undefined :
      case Arity::Kind::Disabled :
      case Arity::Kind::UpToOne :
      case Arity::Kind::Any :
        ERROR(ERR_INVALID_TARGET_ARITY(value));
        return false;
      default:
        break;
    }
  }
  return true;
}

void
Plugin::loadModel(tree::Value const & t) {
  Type::loadModel(t);
  std::string const & n = modelAttribute().head();
  DEBUG("Load \"", n, "\"");
  m_model = Model::load(nullptr, n);
  if (m_attributes.has("target-arity")) {
    Attribute::Ref ar = m_attributes["target-arity"];
    m_targetArity = std::static_pointer_cast<ArityAttributeType>(ar)->value();
  }
  for (auto & ch : MASTER.childrenForPath(n)) {
    Model::Ref child = Model::load(nullptr, ch);
    for (auto & tr : child->header().trigger()) {
      DEBUG("Build plugin model \"", ch, "\" for trigger \"", tr, "\"");
      Class::Ref cref = Class::build(tr, this, ch, m_targetArity);
      if (cref != nullptr) m_plugins[tr] = cref;
    }
  }
}

bool
Plugin::flattenModel() {
  if (not Type::flattenModel()) return false;
  DEBUG("Flatten \"", m_model->filePath(), "\"");
  if (not m_model->flattenModel()) return false;
  for (auto & e : m_plugins) {
    DEBUG("Flatten \"", e.second->modelAttribute().model().filePath(), "\"");
    if (not e.second->modelAttribute().model().flattenModel()) return false;
  }
  return true;
}

bool
Plugin::validateModel() {
  if (not Type::validateModel()) return false;
  DEBUG("Validate \"", m_model->filePath(), "\"");
  if (not m_model->validateModel()) return false;
  for (auto & e : m_plugins) {
    DEBUG("Validate \"", e.second->modelAttribute().model().filePath(), "\"");
    if (not e.second->modelAttribute().model().validateModel()) return false;
    if (not e.second->modelAttribute().model().isAnAncestor(*m_model)) {
      ERROR(ERR_MODEL_NOT_AN_ANCESTOR(m_model->name(), e.second->name()));
      return false;
    }
  }
  return true;
}

bool
Plugin::injectInherited(tree::Object const & r, Object const & o, tree::Value & v) const {
  if (m_parent == nullptr) return false;
  tree::Object const & h = *static_cast<const tree::Object *>(r.parent());
  return m_parent->injectInherited(h, o, v);
}

bool
Plugin::checkInstance(tree::Object const & r, tree::Value const & v) const {
  if (not Type::checkInstance(r, v)) return false;
  if (m_plugins.empty()) {
    ERROR(ERR_NO_PLUGIN_TRIGGERS);
    return false;
  }
  if (v.type() != tree::Value::Type::Object) {
    ERROR(ERR_INSTANCE_NOT_AN_OBJECT);
    return false;
  }
  int score = 0;
  tree::Object const & o = static_cast<tree::Object const &>(v);
  for (auto & e : o) {
    bool matchFound = false;
    Class::Ref target = nullptr;
    for (auto & p: m_plugins) {
      if (common::Regex::match(e.second->path().toString(true), p.first)) {
        target = p.second;
        matchFound = true;
        break;
      }
    }
    if (not matchFound) {
      ERROR(ERR_PLUGIN_NO_MATCH_FOUND(e.first));
      score += 1;
    }
    if (target != nullptr) if (not target->checkInstance(r, *e.second)) {
      ERROR(ERR_FAILED_CHECKING_INSTANCE(e.first));
      score += 1;
    }
  }
  return score == 0;
}

void
Plugin::expandInstance(tree::Object & r, tree::Value & v) {
  Type::expandInstance(r, v);
  tree::Object & o = static_cast<tree::Object &>(v);
  for (auto & e : o) {
    if (m_instances.count(e.first) == 0) for (auto & p: m_plugins) {
      if (common::Regex::match(e.second->path().toString(true), p.first)) {
        m_instances[e.first] = std::static_pointer_cast<Class>(p.second->clone(e.first));
        m_instances[e.first]->setParent(this);
      }
    }
    m_instances[e.first]->expandInstance(r, *e.second);
  }
}

bool
Plugin::flattenInstance(tree::Object & r, tree::Value & v) {
  if (not Type::flattenInstance(r, v)) return false;
  int score = 0;
  tree::Object & o = static_cast<tree::Object &>(v);
  for (auto & e : o) if (not m_instances[e.first]->flattenInstance(r, *e.second)) {
    score += 1;
  }
  return score == 0;
}

bool
Plugin::resolveInstance(tree::Object const & r, tree::Value const & v) const {
  if (not Type::resolveInstance(r, v)) return false;
  int score = 0;
  tree::Object const & o = static_cast<tree::Object const &>(v);
  for (auto & e : o) if (not m_instances.at(e.first)->resolveInstance(r, *e.second)) {
    score += 1;
  }
  return score == 0;
}

void
Plugin::display(Coach::Branch const & br) const {
  Type::display(br);
  size_t count = 0;
  for (auto & e : m_plugins) {
    Coach::Branch here;
    count += 1;
    here = br.push(count == m_plugins.size() ? Coach::Branch::Corner : Coach::Branch::Tee);
    e.second->display(here);
  }
}

bool
Plugin::explain(tree::Path const & p, tree::Path::const_iterator const & i) const {
  Type::explain(p, i);
  if (i == p.end()) {
    std::cout << std::setw(13) << std::left << "available"  << ": [" << std::right;
    size_t cnt = 0;
    for (auto & e : m_plugins) {
      std::cout << std::endl;
      indent(std::cout, 18);
      std::cout << e.first;
      cnt += 1;
      if (cnt < m_plugins.size()) std::cout << ",";
    }
    if (cnt != 0) std::cout << std::endl;
    indent(std::cout, cnt == 0 ? 1 : 16) << "]" << std::endl;
    return true;
  } else {
    switch ((*i)->type()) {
      case tree::path::Item::Type::Named : {
        if (m_plugins.find((*i)->value()) != m_plugins.end()) {
          return m_plugins.at((*i)->value())->explain(p, p.down(i));
        } else if ((*i)->value() == "_") {
          return m_model->explain(p, p.down(i));
        } else {
          for (auto & e: m_plugins) if (common::Regex::match(p.toString(true), e.first)) {
            return e.second->explain(p, p.down(i));
          }
        }
      } break;
      case tree::path::Item::Type::Any : {
        if (p.down(i) != p.end()) for (auto & e: m_plugins) {
          e.second->explain(p, p.down(i));
        } else {
          Coach::explain(p, p.down(i));
          size_t maxSize = 0;
          for (auto & e : m_plugins) {
            if (e.first.length() > maxSize) maxSize = e.first.length();
          }
          for (auto & e : m_plugins) {
            std::cout << std::setw(maxSize + 2) << std::left << e.first << " : ";
            std::cout << e.second->modelAttribute().model().filePath();
            std::cout << std::right << std::endl;
          }
        }
      } return true;
      default: break;
    }
  }
  return false;
}

void
Plugin::collectModelFileDependencies(std::set<std::string> & d) const {
  d.insert(m_model->filePath());
  m_model->collectModelFileDependencies(d);
  for (auto & p : m_plugins) p.second->collectModelFileDependencies(d);
}

void
Plugin::collectInterfaceIncludes(std::set<std::string> & i) const {
  BasicType::collectInterfaceIncludes(i);
  i.insert("<algorithm>");
  i.insert("<cctype>");
  i.insert("<map>");
  i.insert("<string>");
  i.insert("<vector>");
  i.insert(m_model->interfaceIncludeStatement(true));
}

void
Plugin::collectImplementationIncludes(std::set<std::string> & i) const {
  BasicType::collectImplementationIncludes(i);
  i.insert("<ace/tree/Utils.h>");
  i.insert("<map>");
  i.insert("<string>");
  i.insert("<vector>");
  for (auto & e : m_plugins) {
    i.insert(e.second->modelAttribute().model().implementationIncludeStatement(true));
  }
  i.insert(m_model->implementationIncludeStatement(true));
}

void
Plugin::doBuildDefinition(std::string const & s, std::string const & v, std::string const & e,
                              std::ostream & o, int l) const {
  std::string const & n = modelAttribute().head();
  std::string const & tn = m_model->definitionType();
  auto tmpPath = tempName();
  auto tmpObj = tempName();
  auto tmpItem = tempName();
  indent(o, l)      << "auto " << tmpPath << " = ace::tree::Path::parse(" << e << ");" << std::endl;
  indent(o, l)      << "if (r.has(" << tmpPath << ")) {" << std::endl;
  indent(o, l + 2)  << "auto & " << tmpObj << " =" << std::endl;
  indent(o, l + 4)  << "static_cast<ace::tree::Object const &>";
  o                 << "(r.get(" << tmpPath << "));" << std::endl;
  indent(o, l + 2)  << "for (auto & " << tmpItem << " : " << tmpObj << ") {" << std::endl;
  indent(o, l + 4)  << "ace::tree::utils::parsePlugin<" << tn << ">" << std::endl;
  indent(o, l + 6)  << "(*" << tmpItem << ".second, " << "\"" << n << "\", " << v
                    << "[" << tmpItem << ".first]);" << std::endl;
  indent(o, l + 2)  << "}" << std::endl;
  indent(o, l)      << "}" << std::endl;
}

void
Plugin::doSerializerDefinition(std::string const & c, std::string const & n,
                               std::string const & v, const bool b,
                               std::ostream & o, int l) const {
  auto tmpObj = tempName();
  auto tmpIdx = tempName();
  auto tmpAry = tempName();
  auto tmpElt = tempName();
  auto tmpVal = tempName();
  int off  = 0;
  if (optional() and not b) {
    off = 2;
    indent(o, l)           << "if (m_has_" << m_declName << ") {" << std::endl;
  }
  indent(o, l + off)       << "auto " << tmpObj << " = ace::tree::Object::build(" << n << ");";
  o                        << std::endl;
  if (m_targetArity == Arity::Kind::One) {
    indent(o, l + off)     << "for (auto & " << tmpIdx << " : " << v << ") {" << std::endl;
    indent(o, l + off + 2) << "auto " << tmpElt << " = ";
    o                      << "ace::tree::Object::build(" << tmpIdx << ".first);";
    o                      << std::endl;
    indent(o, l + off + 2) << tmpIdx << ".second->serialize(" << tmpElt << ");" << std::endl;
    indent(o, l + off + 2) << tmpObj << "->put(" << tmpElt << ");" << std::endl;
    indent(o, l + off)     << "}" << std::endl;
  } else {
    indent(o, l + off)     << "for (auto & " << tmpIdx << " : " << v << ") {" << std::endl;
    indent(o, l + off + 2) << "auto " << tmpAry << " = ";
    o                      << "ace::tree::Array::build(" << tmpIdx << ".first);";
    o                      << std::endl;
    indent(o, l + off + 4) << "for (auto & " << tmpVal << " : " << tmpIdx << ".second) {";
    o                      << std::endl;
    indent(o, l + off + 4) << "auto " << tmpElt << " = " << "ace::tree::Object::build(\"\");";
    o                      << std::endl;
    indent(o, l + off + 4) << tmpVal << "->serialize(" << tmpElt << ");" << std::endl;
    indent(o, l + off + 4) << tmpAry << "->push_back(" << tmpElt << ");" << std::endl;
    indent(o, l + off + 2) << "}" << std::endl;
    indent(o, l + off + 2) << tmpObj << "->put(" << tmpAry << ");" << std::endl;
    indent(o, l + off)     << "}" << std::endl;
  }
  indent(o, l + off)      << c << "->put(" << tmpObj << ");" << std::endl;
  if (optional() and not b) {
    indent(o, l)           << "}" << std::endl;
  }
}

void
Plugin::doGetterInterface(std::ostream & o, int l) const {
  std::string tn = decorateType(typeName()) + " const & ";
  indent(o, l) << "virtual " << tn << m_declName << "() const = 0;" << std::endl;
  if (m_targetArity == Arity::Kind::One) {
    tn = "template<typename T> T const & ";
  } else {
    tn = "template<typename T> std::vector<typename T::Ref> ";
  }
  indent(o, l) << tn << m_declName << "_at(std::string const & n) const {";
  o << std::endl;
  indent(o, l + 2) << "std::string lc_n = n;";
  o << std::endl;
  indent(o, l + 2) << "std::transform(n.begin(), n.end(), lc_n.begin(), ::tolower);";
  o << std::endl;
  if (m_targetArity == Arity::Kind::One) {
    indent(o, l + 2) << m_model->declarationType() << " const & l_base = *";
    o << m_declName << "().at(lc_n);" << std::endl;
    indent(o, l + 2) << "return dynamic_cast<T const &>(l_base);";
    o << std::endl;
  } else {
    indent(o, l + 2) << "std::vector<typename T::Ref> res;";
    o << std::endl;
    indent(o, l + 2) << "for (auto & e : " << m_declName << "().at(lc_n)) {";
    o << std::endl;
    indent(o, l + 4) << "res.push_back(std::dynamic_pointer_cast<T>(e));";
    o << std::endl;
    indent(o, l + 2) << "}";
    o << std::endl;
    indent(o, l + 2) << "return res;";
    o << std::endl;
  }
  indent(o, l) << "}" << std::endl;
}

void
Plugin::doGetterDeclaration(std::ostream & o, int l) const {
  std::string tn = decorateType(typeName()) + " const & ";
  indent(o, l) << tn << m_declName << "() const;" << std::endl;
}

void
Plugin::doGetterDefinition(std::ostream & o, int l) const {
  const Model * m = static_cast<const Model *>(owner());
  std::string const & h = m->normalizedName();
  std::string tn = decorateType(typeName()) + " const & ";
  indent(o, l) << tn << h << "::" << m_declName << "() const {" << std::endl;
  if (optional()) {
    indent(o, l + 2) << "if (m_" << m_declName << ".empty()) ";
    o << "ace::tree::utils::illegalValueAccess(__PRETTY_FUNCTION__);" << std::endl;
  }
  indent(o, l + 2) << "return m_" << m_declName << ";" << std::endl;
  indent(o, l) << "}" << std::endl;
}

bool
Plugin::merge(BasicType const & b) {
  if (not BasicType::merge(b)) return false;
  Plugin const & pg = dynamic_cast<Plugin const &>(b);
  if (m_model->name() != pg.m_model->name()) {
    ERROR(ERR_BASE_MODEL_MISMATCH);
    return false;
  }
  for (auto & e : pg.m_plugins) {
    e.second->setParent(this);
    m_plugins[e.first] = e.second;
  }
  return true;
}

BasicType::Ref
Plugin::clone(std::string const & n) const {
  Plugin * pg = new Plugin(*this);
  pg->setName(n);
  for (auto & e : m_plugins) {
    BasicType::Ref bt = e.second->clone(e.first);
    bt->setParent(pg);
    pg->m_plugins[e.first] = std::static_pointer_cast<Class>(bt);
  }
  return BasicType::Ref(pg);
}

std::string
Plugin::typeName() const {
  if (m_targetArity == Arity::Kind::One) {
    return "std::map<std::string, " + m_model->declarationType() + "::Ref>";
  } else {
    return "std::map<std::string, std::vector<" + m_model->declarationType() + "::Ref>>";
  }
}

bool
Plugin::has(tree::Path const & p, tree::Path::const_iterator const & i) const {
  if (i == p.end()) return true;
  size_t result = 0;
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (m_instances.find((*i)->value()) != m_instances.end()) {
        result += m_instances.at((*i)->value())->has(p, p.down(i)) ? 1 : 0;
      } else {
        auto subp = p.sub(p.begin(), p.down(i));
        for (auto & e: m_instances) if (common::Regex::match(subp.toString(true), e.first)) {
          result += e.second->has(p, p.down(i)) ? 1 : 0;
        }
      }
    } break;
    case tree::path::Item::Type::Any: {
      if (p.down(i) == p.end() or m_instances.empty()) {
        result += 1;
      } else {
        for (auto & e: m_instances) {
          result += e.second->has(p, p.down(i)) ? 1 : 0;
        }
      }
    } break;
    default: return false;
  }
  if ((*i)->recursive()) for (auto & e : m_instances) {
    result += e.second->has(p, i) ? 1 : 0;
  }
  return result != 0;
}

void
Plugin::get(tree::Path const & p, tree::Path::const_iterator const & i,
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
      } else {
        auto subp = p.sub(p.begin(), p.down(i));
        for (auto & e: m_instances) if (common::Regex::match(subp.toString(true), e.first)) {
          if (p.down(i) == p.end()) {
            r.push_back(e.second);
          } else {
            e.second->get(p, p.down(i), r);
          }
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
Plugin::promoteArity(tree::Path const & p, tree::Path::const_iterator const & i) {
  Type::promoteArity(p, p.end());
  if (i == p.end()) return;
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (m_instances.find((*i)->value()) != m_instances.end()) {
        m_instances[(*i)->value()]->promoteArity(p, p.down(i));
        return;
      }
      auto subp = p.sub(p.begin(), p.down(i));
      for (auto & e: m_instances) if (common::Regex::match(subp.toString(), e.first)) {
        e.second->promoteArity(p, p.down(i));
        break;
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

void
Plugin::disable(tree::Path const & p, tree::Path::const_iterator const & i) {
  Type::disable(p, p.end());
  if (i == p.end()) return;
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (m_instances.find((*i)->value()) != m_instances.end()) {
        m_instances[(*i)->value()]->disable(p, p.down(i));
        return;
      }
      auto subp = p.sub(p.begin(), p.down(i));
      for (auto & e: m_instances) if (common::Regex::match(subp.toString(), e.first)) {
        e.second->disable(p, p.down(i));
        break;
      }
    } break;
    case tree::path::Item::Type::Any: {
      for (auto & e: m_instances) {
        e.second->disable(p, p.down(i));
      }
    } break;
    default: break;
  }
  if ((*i)->recursive()) for (auto & e : m_instances) {
    e.second->disable(p, i);
  }
}

bool
Plugin::isObject() const {
  return true;
}

std::vector<std::string>
Plugin::values(tree::Object const & r) const {
  std::vector<std::string> res;
  tree::Value const & v = r.get(m_name);
  tree::Object const & o = static_cast<tree::Object const &>(v);
  for (auto & e : o) res.push_back(e.first);
  return res;
}

Plugin::ModelAttributeType const &
Plugin::modelAttribute() const {
  Attribute const & attr = *m_attributes["model"];
  return static_cast<ModelAttributeType const &>(attr);
}

Model &
Plugin::model() {
  return *m_model;
}

Model const &
Plugin::model() const {
  return *m_model;
}

std::map<std::string, Class::Ref> &
Plugin::plugins() {
  return m_plugins;
}

std::map<std::string, Class::Ref> const &
Plugin::plugins() const {
  return m_plugins;
}

} // namespace model
} // namespace ace
