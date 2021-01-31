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

#include "Array.h"
#include "Common.h"
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <ace/tree/Array.h>
#include <string>

namespace ace { namespace yamlfmt { namespace Array {

tree::Value::Ref
build(std::string const& name, YAML::Node const& n)
{
  size_t index = 0;
  tree::Array::Ref res = tree::Array::build(name);
  for (auto const& i : n) {
    auto v = Common::build(std::to_string(index), i);
    res->push_back(v);
    index += 1;
  }
  return res;
}

void
dump(tree::Value const& v, YAML::Emitter& e)
{
  tree::Array const& ary = static_cast<tree::Array const&>(v);
  e << YAML::BeginSeq;
  for (size_t i = 0; i < ary.size(); i += 1) {
    Common::dump(*ary.at(i), e);
  }
  e << YAML::EndSeq;
}

}}}
