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

#include "Common.h"
#include <string>

namespace ace {
namespace pyfmt {

tree::Value::Ref
build_value(std::string const & n, PyObject * o) {
  if (PyBool_Check(o) or PyInt_Check(o)  or PyLong_Check(o) or
      PyFloat_Check(o) or PyString_Check(o)) {
    return Primitive::build(n, o);
  } else if (PyList_Check(o)) {
    return Array::build(n, o);
  } else if (PyDict_Check(o)) {
    return Object::build(n, o);
  }
  return tree::Value::Ref();
}

void
dump_value(tree::Value const & v, std::ostream & o, int l, bool i) {
  switch (v.type()) {
    case tree::Value::Type::Boolean   :
    case tree::Value::Type::Float     :
    case tree::Value::Type::Integer   :
    case tree::Value::Type::String    : return Primitive::dump(v, o, l, i);
    case tree::Value::Type::Array     : return Array::dump(v, o, l, i);
    case tree::Value::Type::Object    : return Object::dump(v, o, l, i);
    default                           : break;
  }
}

} // namespace inifmt
} // namespace ace
