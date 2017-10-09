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

#include <ace/formats/ini/Ini.h>
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <array>
#include <cstdio>
#include <sstream>

namespace {

std::string
exec(std::string const & cmd) {
  std::array<char, 128> buffer;
  std::string result;
  std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe) {
    ACE_LOG(Error, "Cannot execute: ", cmd);
    return "";
  }
  while (!feof(pipe.get())) {
    if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
      result += buffer.data();
    }
  }
  return ace::common::String::trim(result);
}

}

namespace ace {
namespace ini {

/**
 * Values
 */

Value::Value(const Type t, std::string const & v)
  : m_type(t), m_raw(v) { }

Value::Type
Value::type() const {
  return m_type;
}

Boolean::Boolean(std::string const & v)
  : Value(Type::Boolean, v), m_value(common::String::value<bool>(v)) { }

bool const &
Boolean::value() const {
  return m_value;
}

Boolean::operator std::string() const {
  return std::string("BLN(" + m_raw + ")");
}

Key::Key(std::string const & k)
  : Value(Type::Key, k), m_key(k) { }

std::string const &
Key::value() const {
  return m_key;
}

Key::operator std::string() const {
  return std::string("KEY(" + m_key + ")");
}

String::String(std::string const & v)
  : Value(Type::String, v), m_value(v) { }

std::string const &
String::value() const {
  return m_value;
}

String::operator std::string() const {
  return std::string("STR(" + m_raw + ")");
}

Integer::Integer(const Base b, std::string const & v)
  : Value(Type::Integer, v), m_value(0) {
  switch (b) {
    case Base::Decimal : {
      std::istringstream(v) >> m_value;
    } break;
    case Base::Octal : {
      std::istringstream(v) >> std::oct >> m_value;
    } break;
    case Base::Hexadecimal : {
      std::istringstream(v) >> std::hex >> m_value;
    } break;
    case Base::Unknown : {
    } break;
  }
}

long const &
Integer::value() const {
  return m_value;
}

Integer::operator std::string() const {
  return std::string("INT(" + m_raw + ")");
}

Float::Float(std::string const & v)
  : Value(Type::Float, v), m_value(common::String::value<double>(v)) {
}

double const &
Float::value() const {
  return m_value;
}

Float::operator std::string() const {
  return std::string("FLT(" + m_raw + ")");
}

Reference::Reference(std::string const & v)
  : Value(Type::Reference, v), m_path() {
  m_path = tree::Path::build(tree::Path::parse(v));
}

tree::Path::Ref
Reference::path() const {
  return m_path;
}

Reference::operator std::string() const {
  return std::string("REF(" + m_raw + ")");
}

Environment::Environment(std::string const & v)
  : Value(Type::Environment, v), m_key(v), m_value() {
  char * env = getenv(v.c_str());
  if (env != nullptr) {
    ACE_LOG(Debug, v, " -> ", env);
    m_value = std::string(env);
  }
}

std::string const &
Environment::value() const {
  return m_value;
}

Environment::operator std::string() const {
  return std::string("ENV(" + m_raw + ")");
}

System::System(std::string const & v)
  : Value(Type::System, v), m_result(::exec(v)) { }

std::string const &
System::result() const {
  return m_result;
}

System::operator std::string() const {
  return std::string("SYS(" + m_raw + ")");
}

/**
 * Statements
 */

Statement::Statement(const Type t)
  : m_type(t) { }

Statement::Type
Statement::type() const {
  return m_type;
}

Section::Section(std::string const & n, tree::Path::Ref const & r)
  : Statement(Type::Section), m_name(n), m_path(r) { }

std::string const &
Section::name() const {
  return m_name;
}

tree::Path::Ref const &
Section::path() const {
  return m_path;
}

Section::operator std::string() const {
  return std::string("SEC(" + m_name + ")");
}

ValueList::ValueList(Data const & v)
  : Statement(Type::ValueList), m_values(v) { }

ValueList::Data const &
ValueList::values() const {
  return m_values;
}

ValueList::operator std::string() const {
  std::ostringstream oss;
  oss << "VLS(";
  for (auto const & e: m_values) {
    oss << static_cast<std::string>(*e);
  }
  oss << ")";
  return oss.str();
}

} // namespace ini
} // namespace ace
