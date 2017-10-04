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

#include <ace/common/String.h>
#include <iomanip>
#include <functional>
#include <limits>
#include <string>
#include <vector>

namespace ace {
namespace common {
namespace String {

std::ostream &
indent(std::ostream & o, int l) {
  for (int i = 0; i < l; i += 1) o << ' ';
  return o;
}

template<>
bool
is<long>(std::string const & s) {
  for (auto & c : s) if (not isdigit(c) and c != '-' and c != '+') return false;
  long r;
  std::istringstream iss(s);
  iss >> std::boolalpha >> r;
  return (iss.rdstate() & (std::istream::failbit | std::istream::badbit)) == 0;
}

template<>
bool
is<double>(std::string const & s) {
  for (auto & c : s) if (not isdigit(c)) switch (c) {
    case '.': case '-': case '+': case 'e': case 'E': break;
    default: return false;
  }
  if (s.find_first_of('.') == std::string::npos and
      s.find_first_of('e') == std::string::npos and
      s.find_first_of('E') == std::string::npos) {
    return false;
  }
  if (s.find_first_of('.') != std::string::npos and
      s.find_first_of('.') != s.find_last_of('.')) {
    return false;
  }
  if (s.find_first_of('e') != std::string::npos and
      s.find_first_of('e') != s.find_last_of('e')) {
    return false;
  }
  if (s.find_first_of('E') != std::string::npos and
      s.find_first_of('E') != s.find_last_of('E')) {
    return false;
  }
  if (s.find_first_of('e') != std::string::npos and
      s.find_first_of('E') != std::string::npos) {
    return false;
  }
  double r;
  std::istringstream iss(s);
  iss >> std::boolalpha >> r;
  return (iss.rdstate() & (std::istream::failbit | std::istream::badbit)) == 0;
}

template<>
double
value<double>(std::string const & s) {
  double r;
  std::istringstream iss(s);
  iss >> std::boolalpha >> r;
  return r;
}

template<>
std::string
value(std::string const & s) {
  return s;
}

std::string
prefix(std::string const & a, std::string const & b) {
  std::ostringstream oss;
  auto ita = a.begin();
  auto itb = b.begin();
  while (ita != a.end() and itb != b.end()) {
    if (*ita != *itb) break;
    oss << *ita;
    ita++;
    itb++;
  }
  return oss.str();
}

std::string
normalize(std::string const & in) {
  std::locale loc;
  std::ostringstream oss;
  if (std::isdigit(in[0])) oss << "_";
  for (auto & c : in) oss << (std::isalnum(c, loc) ? c : '_');
  return oss.str();
}

std::string
expand(std::string const & s, const char c, std::string const & v) {
  std::ostringstream oss;
  for (auto & e : s) if (e == c) {
    oss << v;
  } else {
    oss << e;
  }
  return oss.str();
}

std::string &
ltrim(std::string & s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

std::string &
rtrim(std::string & s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

std::string &
trim(std::string & s) {
  return ltrim(rtrim(s));
}

std::string
camelify(std::string const & s) {
  std::vector<std::string> parts;
  split(s, '_', parts);
  std::ostringstream oss;
  for (auto const & e : parts) if (not e.empty()) {
    std::string temp(e);
    if (std::isalpha(temp[0])) temp[0] = std::toupper(temp[0]);
    oss << temp;
  }
  return oss.str();
}

void
dumpCharArray(std::string const & s, std::ostream & o){
  o << std::setw(2) << std::setfill('0');
  size_t count = 0;
  for (auto const & e : s) {
    if (count % 13 == 0) o << "  ";
    o << "0x" << std::hex << static_cast<int>(e);
    count += 1;
    if (count < s.size()) o << ",";
    if (count % 13 == 0) o <<std::endl;
    else o << " ";
  }
  if (count % 13 != 0) o << std::endl;
}

} // namespace String
} // namespace common
} // namespace ace
