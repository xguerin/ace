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

#include <ace/formats/json/Primitive.h>
#include <ace/common/String.h>
#include <ace/tree/Primitive.h> // NOLINT
#include <sstream>
#include <string>

namespace ace {
namespace jsonfmt {
namespace Primitive {

tree::Value::Ref
build(std::string const & name, json_t * const pri) {
  tree::Primitive::Ref result = nullptr;
  switch (json_typeof(pri)) {
    case JSON_STRING: {
      std::string value(json_string_value(pri));
      result = tree::Primitive::build(name, value);
    } break;
    case JSON_INTEGER: {
      long value = json_integer_value(pri);
      result = tree::Primitive::build(name, value);
    } break;
    case JSON_REAL: {
      double value = json_real_value(pri);
      result = tree::Primitive::build(name, value);
    } break;
    case JSON_TRUE: {
      result = tree::Primitive::build(name, true);
    } break;
    case JSON_FALSE: {
      result = tree::Primitive::build(name, false);
    } break;
    default: {
    } break;
  }
  return result;
}

json_t *
dump(tree::Value const & v) {
  tree::Primitive const & p = static_cast<tree::Primitive const &>(v);
  if (p.is<bool>()) {
    return json_boolean(p.value<bool>());
  }
  if (p.is<long>()) {
    return json_integer(p.value<long>());
  }
  if (p.is<double>()) {
    return json_real(p.value<double>());
  }
  if (p.is<std::string>()) {
    return json_string(p.value<std::string>().c_str());
  }
  return nullptr;
}

} // namespace Primitive
} // namespace jsonfmt
} // namespace ace
