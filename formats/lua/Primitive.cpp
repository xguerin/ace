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
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <iostream>
#include <sstream>
#include <string>

namespace {

static bool escape(const char c) {
  return c == '\'';
}

} // namespace

namespace ace {
namespace luafmt {
namespace Primitive {

tree::Value::Ref
build(std::string const & n, lua_State * L) {
  switch (lua_type(L, -1)) {
    case LUA_TBOOLEAN: {
      bool v = lua_toboolean(L, -1);
      return tree::Primitive::build(n, v);
    }
    case LUA_TNUMBER: {
      std::string v(lua_tostring(L, -1));
      if (common::String::is<double>(v)) {
        double w = common::String::value<double>(v);
        return tree::Primitive::build(n, w);
      } else {
        long w = common::String::value<long>(v);
        return tree::Primitive::build(n, w);
      }
    }
    case LUA_TSTRING: {
      std::string v(lua_tostring(L, -1));
      return tree::Primitive::build(n, v);
    }
    default: break;
  }
  return tree::Value::Ref();
}

void
dump(tree::Value const & v, std::ostream & o, int l, bool i) {
  tree::Primitive const & p = static_cast<tree::Primitive const &>(v);
  if (not i) common::String::indent(o, l);
  if (p.is<std::string>()) {
    o << '\'';
    for (auto & c : p.value()) {
      if (escape(c)) o << '\\';
      o << c;
    }
    o << '\'';
  } else {
    o << p.value();
  }
}

} // namespace Primitive
} // namespace luafmt
} // namespace ace

