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
#include <ace/common/String.h>
#include <ace/tree/Primitive.h>
#include <sstream>
#include <string>

namespace ace { namespace tomlfmt { namespace Primitive {

tree::Value::Ref
build(std::string const& name, toml::Value const& pri)
{
  tree::Primitive::Ref result = nullptr;
  if (pri.is<std::string>()) {
    std::string value(pri.as<std::string>());
    result = tree::Primitive::build(name, value);
  } else if (pri.is<int64_t>()) {
    long value = pri.as<int64_t>();
    result = tree::Primitive::build(name, value);
  } else if (pri.is<double>()) {
    double value = pri.as<double>();
    result = tree::Primitive::build(name, value);
  } else if (pri.is<bool>()) {
    bool value = pri.as<bool>();
    result = tree::Primitive::build(name, value);
  }
  return result;
}

toml::Value
dump(tree::Value const& v)
{
  tree::Primitive const& p = static_cast<tree::Primitive const&>(v);
  if (p.is<bool>()) {
    return toml::Value(p.value<bool>());
  }
  if (p.is<long>()) {
    return toml::Value(static_cast<int64_t>(p.value<long>()));
  }
  if (p.is<double>()) {
    return toml::Value(p.value<double>());
  }
  if (p.is<std::string>()) {
    return toml::Value(p.value<std::string>());
  }
  return toml::Value();
}

}}}
