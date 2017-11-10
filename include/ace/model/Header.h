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

#include "Author.h"
#include "Coach.h"
#include "Object.h"
#include <ace/tree/Path.h>
#include <list>
#include <string>
#include <vector>

namespace ace {
namespace model {

class Header : public Object, public Coach {
 public:

  Header();

  tree::Path path(const bool local = false) const;

  void loadModel(tree::Value const & t);
  bool checkModel(tree::Value const & t) const;

  bool explain(tree::Path const & p, tree::Path::const_iterator const & i) const;

  std::string package() const;
  bool hasAuthor() const;
  Author const & author() const;
  std::list<std::string> const & nameSpace() const;
  std::list<std::string> const & include() const;
  std::list<tree::Path> const & trigger() const;
  std::string const & version() const;
  std::string const & doc() const;

 private:

  std::vector<std::string>  m_package;
  bool                      m_hasAuthor;
  Author                    m_author;
  std::list<std::string>    m_nameSpace;
  std::list<std::string>    m_include;
  std::list<tree::Path>     m_trigger;
  std::string               m_version;
  std::string               m_doc;
};

} // namespace model
} // namespace ace
