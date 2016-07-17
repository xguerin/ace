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

#include <ace/types/String.h>
#include <ace/model/Model.h>
#include <set>
#include <string>

namespace ace {
namespace model {

String::String()
  : Type(BasicType::Kind::String)
  , EnumeratedType(BasicType::Kind::String)
  , m_length() {
  m_attributes.define<LengthAttributeType>("length", true);
}

bool
String::checkModel(tree::Value const & t) const {
  if (not Type::checkModel(t)) return false;
  auto const & o = static_cast<tree::Object const &>(t);
  if (o.has("length")) {
    auto const & s = static_cast<tree::Primitive const &>(o["length"]);
    common::Range<long> r;
    if (not common::Range<long>::parse(s.value<std::string>(), r)) {
      ERROR(ERR_RANGE_FORMAT(s.value<std::string>()));
      return false;
    }
  }
  return true;
}

void
String::loadModel(tree::Value const & t) {
  Type::loadModel(t);
  if (m_attributes.has("length")) {
    auto const & a = static_cast<LengthAttributeType const &>(*m_attributes["length"]);
    common::Range<long>::parse(a.head(), m_length);
  }
}

bool
String::checkInstance(tree::Object const & r, tree::Value const & v) const {
  if (not Type::checkInstance(r, v)) return false;
  size_t score = 0;
  v.each([&](tree::Value const & w) {
    auto const & s = static_cast<tree::Primitive const &>(w);
    if (not m_length.contains(s.value<std::string>().length())) {
      ERROR(ERR_STR_LEN_OUTSIDE_OF_CONSTRAINT);
      score += 1;
    }
  });
  return score == 0;
}

void
String::collectInterfaceIncludes(std::set<std::string> & i) const {
  BasicType::collectInterfaceIncludes(i);
  i.insert("<string>");
}

void
String::collectImplementationIncludes(std::set<std::string> & i) const {
  BasicType::collectInterfaceIncludes(i);
  i.insert("<string>");
}

BasicType::Ref
String::clone(std::string const & n) const {
  String * str = new String(*this);
  str->setName(n);
  return BasicType::Ref(str);
}

} // namespace model
} // namespace ace
