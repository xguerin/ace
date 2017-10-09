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

#include <ace/types/IPv4.h>
#include <ace/common/String.h>
#include <ace/filesystem/Directory.h>
#include <ace/filesystem/File.h>
#include <ace/filesystem/Utils.h>
#include <ace/tree/Checker.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <netdb.h>

#ifdef __OpenBSD__
#include <sys/socket.h>
#endif

namespace ace {
namespace model {

// IPv4 format checker

IPv4FormatChecker::IPv4FormatChecker(const BasicType * o) : FormatChecker(o) { }

bool
IPv4FormatChecker::operator() (tree::Object const & r, tree::Value const & v) const {
  if (not FormatChecker<std::string>::operator()(r, v)) return false;
  int score = 0;
  v.each([&](tree::Value const & w) {
    tree::Primitive const & p = static_cast<tree::Primitive const &>(w);
    if (p.value() != "auto" and not checkFormat(p.value())) {
      ERROR_O(m_owner, ERR_IPv4_BAD_ADDRESS(p.value()));
      score += 1;
    }
  });
  return score == 0;
}

bool IPv4FormatChecker::checkFormat(std::string const & s) {
  struct addrinfo hints, * result = nullptr;
  memset(& hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  int r = getaddrinfo(s.c_str(), nullptr, & hints, & result);
  freeaddrinfo(result);
  return r == 0;
}

// IPv4 class

IPv4::IPv4() : Type(BasicType::Kind::IPv4), EnumeratedType(BasicType::Kind::IPv4) { }

void
IPv4::collectInterfaceIncludes(std::set<std::string> & i) const {
  BasicType::collectInterfaceIncludes(i);
  i.insert("<string>");
}

void
IPv4::collectImplementationIncludes(std::set<std::string> & i) const {
  BasicType::collectInterfaceIncludes(i);
  i.insert("<string>");
}

BasicType::Ref
IPv4::clone(std::string const & n) const {
  IPv4 * mac = new IPv4(*this);
  mac->setName(n);
  return BasicType::Ref(mac);
}

} // namespace model
} // namespace ace
