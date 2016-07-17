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

#ifndef ACE_TREE_OBJECT_H_
#define ACE_TREE_OBJECT_H_

#include "Value.h"
#include <ace/common/Path.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ace {
namespace tree {

class Object : public Value {
 public:

  using Ref = std::shared_ptr<Object>;
  using Content = std::map<std::string, Value::Ref>;
  using const_iterator = Content::const_iterator;

 public:

  Object() = delete;
  static Ref build(std::string const & n = "");

  void merge(Value const & o);

  bool has(std::string const & k) const;
  using Value::has;
  bool has(common::Path const & p, common::Path::const_iterator const & i) const;
  bool has(Path const & p, Path::const_iterator const & i) const;

  size_t size() const;

  Value::Ref const & at(std::string const & k) const;

  void put(Value::Ref const & r);
  void put(std::string const & k, Value::Ref const & r);
  bool put(common::Path const & p, Value::Ref const & r);

  Value & operator[](std::string const & k);
  Value const & operator[](std::string const & k) const;

  Value & get(std::string const & k);
  Value const & get(std::string const & k) const;

  using Value::get;

  Value & get(common::Path const & p, common::Path::const_iterator const & i);
  Value const & get(common::Path const & p, common::Path::const_iterator const & i) const;

  void get(Path const & p, Path::const_iterator const & i, std::vector<Value::Ref> & r);
  void get(Path const & p, Path::const_iterator const & i, std::vector<Value::Ref> & r) const;

  void erase(std::string const & k);
  using Value::erase;

  void erase(common::Path const & p, common::Path::const_iterator const & i);
  void erase(Path const & p, Path::const_iterator const & i);

  const_iterator begin() const;
  const_iterator end() const;

  Path path() const;

 private:

  explicit Object(std::string const & n);

  bool put(common::Path const & p, common::Path::const_iterator const & i, Value::Ref const & r);

  Content m_content;
};

} // namespace tree
} // namespace ace

#endif  // ACE_TREE_OBJECT_H_
