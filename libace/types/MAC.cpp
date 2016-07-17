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

#include <ace/types/MAC.h>
#include <ace/common/String.h>
#include <ace/filesystem/Directory.h>
#include <ace/filesystem/File.h>
#include <ace/filesystem/Utils.h>
#include <ace/tree/Checker.h>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace {

static bool checkFormat(std::string const & n, std::string const & b) {
  std::vector<std::string> addrParts;
  ace::common::String::split(b, ':', addrParts);
  if (addrParts.size() != 6) {
    ACE_LOG(Error, "mac \"", n, "\": ", ERR_MAC_BAD_ADDRESS(b));
    return false;
  }
  for (auto & e : addrParts) {
    if (e.size() != 2) {
      ACE_LOG(Error, "mac \"", n, "\": ", ERR_MAC_BAD_ADDRESS(b));
      return false;
    }
    char * inval = nullptr;
    long m = strtol(e.c_str(), & inval, 16);
    if (*inval != '\0' or errno == ERANGE or errno == EINVAL) {
      ACE_LOG(Error, "mac \"", n, "\": ", ERR_MAC_BAD_ADDRESS(b));
      return false;
    }
    if (m < 0 or m > 255) {
      ACE_LOG(Error, "mac \"", n, "\": ", ERR_MAC_BAD_ADDRESS(b));
      return false;
    }
  }
  return true;
}

} // namespace

namespace ace {
namespace model {

// MAC format checker

MACFormatChecker::MACFormatChecker(const BasicType * o) : FormatChecker(o) { }

bool
MACFormatChecker::operator() (tree::Object const & r, tree::Value const & v) const {
  if (not FormatChecker<std::string>::operator()(r, v)) return false;
  int score = 0;
  v.each([&](tree::Value const & w) {
    tree::Primitive const & p = static_cast<tree::Primitive const &>(w);
    if (p.value() != "auto" and not checkFormat(v.name(), p.value())) score += 1;
  });
  return score == 0;
}

// MAC class

MAC::MAC() : Type(BasicType::Kind::MAC), EnumeratedType(BasicType::Kind::MAC) { }

void
MAC::collectInterfaceIncludes(std::set<std::string> & i) const {
  BasicType::collectInterfaceIncludes(i);
  i.insert("<string>");
}

void
MAC::collectImplementationIncludes(std::set<std::string> & i) const {
  BasicType::collectInterfaceIncludes(i);
  i.insert("<string>");
}

BasicType::Ref
MAC::clone(std::string const & n) const {
  MAC * mac = new MAC(*this);
  mac->setName(n);
  return BasicType::Ref(mac);
}

} // namespace model
} // namespace ace
