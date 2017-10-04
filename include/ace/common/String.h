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

#ifndef ACE_COMMON_STRING_H_
#define ACE_COMMON_STRING_H_

#include <algorithm>
#include <cctype>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <locale>

namespace ace {
namespace common {
namespace String {

std::ostream &
indent(std::ostream & o, int l);

template<typename T>
bool
is(std::string const & s) {
  T r;
  std::istringstream iss(s);
  iss >> std::boolalpha >> r;
  return (iss.rdstate() & (std::istream::failbit | std::istream::badbit)) == 0;
}

template<>
bool
is<long>(std::string const & s);

template<>
bool
is<double>(std::string const & s);

template<typename T>
T
value(std::string const & s) {
  T r;
  std::istringstream iss(s);
  iss >> std::boolalpha >> r;
  return r;
}

template<>
double
value<double>(std::string const & s);

template<>
std::string
value(std::string const & s);

template<typename T>
std::string
from(T const & v) {
  std::ostringstream oss;
  oss << std::boolalpha << std::scientific << v;
  return oss.str();
}

template<template <class, class> class C, class A = std::allocator<std::string>>
void
split(std::string const & s, const char d, C<std::string, A> & r) {
  r.clear();
  if (s.empty()) return;
  std::string buffer;
  for (auto & c : s) {
    if (c == d) {
      r.push_back(buffer);
      buffer.clear();
    } else {
      buffer.push_back(c);
    }
  }
  r.push_back(buffer);
}

template<template <class, class> class C, class A = std::allocator<std::string>>
std::string
join(C<std::string, A> const & r, const char d) {
  std::ostringstream oss;
  size_t cnt = 0;
  for (auto & e : r) {
    oss << e;
    if (cnt++ < r.size() - 1) oss << d;
  }
  return oss.str();
}

std::string
prefix(std::string const & a, std::string const & b);

std::string
normalize(std::string const & in);

std::string
expand(std::string const & s, const char c, std::string const & v);

std::string &
ltrim(std::string & s);

std::string &
rtrim(std::string & s);

std::string &
trim(std::string & s);

std::string
camelify(std::string const & s);

void
dumpCharArray(std::string const & s, std::ostream & o);

} // namespace String
} // namespace common
} // namespace ace

#endif  // ACE_COMMON_STRING_H_
