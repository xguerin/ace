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

#include <ostream>
#include <string>

namespace ace {
namespace model {

class Arity {
 public:

  enum class Kind {
    Undefined, Disabled, UpToOne, One, AtLeastOne, Any
  };

  Arity();
  Arity(const Kind kind, const size_t min, const size_t max);

  static bool parse(std::string const & s, Arity & arity);

  bool check(const size_t v) const;
  Kind kind() const;

  Arity intersect(Arity const & o) const;

  bool promote();
  void disable();

  bool isValid() const;

  std::string marker() const;
  std::string toString() const;

  bool operator<=(Arity const & o) const;
  operator Kind () const;

 private:

  Kind    m_kind;
  size_t  m_min;
  size_t  m_max;
};

std::ostream &
operator<<(std::ostream & o, Arity const & arity);

} // namespace model
} // namespace ace
