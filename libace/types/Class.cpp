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

#include <ace/types/Class.h>
#include <ace/model/Errors.h>
#include <ace/tree/Checker.h>
#include <functional>
#include <list>
#include <set>
#include <stdexcept>
#include <string>

namespace ace { namespace model {

Class::Class() : Type(BasicType::Kind::Class)
{
  m_attributes.define<ModelAttribute>("model", false);
  m_attributes.define<FlatAttribute>("flat", true);
}

Class::Ref
Class::build(std::string const& n, const Object* p, std::string const& m,
             Arity const& a)
{
  tree::Object::Ref root = tree::Object::build();
  root->put(tree::Primitive::build("kind", "class"));
  root->put(tree::Primitive::build("arity", a.toString()));
  root->put(tree::Primitive::build("model", m));
  root->put(tree::Primitive::build("doc", "Generated class hook"));
  Ref cls(new Class());
  cls->setName(n);
  cls->setParent(p);
  if (not cls->checkModel(*root)) {
    return nullptr;
  }
  cls->loadModel(*root);
  return cls;
}

bool
Class::checkInstance(tree::Object const& r, tree::Value const& v) const
{
  if (not Type::checkInstance(r, v)) {
    return false;
  }
  int score = 0;
  if (not multiple() and v.type() != tree::Value::Type::Object) {
    ERROR(ERR_INSTANCE_NOT_AN_OBJECT);
    return false;
  }
  v.each([&](tree::Value const& w) {
    if (w.type() != tree::Value::Type::Object) {
      ERROR(ERR_CLASS_NOT_AN_OBJECT);
      score += 1;
    }
    if (not modelAttribute().model().checkInstance(w)) {
      score += 1;
    }
  });
  return score == 0;
}

// We need to clone the models here to create different hierarchy
// branches for each instance in the array. This is needed as the model
// can be altered individually for each element.

void
Class::expandInstance(tree::Object& r, tree::Value& v)
{
  Type::expandInstance(r, v);
  if (m_clones.empty()) {
    v.each([&](tree::Value& w) {
      Model::Ref mdl = Model::Ref(new Model(modelAttribute().model()));
      mdl->setParent(this);
      m_clones.push_back(mdl);
    });
  }
  size_t index = 0;
  v.each([&](tree::Value& w) { m_clones[index++]->expandInstance(w); });
}

bool
Class::flattenInstance(tree::Object& r, tree::Value& v)
{
  if (not Type::flattenInstance(r, v)) {
    return false;
  }
  int score = 0;
  int index = 0;
  v.each([&](tree::Value& w) {
    if (not m_clones[index++]->flattenInstance(w)) {
      score += 1;
    }
  });
  return score == 0;
}

bool
Class::resolveInstance(tree::Object const& r, tree::Value const& v) const
{
  if (not Type::resolveInstance(r, v)) {
    return false;
  }
  int score = 0;
  int index = 0;
  v.each([&](tree::Value const& w) {
    if (not m_clones[index++]->resolveInstance(w)) {
      score += 1;
    }
  });
  return score == 0;
}

bool
Class::validateModel()
{
  if (!hasFlatAttribute() or !flatAttribute().head()) {
    return Type::validateModel();
  }
  if (multiple()) {
    ERROR(ERR_INVALID_FLAT_CLASS_ARITY);
    return false;
  }
  return true;
}

bool
Class::injectInherited(tree::Object const& r, Object const& o,
                       tree::Value& v) const
{
  if (m_parent == nullptr) {
    return false;
  }
  if (r.parent()->type() == tree::Value::Type::Object) {
    tree::Object const& h = *static_cast<const tree::Object*>(r.parent());
    return m_parent->injectInherited(h, o, v);
  } else {
    tree::Object const& h =
      *static_cast<const tree::Object*>(r.parent()->parent());
    return m_parent->injectInherited(h, o, v);
  }
}

void
Class::display(Coach::Branch const& br) const
{
  Type::display(br);
  modelAttribute().model().display(br);
}

bool
Class::explain(tree::Path const& p, tree::Path::const_iterator const& i) const
{
  if (i == p.end()) {
    return Type<void, FormatChecker<void>>::explain(p, i);
  }
  return modelAttribute().model().explain(p, i);
}

void
Class::collectModelFileDependencies(std::set<std::string>& d) const
{
  d.insert(modelAttribute().model().filePath());
  modelAttribute().model().collectModelFileDependencies(d);
}

void
Class::collectInterfaceIncludes(std::set<std::string>& i) const
{
  BasicType::collectInterfaceIncludes(i);
  i.insert(modelAttribute().model().interfaceIncludeStatement(true));
}

void
Class::collectImplementationIncludes(std::set<std::string>& i) const
{
  BasicType::collectImplementationIncludes(i);
  i.insert(modelAttribute().model().implementationIncludeStatement(true));
}

void
Class::doBuildDefinition(std::string const& s, std::string const& v,
                         std::string const& e, std::ostream& o, int l) const
{
  std::string const& tn = modelAttribute().model().definitionType();
  indent(o, l);
  if (optional() and not multiple()) {
    o << s << " = ";
  }
  o << "ace::tree::utils::parseObject<" << tn << ">(r, " + e + ", " + v << ");";
  o << std::endl;
}

void
Class::doSerializerDefinition(std::string const& c, std::string const& n,
                              std::string const& v, const bool b,
                              std::ostream& o, int l) const
{
  if (not multiple()) {
    int off = 0;
    auto tmpObj = tempName();
    if (optional() and not b) {
      off = 2;
      indent(o, l) << "if (m_has_" << m_declName << ") {" << std::endl;
    }
    indent(o, l + off) << "auto " << tmpObj << " = ace::tree::Object::build("
                       << n << ");";
    o << std::endl;
    indent(o, l + off) << v << "->serialize(" << tmpObj << ");" << std::endl;
    indent(o, l + off) << c << "->put(" << tmpObj << ");" << std::endl;
    if (optional() and not b) {
      indent(o, l) << "}" << std::endl;
    }
  } else {
    auto tmpArray = tempName();
    auto tmpIndex = tempName();
    auto tmpObj = tempName();
    indent(o, l) << "if (not " << v << ".empty()) {" << std::endl;
    indent(o, l + 2) << "auto " << tmpArray << "= ace::tree::Array::build(" << n
                     << ");";
    o << std::endl;
    indent(o, l + 2) << "for (auto & " << tmpIndex << " : " << v << ") {"
                     << std::endl;
    indent(o, l + 4) << "auto " << tmpObj
                     << "= ace::tree::Object::build(\"\");";
    o << std::endl;
    indent(o, l + 4) << tmpIndex << "->serialize(" << tmpObj << ");"
                     << std::endl;
    indent(o, l + 4) << tmpArray << "->push_back(" << tmpObj << ");"
                     << std::endl;
    indent(o, l + 2) << "}" << std::endl;
    indent(o, l + 2) << c << "->put(" << tmpArray << ");" << std::endl;
    indent(o, l) << "}" << std::endl;
  }
}

void
Class::doGetterInterface(std::ostream& o, int l) const
{
  std::string tn = modelAttribute().model().declarationType();
  if (!hasFlatAttribute() or !flatAttribute().head()) {
    tn += "::Ref";
  }
  tn = decorateType(tn);
  indent(o, l) << "virtual " << tn << " const & " << m_declName
               << "() const = 0;" << std::endl;
}

void
Class::doGetterDeclaration(std::ostream& o, int l) const
{
  std::string tn = modelAttribute().model().declarationType();
  if (!hasFlatAttribute() or !flatAttribute().head()) {
    tn += "::Ref";
  }
  tn = decorateType(tn);
  indent(o, l) << tn << " const & " << m_declName << "() const;" << std::endl;
}

void
Class::doGetterDefinition(std::ostream& o, int l) const
{
  const Model* m = static_cast<const Model*>(owner());
  std::string const& h = m->normalizedName();
  std::string tn = modelAttribute().model().declarationType();
  if (!hasFlatAttribute() or !flatAttribute().head()) {
    tn += "::Ref";
  }
  tn = decorateType(tn);
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
  if (!hasFlatAttribute() or !flatAttribute().head()) {
    indent(o, l + 2) << "return m_" << m_declName << ";" << std::endl;
  } else {
    indent(o, l + 2) << "return *m_" << m_declName << ";" << std::endl;
  }
  indent(o, l) << "}" << std::endl;
}

BasicType::Ref
Class::clone(std::string const& n) const
{
  Class* cls = new Class(*this);
  cls->setName(n);
  return BasicType::Ref(cls);
}

std::string
Class::typeName() const
{
  std::string tn = modelAttribute().model().declarationType() + "::Ref";
  return tn;
}

bool
Class::has(tree::Path const& p, tree::Path::const_iterator const& i) const
{
  if (i == p.end()) {
    return true;
  }
  size_t success = 0;
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (not multiple() and not m_clones.empty()) {
        success += m_clones[0]->body().has(p, i) ? 1 : 0;
      }
    } break;
    case tree::path::Item::Type::Any: {
      if (multiple()) {
        if (p.down(i) != p.end()) {
          for (auto& e : m_clones) {
            success += e->body().has(p, p.down(i)) ? 1 : 0;
          }
        } else {
          success += 1;
        }
      } else if (not m_clones.empty()) {
        success += m_clones[0]->body().has(p, i) ? 1 : 0;
      } else {
        success += 1;
      }
    } break;
    case tree::path::Item::Type::Indexed: {
      if (multiple()) {
        for (auto const& index : (*i)->indexes()) {
          if (m_clones.size() >= index) {
            success += m_clones[index]->body().has(p, p.down(i)) ? 1 : 0;
          }
        }
      }
    } break;
    default:
      return false;
  }
  if ((*i)->recursive()) {
    for (auto& e : m_clones) {
      success += e->body().has(p, i) ? 1 : 0;
    }
  }
  return success != 0;
}

