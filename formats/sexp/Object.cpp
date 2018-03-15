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
namespace sexpfmt {
namespace Object {

bool
is(sexp::Value::Ref const & v) {
  bool result = false;
  auto e = v;
  /**
   * If the reference is invalid, return false
   */
  if (v == nullptr) {
    return false;
  }
  if (sexp::List::NIL(v)) {
    return false;
  }
  /**
   * Now check all the entries
   */
  while (true) {
    /**
     * If the element is not a list, then return false
     */
    if (e->type() == sexp::Value::Type::Atom) {
      return false;
    }
    /**
     * E is a list, is it NIL ?
     */
    if (sexp::List::NIL(e)) {
      break;
    }
    /**
     * E is a list check if the first element is a list
     */
    auto lst = static_cast<sexp::List const &>(*e);
    auto car = lst.car();
    if (car->type() != sexp::Value::Type::List) {
      return false;
    }
    /**
     * CAR is a list check if the first element is an atom
     */
    auto pair = static_cast<sexp::List const &>(*car);
    auto pcar = pair.car();
    if (pcar->type() != sexp::Value::Type::Atom) {
      return false;
    }
    /**
     * PCAR is an atom check if it is a symbol
     */
    auto const & atm = static_cast<sexp::Atom const &>(*pcar);
    if (atm.type() != sexp::Atom::Type::Symbol) {
      return false;
    }
    /**
     * Go to the next element
     */
    e = lst.cdr();
  }
  /**
   * All is clear, this is an assoc list
   */
  return true;
}

tree::Value::Ref
build(std::string const & name, sexp::Value::Ref const & v) {
  auto e = v;
  tree::Object::Ref object = tree::Object::build(name);
  /**
   * Check all entries in the assoc list
   */
  while (true) {
    /**
     * If the element is not a list, then return false
     */
    if (e->type() == sexp::Value::Type::Atom) {
      return nullptr;
    }
    /**
     * E is a list, is it NIL ?
     */
    if (sexp::List::NIL(e)) {
      break;
    }
    /**
     * E is a list check if the first element is a list
     */
    auto lst = static_cast<sexp::List const &>(*e);
    auto car = lst.car();
    if (car->type() != sexp::Value::Type::List) {
      return nullptr;
    }
    /**
     * CAR is a list check if the first element is an atom
     */
    auto pair = static_cast<sexp::List const &>(*car);
    auto pcar = pair.car();
    if (pcar->type() != sexp::Value::Type::Atom) {
      return nullptr;
    }
    /**
     * PCAR is an atom check if it is a symbol
     */
    auto const & atm = static_cast<sexp::Atom const &>(*pcar);
    if (atm.type() != sexp::Atom::Type::Symbol) {
      return nullptr;
    }
    /**
     * Put the new entry in the object
     */
    auto const & sym = static_cast<sexp::Symbol const &>(atm);
    object->put(sym.value(), Common::build(sym.value(), pair.cdr()));
    /**
     * Go to the next element
     */
    e = lst.cdr();
  }
  /**
   * All is clear, return the result
   */
  return object;
}

size_t
dump(tree::Value const & v, const tree::Scanner::Format f, const size_t indent,
     std::ostream & o) {
  tree::Object const & w = static_cast<tree::Object const &>(v);
  size_t nindent = indent + 1;
  /**
   * Compute the largest key
   */
  size_t max = 0;
  for (auto const & e: w) {
    if (e.first.length() > max) {
      max = e.first.length();
    }
  }
  /**
   * Dump the object
   */
  o << "(";
  auto it = w.begin();
  while(it != w.end()) {
    o << "(";
    if (f == tree::Scanner::Format::Default) {
      o << std::setw(max) << std::left;
    }
    o << it->first << " ";
    Common::dump(*it->second, f, nindent + max + 2, o);
    o << ")";
    ++it;
    if (it != w.end()) {
      if (f == tree::Scanner::Format::Compact) {
        o << " ";
      } else {
        o << std::endl;
        common::String::indent(o, nindent);
      }
    }
  }
  o << ")";
  return nindent;
}

} // namespace Object
} // namespace sexpfmt
} // namespace ace
