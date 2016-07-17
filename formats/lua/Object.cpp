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

#include <ace/formats/lua/Object.h>
#include <ace/formats/lua/Common.h>
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <string>

namespace ace {
namespace luafmt {
namespace Object {

tree::Value::Ref
build(std::string const & n, lua_State * L) {
  tree::Object::Ref object = tree::Object::build(n);
  tree::Value::Ref v(nullptr);
  lua_pushnil(L);
  while (lua_next(L, -2) != 0) {
    std::string k(lua_tostring(L, -2));
    v = build_value(k, L);
    if (v.get() != nullptr) {
      object->put(k, v);
    } else {
      ACE_LOG(Error, "skipping unsupported value format for key: ", k);
    }
    lua_pop(L, 1);
  }
  return tree::Value::Ref(object);
}

void
dump(tree::Value const & v, std::ostream & o, int l, bool i) {
  tree::Object const & w = static_cast<tree::Object const &>(v);
  if (not i) common::String::indent(o, l);
  o << '{' << std::endl;
  size_t count = 0;
  for (auto & e : w) {
    common::String::indent(o, l + 2) << e.first << " = ";
    dump_value(*e.second, o, l + 2, true);
    if (count < w.size() - 1) o << ", ";
    o << std::endl;
  }
  common::String::indent(o, l) << '}';
}

} // namespace Object
} // namespace luafmt
} // namespace ace
