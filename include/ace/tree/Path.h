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

#ifndef ACE_TREE_PATH_H_
#define ACE_TREE_PATH_H_

#include "Item.h"
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace ace {
namespace tree {

class Path {
 public:

  using Container = std::vector<path::Item::Ref>;
  using iterator = typename Container::iterator;
  using const_iterator = typename Container::const_iterator;
  using reverse_iterator = typename Container::reverse_iterator;
  using const_reverse_iterator = typename Container::const_reverse_iterator;
  using Ref = std::shared_ptr<Path>;

  Path() = default;

  static Ref build(Path const & p = Path());
  static Path parse(std::string const & s);

  Path & push(path::Item::Ref const & ir);
  Path sub(const_iterator const & from, const_iterator const & to) const;

  Path merge(Path const & o) const;
  bool generative() const;
  bool global() const;

  bool operator<(Path const & o) const;

  bool operator==(Path const & o) const;
  bool operator!=(Path const & o) const;

  std::string toString(const bool useBrackets = false) const;
  operator std::string() const;

  iterator begin();
  iterator end();

  reverse_iterator rbegin();
  reverse_iterator rend();

  const_iterator begin() const;
  const_iterator end() const;

  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;

  iterator up(iterator const & it) const;
  iterator down(iterator const & it) const;

  const_iterator up(const_iterator const & it) const;
  const_iterator down(const_iterator const & it) const;

 private:

  Container m_items;
};

std::ostream &
operator<<(std::ostream & o, Path const & p);

} // namespace tree
} // namespace ace

#endif  // ACE_TREE_PATH_H_
