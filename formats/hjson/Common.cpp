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
#include <hjson/hjson.h>
#include "Array.h"
#include "Object.h"
#include "Primitive.h"
#include <ace/common/Log.h>
#include <string>

namespace ace { namespace hjsonfmt { namespace Common {

tree::Value::Ref
build(std::string const& name, Hjson::Value const& value)
{
  tree::Value::Ref result = nullptr;
  switch (value.type()) {
    case Hjson::Type::Map: {
      // result = Object::build(name, value);
    } break;
    case Hjson::Type::Vector: {
      result = Array::build(name, value);
    } break;
    case Hjson::Type::Bool:
    case Hjson::Type::Double:
    case Hjson::Type::Int64:
    case Hjson::Type::String: {
      result = Primitive::build(name, value);
    } break;
    default: {
    } break;
  }
  return result;
}

tree::Value::Ref
parseFile(std::string const& path)
{
  Hjson::Value value = Hjson::UnmarshalFromFile(path);
  if (value.type() != Hjson::Type::Map) {
    ACE_LOG(Error, "root item is not an object");
    return nullptr;
  }
  return build("", value);
}

tree::Value::Ref
parseString(std::string const& str)
{
  Hjson::Value value = Hjson::Unmarshal(str);
  if (value.type() != Hjson::Type::Map) {
    ACE_LOG(Error, "root item is not an object");
    return nullptr;
  }
  return build("", value);
}

Hjson::Value
dump(tree::Value const& v)
{
  switch (v.type()) {
    case tree::Value::Type::Boolean:
    case tree::Value::Type::Float:
    case tree::Value::Type::Integer:
    case tree::Value::Type::String:
      return Primitive::dump(v);
    case tree::Value::Type::Array:
      return Array::dump(v);
    case tree::Value::Type::Object:
      return Object::dump(v);
    default:
      return Hjson::Value();
  }
}

}}}
