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

#include "Value.h"
#include <ace/common/String.h>
#include <memory>
#include <sstream>
#include <string>

namespace ace {
namespace tree {

class Primitive : public Value {
 public:

  using Ref = std::shared_ptr<Primitive>;

 public:

  Primitive() = delete;

  template<typename T> static Ref build(T const & v);
  template<typename T> static Ref build(std::string const & n, T const & v);
  template<size_t N, typename T> static Ref build(const char (&n)[N], T const & v);

  template<typename T> bool is() const;
  template<typename T> T value() const;

  void stringify();
  std::string value() const;

 private:

  class Content {
   public:
    using Ref = std::unique_ptr<Content>;
    virtual ~Content() { }
    virtual std::string toString() const = 0;
  };

  template<typename T> class TypedContent : public Content {
   public:

    TypedContent() = delete;
    explicit TypedContent(T const & v) : m_value(v) { }

    T value() const {
      return m_value;
    }

    std::string toString() const {
      return common::String::from<T>(m_value);
    }

    template<typename U>
    bool is() const {
      return std::is_same<T, U>();
    }

   private:
    T m_value;
  };

  template<typename T> static Value::Type typeOf();

  template<typename T> Primitive(std::string const & n, T const & v);
  template<size_t N> Primitive(std::string const & n, const char (&v)[N]);

  Content::Ref  m_content;
};

template<typename T>
Primitive::Ref
Primitive::build(T const & v) {
  return build("", v);
}

template<typename T>
Primitive::Ref
Primitive::build(std::string const & n, T const & v) {
  return Ref(new Primitive(n, v));
}

template<size_t N, typename T>
Primitive::Ref
Primitive::build(const char (&n)[N], T const & v) {
  return Ref(new Primitive(std::string(n), v));
}

template<>
Primitive::Primitive(std::string const & n, bool const & v);

template<>
Primitive::Primitive(std::string const & n, char const & v);

template<>
Primitive::Primitive(std::string const & n, char const & v);

template<>
Primitive::Primitive(std::string const & n, short const & v);

template<>
Primitive::Primitive(std::string const & n, unsigned short const & v);

template<>
Primitive::Primitive(std::string const & n, int const & v);

template<>
Primitive::Primitive(std::string const & n, unsigned int const & v);

template<>
Primitive::Primitive(std::string const & n, long const & v);

template<>
Primitive::Primitive(std::string const & n, unsigned long const & v);

template<>
Primitive::Primitive(std::string const & n, float const & v);

template<>
Primitive::Primitive(std::string const & n, double const & v);

template<>
Primitive::Primitive(std::string const & n, std::string const & v);

template<>
Primitive::Primitive(std::string const & n, char * const & v);

template<>
Primitive::Primitive(std::string const & n, const char * const & v);

template<size_t N>
Primitive::Primitive(std::string const & n, const char (&v)[N])
  : Value(n, Type::String), m_content() {
  if (N > 0) {
    bool hasZero = false;
    for (size_t i = 0; i < N; i += 1) if (v[i] == '\0') {
      hasZero = true;
      break;
    }
    auto * c = new TypedContent<std::string>(hasZero ? std::string(v) : std::string(v, N));
    m_content = Content::Ref(c);
  }
}

template<>
bool
Primitive::is<bool>() const;

template<>
bool
Primitive::is<short>() const;

template<>
bool
Primitive::is<unsigned short>() const;

template<>
bool
Primitive::is<int>() const;

template<>
bool
Primitive::is<unsigned int>() const;

template<>
bool
Primitive::is<long>() const;

template<>
bool
Primitive::is<unsigned long>() const;

template<>
bool
Primitive::is<float>() const;

template<>
bool
Primitive::is<double>() const;

template<>
bool
Primitive::is<std::string>() const;

template<typename T>
T
Primitive::value() const {
  if (Primitive::typeOf<T>() == m_type) {
    return static_cast<TypedContent<T> const &>(*m_content).value();
  }
  throw std::logic_error("Bad type conversion");
}

template<>
double
Primitive::value() const;

template<typename T>
Value::Type
Primitive::typeOf() {
  return Value::Type::Undefined;
}

template<>
inline
Value::Type
Primitive::typeOf<bool>() {
  return Value::Type::Boolean;
}

template<>
inline
Value::Type
Primitive::typeOf<int>() {
  return Value::Type::Integer;
}

template<>
inline
Value::Type
Primitive::typeOf<long>() {
  return Value::Type::Integer;
}

template<>
inline
Value::Type
Primitive::typeOf<float>() {
  return Value::Type::Float;
}

template<>
inline
Value::Type
Primitive::typeOf<double>() {
  return Value::Type::Float;
}

template<>
inline
Value::Type
Primitive::typeOf<std::string>() {
  return Value::Type::String;
}

} // namespace tree
} // namespace ace
