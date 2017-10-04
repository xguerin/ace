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

#include <ace/formats/json/Common.h>
#include <ace/formats/json/Array.h>
#include <ace/formats/json/Object.h>
#include <ace/formats/json/Primitive.h>
#include <ace/common/Log.h>
#include <string>

namespace ace {
namespace jsonfmt {
namespace Common {

tree::Value::Ref
build(std::string const & name, json_t * const value) {
  tree::Value::Ref result = nullptr;
  switch (json_typeof(value)) {
    case JSON_OBJECT: {
      result = Object::build(name, value);
    } break;
    case JSON_ARRAY: {
      result = Array::build(name, value);
    } break;
    case JSON_STRING:
    case JSON_INTEGER:
    case JSON_REAL:
    case JSON_TRUE:
    case JSON_FALSE: {
      result = Primitive::build(name, value);
    } break;
    default: {
    } break;
  }
  return result;
}

tree::Value::Ref
parseFile(std::string const & path) {
  json_error_t error;
  json_t * item = json_load_file(path.c_str(), JSON_REJECT_DUPLICATES, & error);
  if (item == nullptr) {
    ACE_LOG(Error, "load failed: ", error.text);
    return nullptr;
  }
  if (not json_is_object(item)) {
    ACE_LOG(Error, "root item is not an object");
    return nullptr;
  }
  auto result = build("", item);
  json_decref(item);
  return result;
}

tree::Value::Ref
parseString(std::string const & str) {
  json_error_t error;
  json_t * item = json_loads(str.c_str(), JSON_REJECT_DUPLICATES, & error);
  if (item == nullptr) {
    ACE_LOG(Error, "load failed: ", error.text);
    return nullptr;
  }
  if (not json_is_object(item)) {
    ACE_LOG(Error, "root item is not an object");
    return nullptr;
  }
  auto result = build("", item);
  json_decref(item);
  return result;
}

json_t *
dump(tree::Value const & v) {
  switch (v.type()) {
    case tree::Value::Type::Boolean   :
    case tree::Value::Type::Float     :
    case tree::Value::Type::Integer   :
    case tree::Value::Type::String    : return Primitive::dump(v);
    case tree::Value::Type::Array     : return Array::dump(v);
    case tree::Value::Type::Object    : return Object::dump(v);
    default                           : return nullptr;
  }
}

} // namespace Common
} // namespace inifmt
} // namespace ace

