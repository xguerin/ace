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

#include "Primitive.h"
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <ace/tree/Primitive.h>
#include <sstream>
#include <string>

namespace ace {
namespace sexpfmt {
namespace Primitive {

tree::Value::Ref
build(std::string const & name, sexp::Value::Ref const & obj) {
  return nullptr;
}

size_t
dump(tree::Value const & v, const tree::Scanner::Format f, const size_t indent,
     std::ostream & o) {
  std::string value;
  auto const & p = static_cast<tree::Primitive const &>(v);
  switch (p.type()) {
    case tree::Value::Type::Boolean : {
      value = common::String::from(p.value<bool>());
    } break;
    case tree::Value::Type::String : {
      value = '"' + p.value<std::string>() + '"';
    } break;
    case tree::Value::Type::Integer : {
      value = common::String::from(p.value<long>());
    } break;
    case tree::Value::Type::Float : {
      value =common::String::from(p.value<double>());
    } break;
    default : {
    } break;
  }
  o << value;
  return value.length();
}

} // namespace Primitive
} // namespace sexpfmt
} // namespace ace
