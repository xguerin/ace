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
#include <ace/tree/Array.h>
#include <ace/tree/Object.h>
#include <ace/tree/Primitive.h>
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <cstdio>
#include <string>

namespace ace { namespace yamlfmt { namespace Common {

tree::Value::Ref
parseFile(std::string const& path)
{
  auto node = YAML::LoadFile(path);
  return build("", node);
}

bool
parseFile(std::string const& path, std::list<tree::Value::Ref>& r)
{
  auto nodes = YAML::LoadAllFromFile(path);
  for (auto& node : nodes) {
    auto res = build("", node);
    if (res == nullptr) {
      return false;
    }
    r.push_back(res);
  }
  return true;
}

tree::Value::Ref
parseString(std::string const& str)
{
  auto node = YAML::Load(str);
  return build("", node);
}

bool
parseString(std::string const& str, std::list<tree::Value::Ref>& r)
{
  auto nodes = YAML::LoadAll(str);
  for (auto& node : nodes) {
    auto res = build("", node);
    if (res == nullptr) {
      return false;
    }
    r.push_back(res);
  }
  return true;
}

tree::Value::Ref
build(std::string const& name, YAML::Node const& n)
{
  switch (n.Type()) {
    case YAML::NodeType::Map: {
      return Object::build(name, n);
    }
    case YAML::NodeType::Sequence: {
      return Array::build(name, n);
    }
    case YAML::NodeType::Scalar: {
      return Primitive::build(name, n);
    }
    case YAML::NodeType::Null: {
      ACE_LOG(Warning, "Null type not supported");
      break;
    }
    case YAML::NodeType::Undefined: {
      ACE_LOG(Warning, "Undefined YAML type");
      break;
    }
  }
  return nullptr;
}

void
dump(tree::Value const& v, YAML::Emitter& e)
{
  switch (v.type()) {
    case tree::Value::Type::Array: {
      return Array::dump(v, e);
    }
    case tree::Value::Type::Object: {
      return Object::dump(v, e);
    }
    case tree::Value::Type::Boolean:
    case tree::Value::Type::Integer:
    case tree::Value::Type::Float:
    case tree::Value::Type::String: {
      return Primitive::dump(v, e);
    }
    default: {
    }
  }
}

}}}
