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

#include <ace/model/Attribute.h>
#include <set>
#include <string>

namespace ace {
namespace model {

Attribute::Attribute(std::string const & n, bool opt, bool ovr)
    : m_optional(opt), m_override(ovr) {
  setName(n);
}

tree::Path
Attribute::path(const bool local) const {
  return m_parent->path(local);
}

bool Attribute::merge(Attribute const & b) {
  if (m_optional != b.m_optional) return false;
  if (m_override != b.m_override) return false;
  return true;
}

bool Attribute::override(Attribute const & b) {
  if (not m_override) return false;
  return true;
}

void Attribute::print(std::ostream & o, int l) const {
  o << operator std::string() << std::endl;
}

bool Attribute::optional() const {
  return m_optional;
}

bool Attribute::overridable() const {
  return m_override;
}

} // namespace model
} // namespace ace

