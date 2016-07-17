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

#include <ace/formats/lua/Common.h>
#include <string>

namespace ace {
namespace luafmt {

static
tree::Value::Ref
build_array_or_object(std::string const & n, lua_State * L) {
  lua_pushnil(L);
  if (lua_next(L, -2) == 0) return Object::build(n, L);
  int t = lua_type(L, -2);
  lua_pop(L, 1);
  lua_pop(L, 1);
  return t == LUA_TSTRING ? Object::build(n, L) : Array::build(n, L);
}

void
stack_dump(lua_State *L) {
  int i;
  int top = lua_gettop(L);
  for (i = 1; i <= top; i++) {  /* repeat for each level */
    int t = lua_type(L, i);
    switch (t) {
      case LUA_TSTRING:  /* strings */
        std::cout << lua_tostring(L, i);
        break;
      case LUA_TBOOLEAN:  /* booleans */
        std::cout << (lua_toboolean(L, i) ? "true" : "false");
        break;
      case LUA_TNUMBER:  /* numbers */
        std::cout << lua_tonumber(L, i);
        break;
      case LUA_TNIL:  /* numbers */
        std::cout << "nil";
        break;
      default:  /* other values */
        std::cout << lua_typename(L, t);
        break;
    }
    std::cout << "   ";
  }
  std::cout << std::endl;
}

tree::Value::Ref
build_value(std::string const & n, lua_State * L) {
  switch (lua_type(L, -1)) {
    case LUA_TBOOLEAN:
    case LUA_TNUMBER:
    case LUA_TSTRING:
      return Primitive::build(n, L);
    case LUA_TTABLE:
      return build_array_or_object(n, L);
    default:
      break;
  }
  return tree::Value::Ref();
}

void
dump_value(tree::Value const & v, std::ostream & o, int l, bool i) {
  switch (v.type()) {
    case tree::Value::Type::Boolean   :
    case tree::Value::Type::Float     :
    case tree::Value::Type::Integer   :
    case tree::Value::Type::String    : return Primitive::dump(v, o, l, i);
    case tree::Value::Type::Array     : return Array::dump(v, o, l, i);
    case tree::Value::Type::Object    : return Object::dump(v, o, l, i);
    default                           : break;
  }
}

} // namespace luafmt
} // namespace ace

