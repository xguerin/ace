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

#include <ace/tree/Primitive.h>
#include <string>

namespace ace {
namespace tree {

template<>
Primitive::Primitive(std::string const & n, bool const & v)
  : Value(n, Type::Boolean), m_content(new TypedContent<bool>(v)) {
}

template<>
Primitive::Primitive(std::string const & n, char const & v)
  : Value(n, Type::String), m_content(new TypedContent<std::string>(std::string(1, v))) {
}

template<>
Primitive::Primitive(std::string const & n, unsigned char const & v)
  : Value(n, Type::String), m_content(new TypedContent<long>(v)) {
}

template<>
Primitive::Primitive(std::string const & n, short const & v)
  : Value(n, Type::Integer), m_content(new TypedContent<long>(v)) {
}

template<>
Primitive::Primitive(std::string const & n, unsigned short const & v)
  : Value(n, Type::Integer), m_content(new TypedContent<long>(v)) {
}

template<>
Primitive::Primitive(std::string const & n, int const & v)
  : Value(n, Type::Integer), m_content(new TypedContent<long>(v)) {
}

template<>
Primitive::Primitive(std::string const & n, unsigned int const & v)
  : Value(n, Type::Integer), m_content(new TypedContent<long>(v)) {
}

template<>
Primitive::Primitive(std::string const & n, long const & v)
  : Value(n, Type::Integer), m_content(new TypedContent<long>(v)) {
}

template<>
Primitive::Primitive(std::string const & n, unsigned long const & v)
  : Value(n, Type::Integer), m_content(new TypedContent<long>(v)) {
}

template<>
Primitive::Primitive(std::string const & n, float const & v)
  : Value(n, Type::Float), m_content(new TypedContent<double>(v)) {
}

template<>
Primitive::Primitive(std::string const & n, double const & v)
  : Value(n, Type::Float), m_content(new TypedContent<double>(v)) {
}

template<>
Primitive::Primitive(std::string const & n, char * const & v)
  : Value(n, Type::String), m_content(new TypedContent<std::string>(v)) {
}

template<>
Primitive::Primitive(std::string const & n, const char * const & v)
  : Value(n, Type::String), m_content(new TypedContent<std::string>(v)) {
}

template<>
Primitive::Primitive(std::string const & n, std::string const & v)
  : Value(n, Type::String), m_content(new TypedContent<std::string>(v)) {
}

template<>
bool
Primitive::is<bool>() const {
  return m_type == Value::Type::Boolean;
}

template<>
bool
Primitive::is<short>() const {
  return m_type == Value::Type::Integer;
}

template<>
bool
Primitive::is<unsigned short>() const {
  return m_type == Value::Type::Integer;
}

template<>
bool
Primitive::is<int>() const {
  return m_type == Value::Type::Integer;
}

template<>
bool
Primitive::is<unsigned int>() const {
  return m_type == Value::Type::Integer;
}

template<>
bool
Primitive::is<long>() const {
  return m_type == Value::Type::Integer;
}

template<>
bool
Primitive::is<unsigned long>() const {
  return m_type == Value::Type::Integer;
}

template<>
bool
Primitive::is<float>() const {
  return is<int>() or m_type == Value::Type::Float;
}

template<>
bool
Primitive::is<double>() const {
  return is<long>() or is<unsigned long>() or m_type == Value::Type::Float;
}

template<>
bool
Primitive::is<std::string>() const {
  return m_type == Value::Type::String;
}

void
Primitive::stringify() {
  m_type = Value::Type::String;
}

std::string
Primitive::value() const {
  return m_content->toString();
}

template<>
double
Primitive::value() const {
  if (m_type == Value::Type::Float) {
    return static_cast<TypedContent<double> const &>(*m_content).value();
  } else if (m_type == Value::Type::Integer) {
    return static_cast<TypedContent<long> const &>(*m_content).value();
  } else {
    return 0.0;
  }
}

} // namespace tree
} // namespace ace

