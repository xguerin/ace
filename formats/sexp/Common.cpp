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
#include <fstream>
#include <streambuf>
#include <string>

namespace ace {
namespace sexpfmt {
namespace Common {

tree::Value::Ref
parseFile(std::string const & path) {
  std::ifstream t(path);
  std::string str;
  /**
   * Pre-allocate the string
   */
  t.seekg(0, std::ios::end);
  str.reserve(t.tellg());
  t.seekg(0, std::ios::beg);
  /**
   * Load the file
   */
  str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  /**
   * Parse the string
   */
  return parseString(str);
}

tree::Value::Ref
parseString(std::string const & str) {
  auto res = Scan().parse(str, false);
  if (res == nullptr) return nullptr;
  return build("", res);
}

tree::Value::Ref
build(std::string const & name, sexp::Value::Ref const & v) {
  switch (v->type()) {
    case sexp::Value::Type::Atom: {
      auto const & atom = static_cast<sexp::Atom const &>(*v);
      switch (atom.type()) {
        case sexp::Atom::Type::Boolean: {
          auto const & w = static_cast<sexp::Boolean const &>(atom);
          return tree::Primitive::build(name, w.value());
        }
        case sexp::Atom::Type::String: {
          auto const & w = static_cast<sexp::String const &>(atom);
          return tree::Primitive::build(name, w.value());
        }
        case sexp::Atom::Type::Integer: {
          auto const & w = static_cast<sexp::Integer const &>(atom);
          return tree::Primitive::build(name, w.value());
        }
        case sexp::Atom::Type::Float: {
          auto const & w = static_cast<sexp::Float const &>(atom);
          return tree::Primitive::build(name, w.value());
        }
        case sexp::Atom::Type::Symbol: {
          auto const & w = static_cast<sexp::Symbol const &>(atom);
          return tree::Primitive::build(name, w.value());
        }
      }
    } break;
    case sexp::Value::Type::List: {
      if (Object::is(v)) {
        return Object::build(name, v);
      } else {
        return Array::build(name, v);
      }
    } break;
  }
  return nullptr;
}

size_t
dump(tree::Value const & v, const tree::Scanner::Format f, const size_t indent,
     std::ostream & o) {
  switch (v.type()) {
    case tree::Value::Type::Boolean : {
      return sexpfmt::Primitive::dump(v, f, indent, o);
    } break;
    case tree::Value::Type::String : {
      return sexpfmt::Primitive::dump(v, f, indent, o);
    } break;
    case tree::Value::Type::Integer : {
      return sexpfmt::Primitive::dump(v, f, indent, o);
    } break;
    case tree::Value::Type::Float : {
      return sexpfmt::Primitive::dump(v, f, indent, o);
    } break;
    case tree::Value::Type::Object : {
      return sexpfmt::Object::dump(v, f, indent, o);
    } break;
    case tree::Value::Type::Array : {
      return sexpfmt::Array::dump(v, f, indent, o);
    } break;
    default : {
      return 0;
    } break;
  }
}

} // namespace Common
} // namespace sexpfmt
} // namespace ace

