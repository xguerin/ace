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

#include <ace/filesystem/Directory.h>
#include <ace/tree/Scanner.h>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>

#ifndef MASTER
#define MASTER ace::engine::Master::getInstance()
#endif

namespace ace {
namespace engine {

class Master {
 public:

  typedef void * (*Builder)(tree::Value const & v);

  enum Option {
    None        = 0x00,
    Defaulted   = 0x01,
    Inherited   = 0x02,
    Promoted    = 0x04,
    Undefined   = 0x08,
    Unexpected  = 0x10,
    Relevant    = Defaulted | Inherited | Promoted,
    All         = Defaulted | Inherited | Promoted | Undefined | Unexpected
  };

 public:

  void addModelDirectory(common::Path const & p);
  bool addInlinedModel(std::string const & k, std::string const & s);

  bool hasModel(std::string const & n) const;
  bool isInlinedModel(std::string const & n) const;

  bool addModelBuilder(std::string const & k, std::string const & v, Builder b);

  bool hasModelBuildersFor(std::string const & k) const;
  std::map<std::string, Builder> const & modelBuildersFor(std::string const & k) const;

  bool addChildForPath(std::string const & path, std::string const & ch);
  std::set<std::string> childrenForPath(std::string const & p);

  common::Path modelPathFor(common::Path const & p) const;
  std::string const & modelSourceFor(std::string const & n) const;

  bool addModelPathIntoContext(std::string const & mdp);
  void remModelPathFromContext(std::string const & mdp);

  std::string modelEnvPath() const;

  bool hasScannerByName(std::string const & n) const;
  tree::Scanner & scannerByName(std::string const & n) const;

  bool hasScannerByExtension(std::string const & fn) const;
  tree::Scanner & scannerByExtension(std::string const & fn) const;

  void pushDefaulted(std::string const & p, std::string const & v);
  void pushInherited(std::string const & p, std::string const & f, std::string const & v);
  void pushPromoted(std::string const & p);
  void pushUndefined(std::string const & p);
  void pushUnexpected(std::string const & p);

  std::set<std::string> const & unexpected() const;

  void summarize(std::ostream & o, int filter = Option::Relevant) const;
  void reset();

  static Master & getInstance();

 private:

  Master();

  void loadPluginsAtPath(std::string const & path);
  void collectChildrenForPath(std::string const & p, std::set<std::string> & r) const;

  std::string                                                       m_modelEnvPath;
  std::list<fs::Directory>                                          m_modelDirs;
  std::map<std::string, std::reference_wrapper<const std::string>>  m_inlineModels;
  std::map<std::string, std::map<std::string, Builder>>             m_builders;
  std::map<std::string, std::set<std::string>>                      m_childrenForPath;
  std::set<std::string>                                             m_modelPathContext;
  std::map<std::string, tree::Scanner::Ref>                         m_scannersByName;
  std::map<std::string, tree::Scanner::Ref>                         m_scannersByExtension;
  std::map<std::string, std::string>                                m_defaulted;
  std::map<std::string, std::pair<std::string, std::string>>        m_inherited;
  std::set<std::string>                                             m_promoted;
  std::set<std::string>                                             m_undefined;
  std::set<std::string>                                             m_unexpected;
};

} // namespace engine
} // namespace ace
