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

#include <ace/model/Hook.h>
#include <ace/model/Errors.h>
#include <ace/common/Log.h>
#include <ace/common/Regex.h>
#include <ace/common/String.h>
#include <string>
#include <vector>

namespace ace {
namespace model {

bool
Hook::validate(std::string const & s) {
  std::vector<std::string> elems;
  common::String::split(s, ':', elems);
  if (elems.size() != 3) {
    return false;
  }
  if (elems[0].length() == 0) {
    ACE_LOG(Error, "Hook path cannot be empty");
    return false;
  }
  if (elems[1].length() == 0) {
    ACE_LOG(Error, "Hook match regex cannot be empty");
    return false;
  }
  if (elems[2].length() == 0) {
    ACE_LOG(Error, "Hook replacement regex cannot be empty");
    return false;
  }
  try {
    tree::Path::parse(elems[0]);
  } catch (std::invalid_argument const &) {
    return false;
  }
  ACE_LOG(Debug, s);
  return true;
}

void
Hook::load(std::string const & s) {
  std::vector<std::string> elems;
  common::String::split(s, ':', elems);
  m_path = tree::Path::parse(elems[0]);
  m_pattern = elems[1];
  m_value = elems[2];
}

bool
Hook::match(std::string const & s) const {
  return common::Regex::match(s, m_pattern);
}

bool
Hook::transform(std::string const & v, std::string & r) const {
  return common::Regex::expand(v, m_pattern, m_value, r);
}

tree::Path const &
Hook::path() const {
  return m_path;
}

std::string const &
Hook::pattern() const {
  return m_pattern;
}

std::string const &
Hook::value() const {
  return m_value;
}

} // namespace model
} // namespace ace
