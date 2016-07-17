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

#ifndef ACE_TYPES_MAC_H_
#define ACE_TYPES_MAC_H_

#include <ace/model/EnumeratedType.h>
#include <ace/model/FormatChecker.h>
#include <functional>
#include <set>
#include <string>

namespace ace {
namespace model {

// MAC format checker

class MACFormatChecker : public FormatChecker<std::string> {
 public:

  MACFormatChecker() = delete;
  explicit MACFormatChecker(const BasicType * o);
  bool operator()(tree::Object const & r, tree::Value const & v) const;
};

// MAC class

class MAC : public EnumeratedType<std::string, true, std::equal_to<std::string>, MACFormatChecker> {
 public:

  MAC();

  void collectInterfaceIncludes(std::set<std::string> & i) const;
  void collectImplementationIncludes(std::set<std::string> & i) const;

  BasicType::Ref clone(std::string const & n) const;
};

} // namespace model
} // namespace ace

#endif  // ACE_TYPES_MAC_H_
