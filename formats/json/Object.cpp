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

void
dump(tree::Value const & v, const tree::Scanner::Format f, std::ostream & o, int l, bool i) {
  tree::Object const & w = static_cast<tree::Object const &>(v);
  if (not i and f == tree::Scanner::Format::Default) {
    common::String::indent(o, l);
  }
  o << '{';
  if (f == tree::Scanner::Format::Default) {
    o << std::endl;
  } else {
    o << ' ';
  }
  size_t count = 0;
  for (auto & e : w) {
    if (f == tree::Scanner::Format::Default) {
      common::String::indent(o, l + 2);
    }
    if (f == tree::Scanner::Format::Inlined) {
      o << "\\\"" << e.first << "\\\": ";
    } else {
      o << '"' << e.first << "\": ";
    }
    Common::dump(*e.second, f, o, l + 2, true);
    if (count < w.size() - 1) o << ",";
    count += 1;
    if (f == tree::Scanner::Format::Default) {
      o << std::endl;
    } else {
      o << ' ';
    }
  }
  if (f == tree::Scanner::Format::Default) {
    common::String::indent(o, l);
  }
  o << '}';
}

} // namespace Object
} // namespace jsonfmt
} // namespace ace
