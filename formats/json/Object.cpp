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

#include <ace/formats/json/Object.h>
#include <ace/formats/json/Common.h>
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <ace/tree/Object.h>  // NOLINT
#include <string>

namespace ace {
namespace jsonfmt {
namespace Object {

tree::Value::Ref
build(std::string const & name, json_t * const obj) {
  tree::Object::Ref object = tree::Object::build(name);
  const char * key = nullptr;
  json_t * value = nullptr;
  json_object_foreach(obj, key, value) {
    std::string skey(key);
    tree::Value::Ref v = Common::build(skey, value);
    if (v == nullptr) {
      ACE_LOG(Error, "skipping unsupported value format for key: ", skey);
    } else {
      object->put(skey, v);
    }
  }
  return object;
}

json_t *
dump(tree::Value const & v) {
  tree::Object const & w = static_cast<tree::Object const &>(v);
  json_t * result = json_object();
  for (auto const & e : w) {
    json_t * value = Common::dump(*e.second);
    json_object_set_new(result, e.first.c_str(), value);
  }
  return result;
}

} // namespace Object
} // namespace jsonfmt
} // namespace ace
