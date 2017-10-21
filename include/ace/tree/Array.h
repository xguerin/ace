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

#include "Value.h"
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

namespace ace {
namespace tree {

class Array : public Value {
 public:

  using Ref = std::shared_ptr<Array>;
  using Content = std::vector<Value::Ref>;
  using const_iterator = Content::const_iterator;

 public:

  Array() = delete;
  static Ref build(std::string const & n = "");

  void merge(Value const & o);

  void each(Callback c);
  void each(ConstCallback c) const;

  bool has(std::string const & k) const;
  using Value::has;
  bool has(Path const & p, Path::const_iterator const & i) const;

  size_t size() const;

  Value::Ref const & at(const size_t idx) const;

  Value & operator[](std::string const & k);
  Value const & operator[](std::string const & k) const;

  Value & get(std::string const & k);
  Value const & get(std::string const & k) const;

  using Value::get;

  void get(Path const & p, Path::const_iterator const & i, std::vector<Value::Ref> & r);
  void get(Path const & p, Path::const_iterator const & i, std::vector<Value::Ref> & r) const;

  void erase(std::string const & k);
  using Value::erase;

  void erase(Path const & p, Path::const_iterator const & i);

  void push_back(Value::Ref const & r);

  const_iterator begin() const;
  const_iterator end() const;

 private:

  explicit Array(std::string const & n);

  Content m_content;
};

} // namespace tree
} // namespace ace