void
Class::get(tree::Path const& p, tree::Path::const_iterator const& i,
           std::list<BasicType::Ref>& r) const
{
  if (i == p.end()) {
    return;
  }
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (not multiple() and not m_clones.empty()) {
        m_clones[0]->body().get(p, i, r);
      }
    } break;
    case tree::path::Item::Type::Any: {
      if (multiple()) {
        for (auto& e : m_clones) {
          e->body().get(p, p.down(i), r);
        }
      } else if (not m_clones.empty()) {
        m_clones[0]->body().get(p, i, r);
      }
    } break;
    default:
      return;
  }
  if ((*i)->recursive()) {
    for (auto& e : m_clones) {
      e->body().get(p, i, r);
    }
  }
}

// Because of the separated hierarchy, we also need to promoteArity
// on the clones. The arity promotion is done on all the clones.

void
Class::promoteArity(tree::Path const& p, tree::Path::const_iterator const& i)
{
  Type<void, FormatChecker<void>>::promoteArity(p, p.end());
  if (i == p.end()) {
    return;
  }
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (not multiple() and not m_clones.empty()) {
        m_clones[0]->body().promoteArity(p, i);
      }
    } break;
    case tree::path::Item::Type::Any: {
      if (multiple()) {
        for (auto& e : m_clones) {
          e->body().promoteArity(p, p.down(i));
        }
      } else if (not m_clones.empty()) {
        m_clones[0]->body().promoteArity(p, i);
      }
    } break;
    case tree::path::Item::Type::Indexed: {
      if (multiple()) {
        for (auto const& index : (*i)->indexes()) {
          if (m_clones.size() >= index) {
            m_clones[index]->body().promoteArity(p, p.down(i));
          }
        }
      }
    } break;
    default:
      return;
  }
  if ((*i)->recursive()) {
    for (auto& e : m_clones) {
      e->body().promoteArity(p, i);
    }
  }
}

