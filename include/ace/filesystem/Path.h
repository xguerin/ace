/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
filesystem* copies of the Software, and to permit persons to whom the Software is
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

#include <ostream>
#include <string>
#include <vector>

namespace ace {
namespace fs {

class Path {
 public:

  using Container = std::vector<std::string>;

  using iterator = Container::iterator;
  using const_iterator = Container::const_iterator;

  using reverse_iterator = Container::reverse_iterator;
  using const_reverse_iterator = Container::const_reverse_iterator;

  Path() = default;
  Path(std::string const & v, bool enforceDir = false);

  bool operator==(Path const & o) const;
  bool operator!=(Path const & o) const;

  Path operator-(Path const & o) const;
  Path operator/(Path const & o) const;

  std::string toString() const;
  Path prune() const;

  Path compress() const;

  bool isAbsolute() const;
  bool isDirectory() const;

  static std::string prefix(std::string const & a, std::string const & b);
  static Path prefix(Path const & a, Path const & b);

  bool empty() const;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;

  reverse_iterator rend();
  const_reverse_iterator rend() const;

  iterator up(iterator const & i);
  const_iterator up(const_iterator const & i) const;

  iterator down(iterator const & i);
  const_iterator down(const_iterator const & i) const;

  reverse_iterator up(reverse_iterator const & i);
  const_reverse_iterator up(const_reverse_iterator const & i) const;

  reverse_iterator down(reverse_iterator const & i);
  const_reverse_iterator down(const_reverse_iterator const & i) const;

 private:

  explicit Path(std::vector<std::string> const & c);

  std::vector<std::string>  m_elements;

  friend std::ostream & operator<<(std::ostream & o, Path const & p);
};

std::ostream &
operator<<(std::ostream & o, Path const & p);

} // namespace fs
} // namespace ace
