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

#include <ace/common/Regex.h>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <locale>
#include <re2/re2.h>

#define ESCAPE_CHAR '\\'

namespace {

static bool
expansion_count(std::string const& p, int& r)
{
  bool esc = false;
  std::locale loc;
  std::set<size_t> occs;
  for (auto& c : p) {
    if (c == ESCAPE_CHAR) {
      esc = not esc;
    } else if (esc) {
      if (std::isdigit(c, loc)) {
        occs.insert(c - '0');
      }
      esc = false;
    }
  }
  if (esc) {
    return false;
  }
  if (not occs.empty() and occs.size() != *occs.rbegin()) {
    return false;
  }
  r = occs.size();
  return true;
}

static std::string
expand_string(std::string const& p, std::vector<std::string> const& x)
{
  bool esc = false;
  std::locale loc;
  std::ostringstream oss;
  for (auto& c : p) {
    if (c == ESCAPE_CHAR) {
      if (esc) {
        oss << c;
      }
      esc = not esc;
    } else if (esc) {
      if (std::isdigit(c, loc)) {
        oss << x[c - '1'];
      }
      esc = false;
    } else {
      oss << c;
    }
  }
  return oss.str();
}

}

namespace ace { namespace common { namespace Regex {

bool
match(std::string const& s, std::string const& r)
{
  return RE2::FullMatch(s, r);
}

bool
expand(std::string const& s, std::string const& r, std::string const& p,
       std::string& v)
{
  int n = 0;
  if (not expansion_count(p, n)) {
    return false;
  }
  const RE2::Arg* args[n];
  std::vector<std::string> exps(n);
  for (int i = 0; i < n; i += 1) {
    args[i] = new RE2::Arg(&exps[i]);
  }
  if (not RE2::FullMatchN(s, r, args, n)) {
    return false;
  }
  for (int i = 0; i < n; i += 1) {
    delete args[i];
  }
  v = expand_string(p, exps);
  return true;
}

}}}
