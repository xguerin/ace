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

#include <ace/model/Arity.h>
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

namespace ace { namespace model {

Arity::Arity() : m_kind(Kind::Undefined), m_min(0), m_max(0) {}

Arity::Arity(const Kind kind, const size_t min, const size_t max)
  : m_kind(kind), m_min(min), m_max(max)
{}

bool
Arity::parse(std::string const& s, Arity& arity)
{
  if (s == "0") {
    arity = Arity(Kind::Disabled, 0, 0);
    return true;
  }
  if (s == "?") {
    arity = Arity(Kind::UpToOne, 0, 1);
    return true;
  }
  if (s == "1") {
    arity = Arity(Kind::One, 1, 1);
    return true;
  }
  if (s == "+") {
    arity = Arity(Kind::AtLeastOne, 1, static_cast<size_t>(-1));
    return true;
  }
  if (s == "*") {
    arity = Arity(Kind::Any, 0, static_cast<size_t>(-1));
    return true;
  }
  size_t min = 0, max = static_cast<size_t>(-1);
  std::vector<std::string> parts;
  common::String::split(s, ':', parts);
  if (parts.size() != 2) {
    return false;
  }
  if (not parts[0].empty()) {
    ssize_t tmp = 0;
    std::istringstream(parts[0]) >> tmp;
    if (tmp < 0) {
      return false;
    }
    min = tmp;
  }
  if (not parts[1].empty()) {
    ssize_t tmp = 0;
    std::istringstream(parts[1]) >> tmp;
    if (tmp < 0) {
      return false;
    }
    max = tmp;
  }
  if (min > max) {
    return false;
  }
  switch (min) {
    case 0:
      switch (max) {
        case 0:
          arity = Arity(Kind::Disabled, min, max);
          break;
        case 1:
          arity = Arity(Kind::UpToOne, min, max);
          break;
        default:
          arity = Arity(Kind::Any, min, max);
          break;
      }
      break;
    case 1:
      switch (max) {
        case 1:
          arity = Arity(Kind::One, min, max);
          break;
        default:
          arity = Arity(Kind::AtLeastOne, min, max);
          break;
      }
      break;
    default:
      arity = Arity(Kind::AtLeastOne, min, max);
      break;
  }
  return true;
}

bool
Arity::check(const size_t value) const
{
  return m_min <= value and value <= m_max;
}

Arity::Kind
Arity::kind() const
{
  return m_kind;
}

Arity
Arity::intersect(Arity const& o) const
{
  ACE_LOG(Debug, *this);
  ACE_LOG(Debug, o);
  switch (m_kind) {
    case Kind::Disabled:
    case Kind::Undefined: {
      return Arity();
    }
    case Kind::UpToOne:
      switch (o.m_kind) {
        case Kind::Undefined:
        case Kind::UpToOne:
          return o;
        case Kind::Any:
          return *this;
        default:
          return Arity();
      }
    case Kind::One:
      switch (o.m_kind) {
        case Kind::Undefined:
        case Kind::One:
          return o;
        case Kind::UpToOne:
          return Arity(Kind::UpToOne, 1, 1);
        case Kind::AtLeastOne:
          return Arity(Kind::AtLeastOne, 1, 1);
        default:
          return Arity();
      }
    case Kind::AtLeastOne:
      switch (o.m_kind) {
        case Kind::Undefined:
        case Kind::One:
          return o;
        case Kind::AtLeastOne:
          return Arity(Kind::AtLeastOne, m_min > o.m_min ? m_min : o.m_min,
                       m_max < o.m_max ? m_max : o.m_max);
        default:
          return Arity();
      }
    case Kind::Any:
      switch (o.m_kind) {
        case Kind::Undefined:
        case Kind::UpToOne:
          return Arity(Kind::Any, 0, 1);
        case Kind::Any:
          return Arity(Kind::Any, m_min, m_max < o.m_max ? m_max : o.m_max);
        default:
          return Arity();
      }
  }
  return Arity();
}

bool
Arity::promote()
{
  switch (m_kind) {
    case Kind::UpToOne:
      m_kind = Kind::One;
      m_min = 1;
      m_max = 1;
      return true;
    case Kind::Any:
      m_kind = Kind::AtLeastOne;
      m_min = 1;
      return true;
    default:
      return false;
  }
  return false;
}

void
Arity::disable()
{
  m_kind = Kind::Disabled;
  m_min = 0;
  m_max = 0;
}

bool
Arity::isValid() const
{
  return m_kind != Kind::Undefined;
}

std::string
Arity::marker() const
{
  switch (m_kind) {
    case Kind::Undefined:
      return "X";
    case Kind::Disabled:
      return "0";
    case Kind::UpToOne:
      return "?";
    case Kind::One:
      return "1";
    case Kind::AtLeastOne:
      return "+";
    case Kind::Any:
      return "*";
  }
  return "";
}

std::string
Arity::toString() const
{
  switch (m_kind) {
    case Kind::Undefined:
      return "";
    case Kind::Disabled:
      return "0";
    case Kind::UpToOne:
      return "?";
    case Kind::One:
      return "1";
    case Kind::AtLeastOne: {
      if (m_max == static_cast<size_t>(-1)) {
        return "+";
      }
      return std::to_string(m_min) + ":" + std::to_string(m_max);
    }
    case Kind::Any: {
      if (m_max == static_cast<size_t>(-1)) {
        return "*";
      }
      return std::to_string(m_min) + ":" + std::to_string(m_max);
    }
  }
  return "";
}

bool
Arity::operator<=(Arity const& o) const
{
  if (m_kind != o.m_kind) {
    switch (m_kind) {
      case Kind::One:
        return o.m_kind == Kind::UpToOne or o.m_kind == Kind::AtLeastOne;
      case Kind::AtLeastOne:
      case Kind::UpToOne:
        return o.m_kind == Kind::Any;
      default:
        return false;
    }
  }
  return true;
}

Arity::operator Arity::Kind() const
{
  return m_kind;
}

std::ostream&
operator<<(std::ostream& o, Arity const& arity)
{
  o << arity.toString();
  return o;
}

}}
