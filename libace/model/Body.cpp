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

#include <ace/model/Body.h>
#include <ace/model/Errors.h>
#include <ace/engine/Master.h>
#include <ace/tree/Object.h>
#include <ace/types/Class.h>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace ace {
namespace model {

Body::Body(Body const & o)
  : Section(o), Instance(o), Coach(o) {
  for (auto & t : o.m_types) {
    m_types[t.first] = BasicType::Ref(t.second->clone(t.first));
    m_types[t.first]->setParent(this);
  }
}

bool
Body::injectInherited(tree::Object const & r, Object const & o, tree::Value & v) const {
  BasicType const & t = static_cast<BasicType const &>(o);
  if (has(t.name())) {
    BasicType const & h = *m_types.at(t.name());
    if (h <= t and r.has(t.name())) {
      std::string val = "...";
      tree::Value const & w = r.get(t.name());
      if (w.isPrimitive()) {
        tree::Primitive const & p = static_cast<tree::Primitive const &>(w);
        val = p.value();
      }
      MASTER.pushInherited(t.path(), h.path(), val);
      tree::Object & obj = static_cast<tree::Object &>(v);
      obj.put(t.name(), r.at(t.name()));
      return true;
    }
  }
  return m_parent->injectInherited(r, o, v);
}

bool
Body::checkInstance(tree::Object const & r, tree::Value const & v) const {
  if (v.type() != tree::Value::Type::Object) {
    ERROR(ERR_BODY_NOT_AN_OBJECT);
    return false;
  }
  int score = 0;
  tree::Object const & obj = static_cast<tree::Object const &>(v);
  for (auto & e : obj) if (m_types.find(e.first) != m_types.end()) {
    if (not m_types.at(e.first)->checkInstance(r, *e.second)) {
      ERROR(ERR_FAILED_CHECKING_INSTANCE(e.first));
      score += 1;
    }
  }
  for (auto & e : obj) if (m_types.find(e.first) != m_types.end()) {
    BasicType const & bt = *m_types.at(e.first);
    if (bt.hasDependencies()) for (auto & d : bt.dependencies()) {
      if (not d->checkInstance(r, *e.second)) {
        score += 1;
      }
    }
  }
  return score == 0;
}

void
Body::expandInstance(tree::Object & r, tree::Value & v) {
  tree::Object & obj = static_cast<tree::Object &>(v);

  // 1. Inject value for missing required objects

  for (auto & e : m_types) {
    if (e.second->isObject() and not e.second->optional() and not obj.has(e.first)) {
      r.put(e.first, tree::Object::build(e.first));
      MASTER.pushDefaulted(e.first, "{ }");
    }
  }

  // 2. Inject optional values
  // 3. Expand all instances
  // 4. Expand dependencies on all instances
  // 5. Repeat until no more values are generated

  size_t iteration = 1;
  bool keepLooping = false;
  std::map<std::string, bool> injected, depended;

  do {
    DEBUG("Iteration ", iteration++, " of dependency stabilisation loop");
    keepLooping = false;
    for (auto & e : m_types) {
      if (not injected[e.first] and not obj.has(e.first)) {
        if (not e.second->disabled() and not e.second->optional()) {
          if (e.second->mayInherit() and m_parent->injectInherited(r, *e.second, v)) {
            DEBUG("Injected inherited for ", e.second->path());
            keepLooping = true;
          } else {
            keepLooping = e.second->injectDefault(r, v);
          }
          injected[e.first] = true;
        } else {
          DEBUG("Skipping value injection of disabled type ", e.second->path());
        }
      }
    }
    for (auto & e : obj) if (m_types.find(e.first) != m_types.end()) {
      BasicType & bt = *m_types.at(e.first);
      if (not bt.disabled()) {
        bt.expandInstance(r, *e.second);
      } else {
        DEBUG("Skipping expandInstance of disabled type ", bt.path());
      }
    }
    for (auto & e : obj) if (not depended[e.first] and m_types.find(e.first) != m_types.end()) {
      BasicType & bt = *m_types.at(e.first);
      if (bt.hasDependencies()) {
        if (not bt.disabled()) {
          for (auto & d : bt.dependencies()) d->expandInstance(r, *e.second);
          keepLooping = true;
          depended[e.first] = true;
        } else {
          DEBUG("Skipping dependencies of disabled type ", bt.path());
        }
      }
    }
  }
  while (keepLooping);
}

bool
Body::flattenInstance(tree::Object & r, tree::Value & v) {
  int score = 0;
  /**
   * Flatten the enclosed types and dependencies
   */
  for (auto & e : m_types) if (r.has(e.first)) {
    tree::Value & nv = r.get(e.first);
    if (not e.second->flattenInstance(r, nv)) {
      ERROR(ERR_FAILED_FLATTENING_INSTANCE(e.first));
      score += 1;
    }
  }
  for (auto & e : m_types) if (r.has(e.first)) {
    tree::Value & nv = r.get(e.first);
    BasicType const & bt = *e.second;
    if (bt.hasDependencies()) for (auto & d : bt.dependencies()) {
      if (not d->flattenInstance(r, nv)) score += 1;
    }
  }
  /**
   * Check for disabled options
   */
  std::vector<std::string> disabledIds;
  tree::Object & obj = static_cast<tree::Object &>(v);
  for (auto & e : obj) if (m_types.count(e.first) != 0 and m_types.at(e.first)->disabled()) {
    WARNING(ERR_USING_DISABLED_TYPE(e.first));
    disabledIds.push_back(e.first);
  }
  /**
   * Clear disabled options
   */
  for (auto & e : disabledIds) obj.erase(e);
  return score == 0;
}

bool
Body::resolveInstance(tree::Object const & r, tree::Value const & v) const {
  int score = 0;
  tree::Object const & obj = static_cast<tree::Object const &>(v);

  // 1. Check if the declared options are all expected

  for (auto & e : obj) if (m_types.count(e.first) == 0) {
    tree::Path ePath = m_parent->path();
    ePath.push(tree::path::Item::build(tree::path::Item::Type::Named, e.first));
    MASTER.pushUnexpected(ePath);
  }

  // 2. Check if all the required options are present

  for (auto & e : m_types) if (not obj.has(e.first)) {
    if (e.second->optional()) {
      MASTER.pushUndefined(e.second->path());
    } else if (not e.second->disabled()) {
      ERROR(ERR_MISSING_REQUIRED(e.first));
      score += 1;
    }
  }

  // 3. Call resolve instance on the dependencies

  for (auto & e : obj) if (m_types.count(e.first) != 0) {
    BasicType const & bt = *m_types.at(e.first);
    if (not bt.disabled() and bt.hasDependencies()) for (auto & d : bt.dependencies()) {
      if (not d->resolveInstance(r, *e.second)) score += 1;
    }
  }

  // 4. Call resolve instance on all types

  for (auto & e : obj) if (m_types.count(e.first) != 0) {
    BasicType const & bt = *m_types.at(e.first);
    if (not bt.disabled() and not bt.resolveInstance(r, *e.second)) {
      ERROR(ERR_FAILED_RESOLVING_INSTANCE(e.first));
      score += 1;
    }
  }

  return score == 0;
}

void
Body::display(Coach::Branch const & br) const {
  size_t count = 0;
  for (auto & e : m_types) {
    Coach::Branch here;
    count += 1;
    here = br.push(count == m_types.size() ? Coach::Branch::Corner : Coach::Branch::Tee);
    e.second->display(here);
  }
}

bool
Body::explain(tree::Path const & p, tree::Path::const_iterator const & i) const {
  if (i != p.end()) switch ((*i)->type()) {
    case tree::path::Item::Type::Named: {
      if (m_types.find((*i)->value()) != m_types.end()) {
        return m_types.at((*i)->value())->explain(p, p.down(i));
      }
    } break;
    case tree::path::Item::Type::Any: {
      if (p.down(i) != p.end()) for (auto & t : m_types) {
        t.second->explain(p, p.down(i));
      } else {
        size_t maxlen = 0;
        std::string varname;
        for (auto & t : m_types) if (t.first.length() > maxlen) {
          maxlen = t.first.length();
        }
        for (auto & t : m_types) {
          BasicType const & bt = *t.second;
          switch (bt.kind()) {
            case BasicType::Kind::Class:
            case BasicType::Kind::Plugin:
            case BasicType::Kind::Selector: std::cout << " > ";
                                            break;
            default:                        std::cout << " * ";
                                            break;
          }
          std::cout << std::left << std::setw(maxlen + 2) << t.first << " : ";
          std::cout << std::left << bt << std::endl;
        }
      }
    } return true;
    default: break;
  }
  return false;
}

} // namespace model
} // namespace ace
