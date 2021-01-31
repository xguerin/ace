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

#include "Array.h"
#include "Common.h"
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <string>

namespace ace { namespace pyfmt { namespace Array {

tree::Value::Ref
build(std::string const& n, PyObject* o)
{
  tree::Array::Ref array = tree::Array::build(n);
  tree::Value::Ref v(nullptr);
  for (Py_ssize_t i = 0; i < PyList_Size(o); i += 1) {
    v = build_value("", PyList_GetItem(o, i));
    if (v.get() != nullptr) {
      array->push_back(v);
    } else {
      ACE_LOG(Error, "skipping unsupported value format at index: ", i);
    }
  }
  return tree::Value::Ref(array);
}

void
dump(tree::Value const& v, std::ostream& o, int l, bool i)
{
  tree::Array const& a = static_cast<tree::Array const&>(v);
  bool inl = false;
  if (not i) {
    common::String::indent(o, l);
  }
  o << '[' << std::endl;
  size_t count = 0;
  for (auto& w : a) {
    dump_value(*w, o, l + 2, inl);
    inl = true;
    if (count < a.size() - 1) {
      o << ", ";
    }
    count += 1;
  }
  o << std::endl;
  common::String::indent(o, l + 2) << ']';
}

}}}
