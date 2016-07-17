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

#ifndef ACE_TREE_SCANNER_H_
#define ACE_TREE_SCANNER_H_

#include "Object.h"
#include <memory>
#include <string>

namespace ace {
namespace tree {

class Scanner {
 public:

  using Ref = std::shared_ptr<Scanner>;

 public:

  enum class Format {
    Compact,
    Default,
    Inlined
  };

  Scanner() = default;
  explicit Scanner(Scanner const &) = default;
  virtual ~Scanner() { }

  virtual Value::Ref open(std::string const & fn, int argc, char ** argv) = 0;
  virtual Value::Ref parse(std::string const & s, int argc, char ** argv) = 0;
  virtual void dump(Value const & v, const Format f, std::ostream & o) const = 0;

  virtual std::string name() const = 0;
  virtual std::string extension() const = 0;

 protected:

  static void shift(std::string const & fn, int & argc, char ** & argv);
};

} // namespace tree
} // namespace ace

#endif  // ACE_TREE_SCANNER_H_
