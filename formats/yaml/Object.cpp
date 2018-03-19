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
#include <iomanip>

namespace ace {
namespace yamlfmt {
namespace Object {

tree::Value::Ref
build(std::string const & name, YAML::Node const & p) {
  tree::Object::Ref res = tree::Object::build(name);
  for (auto const & e: p) {
    std::string key =  e.first.as<std::string>();
    auto v = Common::build(key, e.second);
    res->put(key, v);
  }
  return res;
}

void
dump(tree::Value const & v, YAML::Emitter & e) {
  tree::Object const & w = static_cast<tree::Object const &>(v);
  e << YAML::BeginMap;
  for (auto const & i : w) {
    e << YAML::Key << i.first;
    e << YAML::Value;
    Common::dump(*i.second, e);
  }
  e << YAML::EndMap;
}

} // namespace Object
} // namespace yamlfmt
} // namespace ace
