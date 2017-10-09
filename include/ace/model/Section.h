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

#include "BasicType.h"
#include "Coach.h"
#include "Instance.h"
#include "Object.h"

#include <list>
#include <map>
#include <string>

namespace ace {
namespace model {

class Section : public Object {
 public:

  using Types = std::map<std::string, BasicType::Ref>;
  using iterator = Types::iterator;
  using const_iterator = Types::const_iterator;

 public:

  Section();

  // Object

  tree::Path path(const bool local = false) const;

  virtual bool checkModel(tree::Value const & t) const;
  virtual void loadModel(tree::Value const & t);
  virtual bool flattenModel();
  virtual bool validateModel();

  // Local

  bool merge(Section const & b);
  bool override(Section const & b);

  bool has(std::string const & k) const;
  bool has(tree::Path const & path) const;
  bool has(tree::Path const & p, tree::Path::const_iterator const & i) const;

  BasicType & get(std::string const & k);
  BasicType const & get(std::string const & k) const;

  void get(tree::Path const & path, std::list<BasicType::Ref> & r) const;

  void get(tree::Path const & p, tree::Path::const_iterator const & i,
           std::list<BasicType::Ref> & r) const;

  void promoteArity(tree::Path const & p);
  void promoteArity(tree::Path const & p, tree::Path::const_iterator const & i);

  void disable(tree::Path const & p);
  void disable(tree::Path const & p, tree::Path::const_iterator const & i);

  // Iterators

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

 private:

  bool checkName(std::string const & n) const;
  template<typename T> bool checkType(std::string const & n, tree::Value const & t) const;

 protected:

  bool checkType(std::string const & n, tree::Value const & t) const;
  void loadType(std::string const & n, tree::Value const & t);

  Types m_types;
};

template<typename T>
bool
Section::checkType(std::string const & n, tree::Value const & t) const {
  T type;
  type.setName(n);
  type.setParent(this);
  return type.checkModel(t);
}

} // namespace model
} // namespace ace
