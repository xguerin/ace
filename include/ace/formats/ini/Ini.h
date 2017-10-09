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

#include <ace/tree/Path.h>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace ace {
namespace ini {

class Value {
 public:

  using Ref = std::shared_ptr<Value>;

  enum class Type {
    Boolean,
    Key,
    String,
    Integer,
    Float,
    Environment,
    Reference,
    System
  };

  Value() = default;
  Value(const Type t, std::string const & v);

  Type type() const;

  virtual operator std::string() const = 0;

 protected:

  Type        m_type;
  std::string m_raw;
};

class Boolean : public Value {
 public:

  Boolean() = default;
  Boolean(std::string const & v);

  bool const & value() const;

  operator std::string() const;

 private:

  bool m_value;
};

class Key : public Value {
 public:

  Key() = default;
  Key(std::string const & k);

  std::string const & value() const;

  operator std::string() const;

 private:

  std::string m_key;
};

class String : public Value {
 public:

  String() = default;
  String(std::string const & v);

  std::string const & value() const;

  operator std::string() const;

 private:

  std::string m_value;
};

class Integer : public Value {
 public:

  enum class Base {
    Decimal,
    Octal,
    Hexadecimal,
    Unknown
  };

  Integer() = default;
  Integer(const Base b, std::string const & v);

  long const & value() const;

  operator std::string() const;

 private:

  long m_value;
};

class Float : public Value {
 public:

  Float() = default;
  Float(std::string const & v);

  double const & value() const;

  operator std::string() const;

 private:

  double m_value;
};

class Reference : public Value {
 public:

  Reference() = default;
  Reference(std::string const & v);

  tree::Path::Ref path() const;

  operator std::string() const;

 private:

  tree::Path::Ref m_path;
};

class Environment : public Value {
 public:

  Environment() = default;
  Environment(std::string const & v);

  std::string const & value() const;

  operator std::string() const;

 private:

  std::string m_key;
  std::string m_value;
};

class System : public Value {
 public:

  System() = default;
  System(std::string const & v);

  std::string const & result() const;

  operator std::string() const;

 private:

  std::string m_result;
};

class Statement {
 public:

  using Ref = std::shared_ptr<Statement>;

  enum class Type {
    Section,
    ValueList,
    Unknown
  };

  Statement() = default;
  Statement(const Type t);

  Type type() const;

  virtual operator std::string() const = 0;

 private:

  Type m_type;
};

class Section : public Statement {
 public:

  Section() = default;
  Section(std::string const & n, tree::Path::Ref const & r);

  std::string const & name() const;
  tree::Path::Ref const & path() const;

  operator std::string() const;

 private:

  std::string     m_name;
  tree::Path::Ref m_path;
};

class ValueList : public Statement {
 public:

  using Data = std::list<Value::Ref>;

  ValueList() = default;
  ValueList(Data const & v);

  Data const & values() const;

  operator std::string() const;

 private:

  Data m_values;
};

} // namespace ini
} // namespace ace
