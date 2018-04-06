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

#include <ace/model/EnumeratedType.h>
#include <ace/model/FormatChecker.h>
#include <functional>
#include <set>
#include <string>

namespace ace {
namespace model {

// URI comparator

struct URISchemaCompare : public std::binary_function<std::string, std::string, bool> {
  bool operator() (std::string const & a, std::string const & b) const;
};

// URI format checker

class URIFormatChecker : public FormatChecker<std::string> {
 public:

  URIFormatChecker() = delete;
  explicit URIFormatChecker(const BasicType * o);
  bool operator()(tree::Object const & r, tree::Value const & v) const;
};

// URI class

class URI : public EnumeratedType<std::string, false, URISchemaCompare, URIFormatChecker> {
 public:

  enum class Scheme {
    File, HTTP, IPv4, FTP, Undefined
  };

  URI();

  bool validateModel();

  void collectInterfaceIncludes(std::set<std::string> & i) const;
  void collectImplementationIncludes(std::set<std::string> & i) const;

  BasicType::Ref clone(std::string const & n) const;

  static Scheme parseScheme(std::string const & n);
  static std::string toString(const Scheme s);
};

} // namespace model
} // namespace ace
