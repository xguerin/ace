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

#include "Object.h"
#include "Common.h"
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <ace/tree/Object.h>
#include <string>

namespace ace { namespace tomlfmt { namespace Object {

tree::Value::Ref
build(std::string const& name, toml::Value const& obj)
{
  tree::Object::Ref object = tree::Object::build(name);
  const toml::Table& tb = obj.as<toml::Table>();
  for (auto const& e : tb) {
    std::string skey(e.first);
    tree::Value::Ref v = Common::build(skey, e.second);
    if (v == nullptr) {
      ACE_LOG(Error, "skipping unsupported value format for key: ", skey);
    } else {
      object->put(skey, v);
    }
  }
  return object;
}

toml::Value
dump(tree::Value const& v)
{
  tree::Object const& w = static_cast<tree::Object const&>(v);
  toml::Table table;
  for (auto const& e : w) {
    table[e.first] = Common::dump(*e.second);
  }
  return table;
}

}}}
