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

#include <ace/model/Author.h>
#include <ace/tree/Checker.h>
#include <ace/tree/Primitive.h>
#include <string>

namespace ace {
namespace model {

Author::Author() : m_name(), m_email() { }

tree::Path
Author::path(const bool local) const {
  return m_parent->path(local);
}

bool Author::checkModel(tree::Value const & t) const {
  const ace::tree::Checker::Schema jsonSchema = {
    { "name"  , { ace::tree::Value::Type::String, false } },
    { "email" , { ace::tree::Value::Type::String, false } }
  };
  ace::tree::Checker chk(path(), t);
  return chk.validate(jsonSchema);
}

void Author::loadModel(tree::Value const & t) {
  m_name = static_cast<tree::Primitive const &>(t["name"]).value<std::string>();
  m_email = static_cast<tree::Primitive const &>(t["email"]).value<std::string>();
}

std::string const & Author::name() const {
  return m_name;
}

std::string const & Author::email() const {
  return m_email;
}

} // namespace model
} // namespace ace

namespace std {

ostream & operator<<(ostream & o, ace::model::Author const & a) {
  o << a.name() << " (" << a.email() << ")";
  return o;
}

} // namespace std
