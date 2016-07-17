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

#include <ace/formats/lua/Scanner.h>
#include <ace/formats/lua/Common.h>
#include <ace/formats/lua/Object.h>
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <ace/engine/Master.h>
#include <iostream>
#include <string>
#include <vector>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

namespace {

static void report_errors(lua_State * L) {
  const char * str = lua_tostring(L, -1);
  std::cerr << "-- " << str << std::endl;
  lua_pop(L, 1);
}

static void build_args(lua_State * L, int argc, char ** argv) {
  lua_newtable(L);
  for (int i = 0; i < argc; i += 1) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i);
  }
  lua_setglobal(L, "arg");
}

} // namespace

namespace ace {
namespace luafmt {

tree::Value::Ref
Scanner::open(std::string const & fn, int argc, char ** argv) {
  if (argc == 0 or argv == nullptr) {
    ACE_LOG(Error, "invalid ARGC/ARGV arguments");
    return nullptr;
  }
  tree::Value::Ref obj;
  lua_State * L = lua_open();
  luaL_openlibs(L);
  shift(fn, argc, argv);
  build_args(L, argc, argv);
  if (luaL_loadfile(L, fn.c_str()) != 0) {
    report_errors(L);
    goto error;
  }
  if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0) {
    report_errors(L);
    goto error;
  }
  lua_getglobal(L, "config");
  if (!lua_istable(L, -1)) {
    ACE_LOG(Error, "no \"config\" dictionary found in \"", fn, "\"");
    goto error;
  }

  obj = Object::build("", L);
  lua_pop(L, 1);

 error:
  return obj;
}

tree::Value::Ref
Scanner::parse(std::string const & s, int argc, char ** argv) {
  if (argc == 0 or argv == nullptr) {
    ACE_LOG(Error, "invalid ARGC/ARGV arguments");
    return nullptr;
  }
  tree::Value::Ref obj;
  lua_State * L = lua_open();
  luaL_openlibs(L);
  shift("", argc, argv);
  build_args(L, argc, argv);
  if (luaL_loadstring(L, s.c_str()) != 0) {
    report_errors(L);
    goto error;
  }
  if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0) {
    report_errors(L);
    goto error;
  }
  lua_getglobal(L, "config");
  if (!lua_istable(L, -1)) {
    ACE_LOG(Error, "no \"config\" dictionary found in inline LUA");
    goto error;
  }

  obj = Object::build("", L);
  lua_pop(L, 1);

 error:
  return obj;
}

void
Scanner::dump(tree::Value const & v, const Format f, std::ostream & o) const {
  o << "config = ";
  dump_value(v, o, 0, true);
  o << std::endl;
}

std::string
Scanner::name() const {
  return "lua";
}

std::string
Scanner::extension() const {
  return "lua";
}

} // namespace luafmt
} // namespace ace

extern "C" {
void * loadPlugin() {
  return new ace::luafmt::Scanner();
}
}
