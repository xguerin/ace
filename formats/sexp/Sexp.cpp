/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtasexpng a copy
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

#include "Sexp.h"
#include <ace/common/String.h>
#include <iostream>
#include <sstream>

namespace ace {
namespace sexp {

/**
 * Value class
 */

Value::Value(const Type t)
  : m_type(t) { }

Value::Type
Value::type() const {
  return m_type;
}

/**
 * Atom class
 */

Atom::Atom(const Type t)
  : Value(Value::Type::Atom), m_type(t) { }

Atom::Type
Atom::type() const {
  return m_type;
}

/**
 * Boolean
 */

Boolean::Boolean()
  : Atom(Type::Boolean), m_value(false) { }

Boolean::Boolean(std::string const & v)
  : Atom(Type::Boolean), m_value(common::String::value<bool>(v)) { }

bool
Boolean::value() const {
  return m_value;
}

/**
 * String
 */

String::String()
  : Atom(Type::String), m_value() { }

String::String(std::string const & v)
  : Atom(Type::String), m_value(v) { }

std::string const &
String::value() const {
  return m_value;
}

/**
 * Integer
 */

long
Integer::parse(const Base b, std::string const & v) {
  long value;
  switch (b) {
    case Base::Decimal : {
      std::istringstream(v) >> value;
    } break;
    case Base::Octal : {
      std::istringstream(v) >> std::oct >> value;
    } break;
    case Base::Hexadecimal : {
      std::istringstream(v) >> std::hex >> value;
    } break;
  }
  return value;
}

Integer::Integer()
  : Atom(Type::Integer), m_value(0) { }

Integer::Integer(const Base b, std::string const & v)
  : Atom(Type::Integer), m_value(parse(b, v)) { }

long
Integer::value() const {
  return m_value;
}

/**
 * Float
 */

Float::Float()
  : Atom(Type::Float), m_value(0.0) { }

Float::Float(std::string const & v)
  : Atom(Type::Float), m_value(common::String::value<double>(v)) { }

double
Float::value() const {
  return m_value;
}

/**
 * Symbol
 */

Symbol::Symbol()
  : Atom(Type::Symbol), m_value() { }

Symbol::Symbol(std::string const & v)
  : Atom(Type::Symbol), m_value(v) { }

std::string const &
Symbol::value() const {
  return m_value;
}

/**
 * List class
 */
Value::Ref List::NIL(new List);

List::List()
  : Value(Type::List), m_car(nullptr), m_cdr(nullptr) { }

List::List(sexp::Value::Ref const & v)
  : Value(Type::List), m_car(v), m_cdr(NIL) { }

Value::Ref List::car() const {
  return m_car;
}

Value::Ref List::cdr() const {
  return m_cdr;
}

void
List::append(Value::Ref const & v) {
  if (m_cdr == NIL || m_cdr->type() == Value::Type::Atom) {
    m_cdr = v;
  } else {
    static_cast<List &>(*m_cdr).append(v);
  }
}

} // namespace sexp
} // namespace ace
