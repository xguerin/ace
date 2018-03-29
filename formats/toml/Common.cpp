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
#include "Array.h"
#include "Object.h"
#include "Primitive.h"
#include <ace/common/Log.h>
#include <iostream>
#include <string>

namespace ace {
namespace tomlfmt {
namespace Common {

tree::Value::Ref
build(std::string const & name, toml::Value const & value) {
  tree::Value::Ref result = nullptr;
  if (value.is<toml::Table>()) {
    result = Object::build(name, value);
  } else if (value.is<toml::Array>()) {
    result = Array::build(name, value);
  } else if (value.is<std::string>()) {
    result = Primitive::build(name, value);
  } else if (value.is<int64_t>()) {
    result = Primitive::build(name, value);
  } else if (value.is<double>()) {
    result = Primitive::build(name, value);
  } else if (value.is<bool>()) {
    result = Primitive::build(name, value);
  }
  return result;
}

tree::Value::Ref
parseFile(std::string const & path) {
  std::ifstream ifs(path);
  if (ifs.fail()) {
    ACE_LOG(Error, "Cannot open file: ", path);
    return nullptr;
  }
  toml::ParseResult pr = toml::parse(ifs);
  return build("", pr.value);
}

tree::Value::Ref
parseString(std::string const & str) {
  std::istringstream iss(str);
  tree::Value::Ref result = nullptr;
  toml::ParseResult pr = toml::parse(iss);
  result = build("", pr.value);
  return result;
}

toml::Value
dump(tree::Value const & v) {
  switch (v.type()) {
    case tree::Value::Type::Boolean   :
    case tree::Value::Type::Float     :
    case tree::Value::Type::Integer   :
    case tree::Value::Type::String    : return Primitive::dump(v);
    case tree::Value::Type::Array     : return Array::dump(v);
    case tree::Value::Type::Object    : return Object::dump(v);
    default                           : return toml::Value();
  }
}

} // namespace Common
} // namespace inifmt
} // namespace ace
