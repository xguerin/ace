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
#include <ace/common/String.h>
#include <ace/tree/Primitive.h> // NOLINT
#include <sstream>
#include <string>

namespace ace {
namespace hjsonfmt {
namespace Primitive {

tree::Value::Ref
build(std::string const & name, Hjson::Value const& pri) {
  tree::Primitive::Ref result = nullptr;
  switch (pri.type()) {
    case Hjson::Type::String: {
      std::string value(pri.to_string());
      result = tree::Primitive::build(name, value);
    } break;
    case Hjson::Type::Int64: {
      long value = pri.to_int64();
      result = tree::Primitive::build(name, value);
    } break;
    case Hjson::Type::Double: {
      double value = pri.to_double();
      result = tree::Primitive::build(name, value);
    } break;
    case Hjson::Type::Bool: {
      result = tree::Primitive::build(name, static_cast<bool>(pri));
    } break;
    default: {
    } break;
  }
  return result;
}

Hjson::Value
dump(tree::Value const & v) {
  tree::Primitive const & p = static_cast<tree::Primitive const &>(v);
  if (p.is<bool>()) {
    return Hjson::Value(p.value<bool>());
  }
  if (p.is<long>()) {
    return Hjson::Value(p.value<long>());
  }
  if (p.is<double>()) {
    return Hjson::Value(p.value<double>());
  }
  if (p.is<std::string>()) {
    return Hjson::Value(p.value<std::string>());
  }
  return nullptr;
}

} // namespace Primitive
} // namespace hjsonfmt
} // namespace ace
