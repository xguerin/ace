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

#include <ace/types/URI.h>
#include <ace/types/IPv4.h>
#include <ace/common/String.h>
#include <ace/filesystem/Directory.h>
#include <ace/filesystem/File.h>
#include <ace/filesystem/Utils.h>
#include <ace/tree/Checker.h>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace {

static bool
checkFileFormat(std::string const& n, std::string const& b)
{
  ace::fs::Path nodePath(b);
  if (nodePath.isDirectory()) {
    ACE_LOG(Error, "uri \"", n, "\": ", ERR_FILE_URI_PATH_NOT_A_FILE(b));
    return false;
  }
  ace::fs::File file(nodePath);
  if (file.isValid()) {
    return true;
  }
  ACE_LOG(Error, "uri \"", n, "\": ", ERR_FILE_URI_FILE_NOT_FOUND(b));
  return false;
}

static bool
checkIPv4Format(std::string const& n, std::string const& b)
{
  std::vector<std::string> addrParts;
  ace::common::String::split(b, ':', addrParts);
  if (addrParts.empty() or addrParts.size() > 2) {
    ACE_LOG(Error, "uri \"", n, "\": ", ERR_IPv4_URI_BAD_ADDRESS(b));
    return false;
  }
  for (auto& e : addrParts) {
    if (e.empty()) {
      ACE_LOG(Error, "uri \"", n, "\": ", ERR_IPv4_URI_BAD_ADDRESS(b));
      return false;
    }
  }
  if (not ace::model::IPv4FormatChecker::checkFormat(addrParts[0])) {
    ACE_LOG(Error, "uri \"", n, "\": ", ERR_IPv4_URI_BAD_ADDRESS(b));
    return false;
  }
  if (addrParts.size() == 2) {
    std::string const& port = addrParts[1];
    long p = -1;
    if ((std::istringstream(port) >> p).fail()) {
      ACE_LOG(Error, "uri \"", n, "\": ", ERR_IPv4_URI_INVALID_PORT(b));
      return false;
    }
    if (p < 0 or p > 65535) {
      ACE_LOG(Error, "uri \"", n, "\": ", ERR_IPv4_URI_INVALID_PORT(b));
      return false;
    }
  }
  return true;
}

static bool
checkFormat(std::string const& n, std::string const& b)
{
  size_t markerPos = b.find("://");
  if (markerPos == std::string::npos or markerPos == 0) {
    ACE_LOG(Error, "uri \"", n, "\": ", ERR_VALUE_NOT_AN_URI(b));
    return false;
  }
  std::string scheme = b.substr(0, markerPos);
  if (ace::model::URI::parseScheme(scheme) ==
      ace::model::URI::Scheme::Undefined) {
    ACE_LOG(Error, "uri \"", n, "\": ", ERR_UNSUPPORTED_SCHEME(scheme));
    return false;
  }
  std::string value = b.substr(markerPos + 3);
  switch (ace::model::URI::parseScheme(scheme)) {
    case ace::model::URI::Scheme::File: {
      if (not checkFileFormat(n, value)) {
        return false;
      }
    } break;
    case ace::model::URI::Scheme::IPv4: {
      if (not checkIPv4Format(n, value)) {
        return false;
      }
    } break;
    default:
      break;
  }
  return true;
}

}

namespace ace { namespace model {

// URI comparator

bool
URISchemaCompare::operator()(std::string const& a, std::string const& b) const
{
  return a == "auto" or a.compare(0, b.length(), b) == 0;
}

// URI format checker

URIFormatChecker::URIFormatChecker(const BasicType* o) : FormatChecker(o) {}

bool
URIFormatChecker::operator()(tree::Object const& r, tree::Value const& v) const
{
  if (not FormatChecker<std::string>::operator()(r, v)) {
    return false;
  }
  int score = 0;
  v.each([&](tree::Value const& w) {
    tree::Primitive const& p = static_cast<tree::Primitive const&>(w);
    if (p.value() != "auto" and not checkFormat(v.name(), p.value())) {
      score += 1;
    }
  });
  return score == 0;
}

// URI class

URI::URI() : Type(BasicType::Kind::URI), EnumeratedType(BasicType::Kind::URI) {}

bool
URI::validateModel()
{
  if (not EnumeratedType::validateModel()) {
    return false;
  }
  int score = 0;
  if (hasDefaultAttribute()) {
    for (auto& e : defaultAttribute().values()) {
      if (parseScheme(e) == Scheme::Undefined) {
        ERROR(ERR_INVALID_DEFAULT_SCHEME(e));
        score += 1;
      }
    }
  }
  if (hasEitherAttribute()) {
    for (auto& e : eitherAttribute().values()) {
      if (parseScheme(e) == Scheme::Undefined) {
        ERROR(ERR_INVALID_EITHER_SCHEME(e));
        score += 1;
      }
    }
  }
  return score == 0;
}

void
URI::collectInterfaceIncludes(std::set<std::string>& i) const
{
  BasicType::collectInterfaceIncludes(i);
  i.insert("<string>");
}

void
URI::collectImplementationIncludes(std::set<std::string>& i) const
{
  BasicType::collectInterfaceIncludes(i);
  i.insert("<string>");
}

BasicType::Ref
URI::clone(std::string const& n) const
{
  URI* uri = new URI(*this);
  uri->setName(n);
  return BasicType::Ref(uri);
}

URI::Scheme
URI::parseScheme(std::string const& n)
{
  const std::map<std::string, ace::model::URI::Scheme> stringToScheme = {
    { "file", Scheme::File },
    { "http", Scheme::HTTP },
    { "ipv4", Scheme::IPv4 },
    { "ftp", Scheme::FTP }
  };
  if (stringToScheme.count(n) != 0) {
    return stringToScheme.at(n);
  }
  return Scheme::Undefined;
}

std::string
URI::toString(const Scheme s)
{
  const std::map<ace::model::URI::Scheme, std::string> schemeToString = {
    { Scheme::File, "file" },
    { Scheme::HTTP, "http" },
    { Scheme::IPv4, "ipv4" },
    { Scheme::FTP, "ftp" }
  };
  if (schemeToString.count(s) != 0) {
    return schemeToString.at(s);
  }
  return "undefined";
}

}}
