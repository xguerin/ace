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

#include <ace/formats/json/Array.h>
#include <ace/formats/json/Common.h>
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <ace/tree/Array.h> // NOLINT
#include <string>

namespace ace {
namespace jsonfmt {
namespace Array {

tree::Value::Ref
build(std::string const & name, json_t * const ary) {
  tree::Array::Ref array = tree::Array::build(name);
  size_t index = 0;
  json_t * value = nullptr;
  json_array_foreach(ary, index, value) {
    tree::Value::Ref v = Common::build(std::to_string(index), value);
    if (v == nullptr) {
      ACE_LOG(Error, "skipping unsupported value format at index: ", index);
    } else {
      array->push_back(v);
    }
  }
  return array;
}

json_t *
dump(tree::Value const & v) {
  tree::Array const & ary = static_cast<tree::Array const &>(v);
  json_t * result = json_array();
  for (auto const & e : ary) {
    json_t * value = Common::dump(*e);
    json_array_append_new(result, value);
  }
  return result;
}

} // namespace Array
} // namespace jsonfmt
} // namespace ace
