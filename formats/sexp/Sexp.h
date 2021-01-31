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

#pragma once

#include <list>
#include <memory>
#include <string>

namespace ace { namespace sexp {

/**
 * Value class
 */
class Value
{
public:
  using Ref = std::shared_ptr<Value>;

  enum class Type
  {
    Atom,
    List
  };

  Value() = delete;

  Type type() const;

protected:
  Value(const Type t);

  Type m_type;
};

/**
 * Atom class
 */

class Atom : public Value
{
public:
  enum class Type
  {
    Boolean,
    String,
    Integer,
    Float,
    Symbol,
  };

  Atom() = delete;
  Atom(const Type t);

  Type type() const;

private:
  Type m_type;
};

/**
 * Boolean class
 */
class Boolean : public Atom
{
public:
  Boolean();
  Boolean(std::string const& v);

  bool value() const;

private:
  bool m_value;
};

class String : public Atom
{
public:
  String();
  String(std::string const& v);

  std::string const& value() const;

private:
  std::string m_value;
};

class Integer : public Atom
{
public:
  enum class Base
  {
    Decimal,
    Octal,
    Hexadecimal
  };

  Integer();
  Integer(const Base b, std::string const& v);

  long value() const;

private:
  static long parse(const Base b, std::string const& v);

  long m_value;
};

class Float : public Atom
{
public:
  Float();
  Float(std::string const& v);

  double value() const;

private:
  double m_value;
};

class Symbol : public Atom
{
public:
  Symbol();
  Symbol(std::string const& v);

  std::string const& value() const;

private:
  std::string m_value;
};

/**
 * List class
 */
class List : public Value
{
public:
  List();
  List(Value::Ref const& v);

  size_t len() const;
  Value::Ref car() const;
  Value::Ref cdr() const;

  void append(Value::Ref const& v);

  static bool NIL(Value::Ref const& r);

private:
  Value::Ref m_car;
  Value::Ref m_cdr;
};

}}
