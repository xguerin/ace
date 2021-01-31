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
#include <ace/tree/Array.h> // NOLINT
#include <string>

namespace ace { namespace sexpfmt { namespace Array {

tree::Value::Ref
build(std::string const& name, sexp::Value::Ref const& v)
{
  auto e = v;
  size_t index = 0;
  tree::Array::Ref array = tree::Array::build(name);
  /**
   * Check all entries in the assoc list
   */
  while (!sexp::List::NIL(e)) {
    switch (e->type()) {
      case sexp::Value::Type::Atom: {
        array->push_back(Common::build(std::to_string(index), e));
        e = sexp::Value::Ref(new sexp::List);
        index += 1;
      } break;
      case sexp::Value::Type::List: {
        auto const& list = static_cast<sexp::List const&>(*e);
        array->push_back(Common::build(std::to_string(index), list.car()));
        index += 1;
        e = list.cdr();
      } break;
    }
  }
  /**
   * All is clear, return the result
   */
  return array;
}

size_t
dump(tree::Value const& v, const tree::Scanner::Format f, const size_t indent,
     std::ostream& o)
{
  auto const& a = static_cast<tree::Array const&>(v);
  size_t nindent = indent + 1;
  o << "(";
  for (size_t i = 0; i < a.size(); i += 1) {
    size_t offset = Common::dump(*a.at(i), f, nindent, o);
    if (i < a.size() - 1) {
      if (a.at(i)->type() == tree::Value::Type::Object) {
        if (f == tree::Scanner::Format::Default) {
          o << std::endl;
          common::String::indent(o, nindent);
        }
      } else {
        o << " ";
        nindent += offset + 1;
      }
    }
  }
  o << ")";
  return nindent;
}

}}}
