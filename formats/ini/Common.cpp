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
#include "Lexer.h"
#include "Array.h"
#include "Object.h"
#include "Primitive.h"
#include <ace/tree/Array.h>
#include <ace/tree/Object.h>
#include <ace/tree/Primitive.h>
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <iostream>
#include <string>

namespace {

static bool
isWhiteSpace(std::string const& str)
{
  if (str.empty()) {
    return true;
  }
  for (auto const& c : str) {
    if (c != ' ' && c != '\t' && c != '\f') {
      return false;
    }
  }
  return true;
}

ace::tree::Value::Ref
build(ace::tree::Object const& g, std::string const& n,
      ace::ini::Value::Ref const& r)
{
  switch (r->type()) {
    case ace::ini::Value::Type::Boolean: {
      auto const& v = static_cast<ace::ini::Boolean const&>(*r);
      return ace::tree::Primitive::build(n, v.value());
    }
    case ace::ini::Value::Type::Key: {
      auto const& v = static_cast<ace::ini::Key const&>(*r);
      return ace::tree::Primitive::build(n, v.value());
    }
    case ace::ini::Value::Type::String: {
      auto const& v = static_cast<ace::ini::String const&>(*r);
      return ace::tree::Primitive::build(n, v.value());
    }
    case ace::ini::Value::Type::Integer: {
      auto const& v = static_cast<ace::ini::Integer const&>(*r);
      return ace::tree::Primitive::build(n, v.value());
    }
    case ace::ini::Value::Type::Float: {
      auto const& v = static_cast<ace::ini::Float const&>(*r);
      return ace::tree::Primitive::build(n, v.value());
    }
    case ace::ini::Value::Type::Reference: {
      auto const& v = static_cast<ace::ini::Reference const&>(*r);
      std::vector<ace::tree::Value::Ref> lasso;
      g.get(*v.path(), lasso);
      if (lasso.size() == 0) {
        return ace::tree::Primitive::build(n, "");
      } else {
        return lasso.front();
      }
    }
    case ace::ini::Value::Type::Environment: {
      auto const& v = static_cast<ace::ini::Environment const&>(*r);
      return ace::tree::Primitive::build(n, v.value());
    }
    case ace::ini::Value::Type::System: {
      auto const& v = static_cast<ace::ini::System const&>(*r);
      return ace::tree::Primitive::build(n, v.result());
    }
  }
  return nullptr;
}

ace::tree::Object::Ref
processStream(std::istream& in)
{
  std::string line;
  std::vector<std::string> parts;
  ace::tree::Path::Ref dest;
  auto top = ace::tree::Object::build();
  auto cur = top;
  while (!in.eof()) {
    std::getline(in, line);
    ace::common::String::split(std::string(line), '=', parts);
    if (parts.size() == 1) {
      auto e = ace::inifmt::Scan().parse(parts[0]);
      /**
       * If e is invalid and line is whitespace, simply continue
       */
      if (e == nullptr) {
        if (isWhiteSpace(line)) {
          continue;
        }
        ACE_LOG(Error, "Parsing failed");
        return nullptr;
      }
      if (e->type() != ace::ini::Statement::Type::Section) {
        ACE_LOG(Error, "Parsing failed");
        return nullptr;
      }
      auto const& section = static_cast<ace::ini::Section const&>(*e);
      /**
       * Process section
       */
      if (cur != top) {
        top->put(*dest, cur);
      }
      cur = ace::tree::Object::build(section.name());
      dest = section.path();
    } else if (parts.size() >= 2) {
      /**
       * Process key
       */
      auto k = ace::inifmt::Scan().parse(parts[0]);
      if (k == nullptr) {
        ACE_LOG(Error, "Parsing failed");
        return nullptr;
      }
      if (k->type() != ace::ini::Statement::Type::ValueList) {
        ACE_LOG(Error, "Parsing failed");
        return nullptr;
      }
      auto const& vl = static_cast<ace::ini::ValueList const&>(*k);
      if (vl.values().size() != 1) {
        ACE_LOG(Error, "Parsing failed");
        return nullptr;
      }
      if (vl.values().front()->type() != ace::ini::Value::Type::Key) {
        ACE_LOG(Error, "Parsing failed");
        return nullptr;
      }
      auto const& key = static_cast<ace::ini::Key const&>(*vl.values().front());
      /**
       * Process value
       */
      auto rest = std::vector<std::string>(++parts.begin(), parts.end());
      std::string rem = ace::common::String::join(rest, '=');
      auto v = ace::inifmt::Scan().parse(rem);
      if (v == nullptr) {
        ACE_LOG(Error, "Parsing failed");
        return nullptr;
      }
      if (v->type() != ace::ini::Statement::Type::ValueList) {
        ACE_LOG(Error, "Parsing failed");
        return nullptr;
      }
      auto const& array = static_cast<ace::ini::ValueList const&>(*v);
      /**
       * Process the values
       */
      if (array.values().size() == 1) {
        cur->put(key.value(),
                 ::build(*top, key.value(), array.values().front()));
      } else {
        auto ary = ace::tree::Array::build(key.value());
        for (auto const& w : array.values()) {
          ary->push_back(::build(*top, key.value(), w));
        }
        cur->put(key.value(), ary);
      }
    }
  }
  /**
   * Push the last object
   */
  if (cur != top) {
    top->put(*dest, cur);
  }
  return top;
}

}

namespace ace { namespace inifmt { namespace Common {

tree::Value::Ref
parseFile(std::string const& path)
{
  std::ifstream ifs(path);
  if (ifs.fail()) {
    ACE_LOG(Error, "Cannot open file: ", path);
    return nullptr;
  }
  auto result = processStream(ifs);
  ifs.close();
  return result;
}

tree::Value::Ref
parseString(std::string const& str)
{
  std::istringstream iss(str);
  return processStream(iss);
}

void
dump(tree::Value const& v, std::ostream& o)
{
  switch (v.type()) {
    case tree::Value::Type::Boolean:
    case tree::Value::Type::String:
    case tree::Value::Type::Integer:
    case tree::Value::Type::Float: {
      inifmt::Primitive::dump(v, o);
    } break;
    case tree::Value::Type::Object: {
      inifmt::Object::dump(v, o);
    } break;
    case tree::Value::Type::Array: {
      inifmt::Array::dump(v, o);
    } break;
    default: {
    } break;
  }
}

}}}
