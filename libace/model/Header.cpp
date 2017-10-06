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

#include <ace/model/Header.h>
#include <ace/model/Errors.h>
#include <ace/engine/Master.h>
#include <ace/tree/Array.h>
#include <ace/tree/Checker.h>
#include <ace/tree/Primitive.h>
#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

namespace ace {
namespace model {

Header::Header() : m_author(), m_version(), m_doc() {
  m_author.setParent(this);
}

tree::Path
Header::path(const bool local) const {
  return m_parent->path(local);
}

bool
Header::checkModel(tree::Value const & t) const {
  const ace::tree::Checker::Schema jsonSchema = {
    { "package"  , { ace::tree::Value::Type::Array , true  } },
    { "author"   , { ace::tree::Value::Type::Object, false } },
    { "namespace", { ace::tree::Value::Type::Array , true  } },
    { "include"  , { ace::tree::Value::Type::Array , true  } },
    { "trigger"  , { ace::tree::Value::Type::Array , true  } },
    { "version"  , { ace::tree::Value::Type::String, false } },
    { "doc"      , { ace::tree::Value::Type::String, false } }
  };
  ace::tree::Checker chk(path(), t);
  if (not chk.validate(jsonSchema)) return false;
  if (not m_author.checkModel(t["author"])) return false;
  if (static_cast<tree::Primitive const &>(t["doc"]).value<std::string>().empty()) {
    ERROR(ERR_EMPTY_DOC);
    return false;
  }
  if (t.has("package")) for (auto & ex : static_cast<tree::Array const &>(t["package"])) {
    if (ex->type() != tree::Value::Type::String) {
      ERROR(ERR_WRONG_PACKAGE_TYPE);
      return false;
    }
    auto const & pri = static_cast<tree::Primitive const &>(*ex);
    if (pri.value<std::string>().empty()) {
      ERROR(ERR_WRONG_PACKAGE_TYPE);
      return false;
    }
  }
  if (t.has("namespace")) for (auto & ex : static_cast<tree::Array const &>(t["namespace"])) {
    if (ex->type() != tree::Value::Type::String) {
      ERROR(ERR_WRONG_NS_TYPE);
      return false;
    }
    auto const & pri = static_cast<tree::Primitive const &>(*ex);
    if (pri.value<std::string>().empty()) {
      ERROR(ERR_WRONG_NS_TYPE);
      return false;
    }
  }
  if (t.has("include")) for (auto & ex : static_cast<tree::Array const &>(t["include"])) {
    if (ex->type() != tree::Value::Type::String) {
      ERROR(ERR_WRONG_INCLUDE_TYPE);
      return false;
    }
    auto const & pri = static_cast<tree::Primitive const &>(*ex);
    if (pri.value<std::string>().empty()) {
      ERROR(ERR_WRONG_INCLUDE_VALUE);
      return false;
    }
    if (not MASTER.hasModel(pri.value<std::string>())) {
      ERROR(ERR_MODEL_NOT_FOUND(pri.value<std::string>()));
      return false;
    }
  }
  if (t.has("trigger")) for (auto & ex : static_cast<tree::Array const &>(t["trigger"])) {
    if (ex->type() != tree::Value::Type::String) {
      ERROR(ERR_WRONG_TRIGGER_TYPE);
      return false;
    }
    auto const & pri = static_cast<tree::Primitive const &>(*ex);
    if (pri.value<std::string>().empty()) {
      ERROR(ERR_EMPTY_TRIGGER);
      return false;
    }
    try {
      auto path = tree::Path::parse(pri.value<std::string>());
      if (!path.global()) {
        ERROR(ERR_WRONG_TRIGGER_SCOPE);
        return false;
      }
    }
    catch(std::invalid_argument const & e) {
      ERROR(ERR_WRONG_TRIGGER_FORMAT);
      return false;
    }
  }
  return true;
}

void
Header::loadModel(tree::Value const & t) {
  m_include.clear();
  m_author.loadModel(t["author"]);
  m_version = static_cast<tree::Primitive const &>(t["version"]).value<std::string>();
  m_doc = static_cast<tree::Primitive const &>(t["doc"]).value<std::string>();
  if (t.has("package")) for (auto & ex : static_cast<tree::Array const &>(t["package"])) {
    m_package.push_back(static_cast<tree::Primitive const &>(*ex).value<std::string>());
  }
  if (t.has("namespace")) for (auto & ex : static_cast<tree::Array const &>(t["namespace"])) {
    m_nameSpace.push_back(static_cast<tree::Primitive const &>(*ex).value<std::string>());
  }
  if (t.has("include")) for (auto & ex : static_cast<tree::Array const &>(t["include"])) {
    m_include.push_back(static_cast<tree::Primitive const &>(*ex).value<std::string>());
  }
  if (t.has("trigger")) for (auto & ex : static_cast<tree::Array const &>(t["trigger"])) {
    auto path = tree::Path::parse(static_cast<tree::Primitive const &>(*ex).value<std::string>());
    m_trigger.push_back(path);
  }
}

bool
Header::explain(tree::Path const & p, tree::Path::const_iterator const & i) const {
  if (i != p.end()) return false;
  if (m_include.size() != 0) {
    std::cout << " * Include : ";
    for (auto & e : m_include) {
      std::cout << e;
      if (e != *m_include.rbegin()) std::cout << std::endl << "           | ";
    }
    std::cout << std::endl;
  }
  std::cout << " * Author  : " << m_author << std::endl;
  std::cout << " * Summary : " << m_doc << std::endl;
  return true;
}

std::string
Header::package() const {
  std::ostringstream oss;
  for (auto & e : m_package) oss << e << "/";
  return oss.str();
}

Author const &
Header::author() const {
  return m_author;
}

std::list<std::string> const &
Header::nameSpace() const {
  return m_nameSpace;
}

std::list<std::string> const &
Header::include() const {
  return m_include;
}

std::list<tree::Path> const &
Header::trigger() const {
  return m_trigger;
}

std::string const &
Header::version() const {
  return m_version;
}

std::string const &
Header::doc() const {
  return m_doc;
}

} // namespace model
} // namespace ace
