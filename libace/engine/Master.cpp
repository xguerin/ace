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

#include <ace/engine/Master.h>
#include <ace/common/Log.h>
#include <ace/common/Path.h>
#include <ace/common/String.h>
#include <ace/filesystem/Utils.h>
#include <ace/model/Model.h>
#include <ace/tree/Array.h>
#include <ace/tree/Object.h>
#include <ace/tree/Primitive.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <dlfcn.h>

namespace {

static bool
loadPlugin(std::string const & p, ace::tree::Scanner::Ref & result) {
  void * handle = dlopen(p.c_str(), RTLD_NOW | RTLD_GLOBAL);
  if (handle == nullptr) {
    std::cerr << "Cannot load plugin: " << p << std::endl;
    std::cerr << dlerror() << std::endl;
    return false;
  }
  typedef void * (* Getter)();
  Getter getter = reinterpret_cast<Getter>(dlsym(handle, "loadPlugin"));
  if (getter == nullptr) {
    std::cerr << "No valid scanner in " << p << std::endl;
    return false;
  }
  result = ace::tree::Scanner::Ref(reinterpret_cast<ace::tree::Scanner *>(getter()));
  return true;
}

} // namespace

namespace ace {
namespace engine {

Master::Master()
  : m_modelEnvPath(),
  m_modelDirs(),
  m_builders(),
  m_childrenForPath(),
  m_modelPathContext(),
  m_scannersByName(),
  m_scannersByExtension(),
  m_defaulted(),
  m_inherited(),
  m_promoted(),
  m_undefined() {
  addModelDirectory(fs::Directory().path());
  char * env = getenv("ACE_MODEL_PATH");
  if (env != nullptr) {
    ACE_LOG(Warning, "Environment variable ACE_MODEL_PATH is set and will be used");
    m_modelEnvPath = std::string(env);
    std::vector<std::string> elems;
    common::String::split(m_modelEnvPath, ':', elems);
    for (auto & e : elems) if (not e.empty()) {
      auto path = e;
      if (*path.rbegin() != '/') path += "/";
      if (not e.empty()) addModelDirectory(path);
    }
  }
}

void
Master::addModelDirectory(common::Path const & p) {
  if (p.isDirectory()) {
    fs::Directory adir(p);
    if (adir.isValid()) {
      m_modelDirs.push_back(adir);
    } else {
      ACE_LOG(Warning, "Path \"", p, "\" is not a valid directory");
    }
  } else {
    ACE_LOG(Warning, "Path \"", p, "\" is not a directory path (trailing '/' missing)");
  }
}

bool
Master::addInlinedModel(std::string const & k, std::string const & s) {
  if (m_inlineModels.find(k) != m_inlineModels.end()) return false;
  m_inlineModels.emplace(k, s);
  auto root = scannerByName("json").parse(s, 0, nullptr);
  auto const & hdr = static_cast<tree::Object const &>((*root)["header"]);
  if (hdr.has("include")) for (auto & ex : static_cast<tree::Array const &>(hdr["include"])) {
    std::string ch = static_cast<tree::Primitive const &>(*ex).value<std::string>();
    m_childrenForPath[ch].insert(k);
  }
  return true;
}

bool
Master::hasModel(std::string const & n) const {
  if (m_inlineModels.find(n) != m_inlineModels.end()) return true;
  common::Path path(n);
  for (auto & e : m_modelDirs) if (e.has(path)) return true;
  return false;
}

bool
Master::isInlinedModel(std::string const & n) const {
  if (m_inlineModels.find(n) != m_inlineModels.end()) return true;
  return false;
}

bool
Master::addModelBuilder(std::string const & k, std::string const & v, Builder b) {
  if (m_builders.count(k) != 0 and m_builders[k].count(v) != 0) return false;
  m_builders[k][v] = b;
  return true;
}

bool
Master::hasModelBuildersFor(std::string const & k) const {
  return m_builders.count(k) != 0;
}

std::map<std::string, Master::Builder> const &
Master::modelBuildersFor(std::string const & k) const {
  return m_builders.at(k);
}

bool
Master::addChildForPath(std::string const & path, std::string const & ch) {
  if (m_childrenForPath.count(path) != 0 and m_childrenForPath[path].count(ch) != 0) return false;
  m_childrenForPath[ch].insert(path);
  return true;
}

std::set<std::string>
Master::childrenForPath(std::string const & p) {
  std::set<std::string> result;
  collectChildrenForPath(p, result);
  return result;
}

common::Path
Master::modelPathFor(common::Path const & p) const {
  for (auto & e : m_modelDirs) if (e.has(p)) {
    return e.path() / p;
  }
  return common::Path();
}

std::string const &
Master::modelSourceFor(std::string const & n) const {
  return m_inlineModels.at(n);
}

bool
Master::addModelPathIntoContext(std::string const & mdp) {
  if (m_modelPathContext.count(mdp) != 0) return false;
  m_modelPathContext.insert(mdp);
  return true;
}

void
Master::remModelPathFromContext(std::string const & mdp) {
  m_modelPathContext.erase(mdp);
}

bool
Master::hasScannerByName(std::string const & name) const {
  return m_scannersByName.count(name) != 0;
}

tree::Scanner &
Master::scannerByName(std::string const & name) const {
  return *m_scannersByName.at(name);
}

bool
Master::hasScannerByExtension(std::string const & fn) const {
  std::vector<std::string> fnParts;
  common::String::split(fn, '.', fnParts);
  if (fnParts.empty() or fnParts.size() < 2) return false;
  return m_scannersByExtension.count(*fnParts.rbegin()) != 0;
}

tree::Scanner &
Master::scannerByExtension(std::string const & fn) const {
  std::vector<std::string> fnParts;
  common::String::split(fn, '.', fnParts);
  if (fnParts.empty() or fnParts.size() < 2) {
    throw std::invalid_argument("Invalid file name: " + fn);
  }
  return *m_scannersByExtension.at(*fnParts.rbegin());
}

std::set<std::string> const &
Master::unexpected() const {
  return m_unexpected;
}

void
Master::summarize(std::ostream & o, int filter) const {
  if (not m_defaulted.empty() and filter & Option::Defaulted) {
    size_t len = 0;
    o << "[Defaulted options]" << std::endl;
    for (auto & e : m_defaulted) if (e.first.length() > len) len = e.first.length();
    for (auto & e : m_defaulted) {
      o << std::setw(len + 1) << std::left << e.first << std::right;
      o << std::setfill(' ') << "[ " << e.second << " ]" << std::endl;
    }
  }
  if (not m_inherited.empty() and filter & Option::Inherited) {
    size_t flen = 0, vlen = 0;
    o << "[Inherited options]" << std::endl;
    for (auto & e : m_inherited) {
      if (e.first.length() > flen) flen = e.first.length();
    }
    for (auto & e : m_inherited) {
      if (e.second.first.length() > vlen) vlen = e.second.first.length();
    }
    for (auto & e : m_inherited) {
      o << std::setw(flen + 1) << std::left << e.first << std::right;
      o << std::setfill(' ') << "< ";
      o << std::setw(vlen + 1) << e.second.first;
      o << "[ " << e.second.second << " ]" << std::endl;
    }
  }
  if (not m_promoted.empty() and filter & Option::Promoted) {
    o << "[Promoted options]" << std::endl;
    for (auto & v : m_promoted) {
      o << std::left << v << std::right << std::endl;
    }
  }
  if (not m_undefined.empty() and filter & Option::Undefined) {
    o << "[Non-required (omitted) options]" << std::endl;
    for (auto & v : m_undefined) {
      o << std::left << v << std::right << std::endl;
    }
  }
  if (not m_unexpected.empty() and filter & Option::Unexpected) {
    o << "[Unexpected (ignored) options]" << std::endl;
    for (auto & v : m_unexpected) {
      o << std::left << v << std::right << std::endl;
    }
  }
}

void
Master::reset() {
  m_modelDirs.clear();
  m_inlineModels.clear();
  m_builders.clear();
  m_childrenForPath.clear();
  m_modelPathContext.clear();
  m_defaulted.clear();
  m_inherited.clear();
  m_promoted.clear();
  m_undefined.clear();
  m_unexpected.clear();
  addModelDirectory(fs::Directory().path());
}

Master &
Master::getInstance() {
  static std::shared_ptr<Master> singleton = nullptr;
  if (singleton == nullptr) {
    singleton = std::shared_ptr<Master>(new Master());
    std::string paths = fs::Utils::libraryPrefix() + "/lib/";
    char * env = getenv("ACE_SCANNER_PATH");
    if (env != nullptr) paths = std::string(env);
    std::vector<std::string> elems;
    common::String::split(paths, ':', elems);
    for (auto & e : elems) if (not e.empty()) {
      auto path = e;
      if (*path.rbegin() != '/') path += "/";
      if (not e.empty()) singleton->loadPluginsAtPath(path);
    }
  }
  return *singleton;
}

std::string
Master::modelEnvPath() const {
  return m_modelEnvPath;
}

void
Master::pushDefaulted(std::string const & p, std::string const & v) {
  m_defaulted[p] = v;
}

void
Master::pushInherited(std::string const & p, std::string const & f,
                           std::string const & v) {
  m_inherited[p] = { f, v };
}

void
Master::pushPromoted(std::string const & p) {
  m_promoted.insert(p);
}

void
Master::pushUndefined(std::string const & p) {
  m_undefined.insert(p);
}

void
Master::pushUnexpected(std::string const & p) {
  m_unexpected.insert(p);
}

void
Master::loadPluginsAtPath(std::string const & path) {
  common::Path libpath(path);
  fs::Directory dir(libpath);
  dir.each([&](ace::fs::Node const & node) {
    if (ace::fs::Node::type(node.path(), false) != ace::fs::Node::Type::Regular) {
      return;
    }
    std::string name = *node.path().rbegin();
    if (name.find("_format") == std::string::npos) return;
    tree::Scanner::Ref scanner;
    if (not ::loadPlugin(node.path().toString(), scanner)) {
      return;
    }
    if (m_scannersByName.count(scanner->name()) == 0) {
      ACE_LOG(Info, "[", std::setw(6), scanner->name(), "] ", node.path());
      m_scannersByName[scanner->name()] = scanner;
      m_scannersByExtension[scanner->extension()] = scanner;
    }
  });
}

void
Master::collectChildrenForPath(std::string const & p, std::set<std::string> & r) const {
  if (m_childrenForPath.count(p) != 0) for (auto & e : m_childrenForPath.at(p)) {
    r.insert(e);
    collectChildrenForPath(e, r);
  }
}

} // namespace engine
} // namespace ace

