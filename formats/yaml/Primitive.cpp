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
#include <ace/tree/Primitive.h>
#include <sstream>
#include <string>

namespace ace { namespace yamlfmt { namespace Primitive {

tree::Value::Ref
build(std::string const& name, YAML::Node const& r)
{
  std::string value = r.as<std::string>();
  /*
   * If the node is an escaped string then it would have a tag starting with !.
   * In that case, return the string verbatim.
   */
  if (r.Tag().length() > 0 && r.Tag()[0] == '!') {
    return tree::Primitive::build(name, value);
  }
  /*
   * Process the content of the string.
   */
  if (common::String::is<long>(value)) {
    long v = common::String::value<long>(value);
    return tree::Primitive::build(name, v);
  } else if (common::String::is<double>(value)) {
    double v = common::String::value<double>(value);
    return tree::Primitive::build(name, v);
  } else if (common::String::is<bool>(value)) {
    bool v = common::String::value<bool>(value);
    return tree::Primitive::build(name, v);
  } else {
    return tree::Primitive::build(name, value);
  }
}

void
dump(tree::Value const& v, YAML::Emitter& e)
{
  tree::Primitive const& p = static_cast<tree::Primitive const&>(v);
  if (p.is<long>()) {
    e << p.value<long>();
  } else if (p.is<double>()) {
    e << p.value<double>();
  } else if (p.is<bool>()) {
    e << p.value<bool>();
  } else {
    auto const& v = p.value<std::string>();
    /*
     * If the string can be decoded as another primitive type, then quote it.
     */
    if (common::String::is<long>(v) || common::String::is<double>(v) ||
        common::String::is<bool>(v)) {
      e << YAML::DoubleQuoted;
    }
    /*
     * Emit the value.
     */
    e << v << YAML::Auto;
  }
}

}}}
