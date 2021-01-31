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

#include <memory>
#include <string>
#include <vector>

namespace ace { namespace tree { namespace path {

class Item
{
public:
  using Ref = std::shared_ptr<Item>;

  enum class Type
  {
    Any,
    Local,
    Global,
    Indexed,
    Named,
    Ranged
  };

  struct Range
  {
    size_t low;
    size_t high;
    size_t steps;

    bool operator==(Range const& o) const;
    bool operator!=(Range const& o) const;
    bool contains(const size_t v) const;
  };

  Item() = delete;

  static Ref build(const Type t);
  static Ref build(const Type t, std::vector<size_t> const& idx);
  static Ref build(const Type t, std::string const& n);
  static Ref build(const Type t, Range const& rng);

  explicit Item(const Type t);
  Item(const Type t, std::vector<size_t> const& idx);
  Item(const Type t, std::string const& n);
  Item(const Type t, Range const& rng);

  void setRecursive();
  bool recursive() const;

  Type type() const;
  std::vector<size_t> const& indexes() const;
  Range const& range() const;
  bool root() const;
  std::string const& value() const;

  bool operator==(Item const& o) const;
  bool operator!=(Item const& o) const;

  std::string toString(const bool useBrackets = false) const;

private:
  Type m_type;
  bool m_rec;
  std::vector<size_t> m_indexes;
  std::string m_value;
  Range m_range;
};

std::ostream& operator<<(std::ostream& o, Item const& p);

}}}
