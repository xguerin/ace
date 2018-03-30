/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without reexaction, including without limitation the rights
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

#include <ace/model/Hook.h>
#include <ace/model/Errors.h>
#include <ace/common/Log.h>
#include <ace/common/Regex.h>
#include <ace/common/String.h>
#include <ace/tree/Checker.h>
#include <ace/tree/Primitive.h>
#include <string>
#include <vector>

namespace ace {
namespace model {

static tree::Checker::Schema schema = {
  { "path"  , tree::Checker::Pattern(tree::Value::Type::String, false) },
  { "from"  , tree::Checker::Pattern(tree::Value::Type::String, false) },
  { "to"    , tree::Checker::Pattern(tree::Value::Type::String, false) },
  { "exact", tree::Checker::Pattern(tree::Value::Type::Boolean, true ) }
};

bool
Hook::validate(tree::Object const & r) {
  tree::Checker checker("", r);
  if (!checker.validate(schema)) {
    ACE_LOG(Debug, "Hook schema verification failed");
    return false;
  }
  auto const & p = r.get("path");
  auto const & v = static_cast<tree::Primitive const &>(p);
  try {
    tree::Path::parse(v.value<std::string>());
  } catch (std::invalid_argument const &) {
    return false;
  }
  return true;
}

void
Hook::load(tree::Object const & r) {
  auto const & p = static_cast<tree::Primitive const &>(r.get("path"));
  auto const & f = static_cast<tree::Primitive const &>(r.get("from"));
  auto const & t = static_cast<tree::Primitive const &>(r.get("to"));
  m_path = tree::Path::parse(p.value<std::string>());
  m_pattern = f.value<std::string>();
  m_value = t.value<std::string>();
  /*
   * Check if the exact mode is defined.
   */
  if (r.has("exact")) {
    auto const & s = static_cast<tree::Primitive const &>(r.get("exact"));
    m_exact = s.value<bool>();
  }
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

bool
Hook::exact() const {
  return m_exact;
}

} // namespace model
} // namespace ace