void
Class::disable(tree::Path const& p, tree::Path::const_iterator const& i)
{
  if (i == p.end()) {
    Type<void, FormatChecker<void>>::disable(p, i);
    return;
  }
  switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (not multiple() and not m_clones.empty()) {
        m_clones[0]->body().disable(p, i);
      }
    } break;
    case tree::path::Item::Type::Any: {
      if (multiple()) {
        for (auto& e : m_clones) {
          e->body().disable(p, p.down(i));
        }
      } else if (not m_clones.empty()) {
        m_clones[0]->body().disable(p, i);
      }
    } break;
    case tree::path::Item::Type::Indexed: {
      if (multiple()) {
        for (auto const& index : (*i)->indexes()) {
          if (m_clones.size() >= index) {
            m_clones[index]->body().disable(p, p.down(i));
          }
        }
      }
    } break;
    default:
      return;
  }
  if ((*i)->recursive()) {
    for (auto& e : m_clones) {
      e->body().disable(p, i);
    }
  }
}

bool
Class::isObject() const
{
  return true;
}

ModelAttribute&
Class::modelAttribute()
{
  Attribute& attr = *m_attributes["model"];
  return static_cast<ModelAttribute&>(attr);
}

ModelAttribute const&
Class::modelAttribute() const
{
  Attribute const& attr = *m_attributes["model"];
  return static_cast<ModelAttribute const&>(attr);
}

bool
Class::hasFlatAttribute() const
{
  return this->m_attributes.has("flat");
}

Class::FlatAttribute&
Class::flatAttribute()
{
  Attribute& attr = *m_attributes["flat"];
  return static_cast<FlatAttribute&>(attr);
}

Class::FlatAttribute const&
Class::flatAttribute() const
{
  Attribute const& attr = *m_attributes["flat"];
  return static_cast<FlatAttribute const&>(attr);
}

}}
