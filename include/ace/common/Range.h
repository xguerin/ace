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

#include "String.h"
#include <functional>
#include <string>
#include <vector>

namespace ace { namespace common {

template<typename T, typename C = std::less<T>>
class Range
{
public:
  class Bound
  {
  public:
    Bound();
    Bound(T const& v, const bool o = false);

    T const& value() const;
    bool any() const;
    bool open() const;

    template<typename K>
    bool bounds(T const& v) const;

    bool operator==(Bound const& o) const;
    bool operator!=(Bound const& o) const;

    bool operator<(Bound const& o) const;

  private:
    T m_value;
    bool m_any;
    bool m_open;
  };

public:
  static bool parse(std::string const& s, Range<T, C>& r);

  Range() = default;
  Range(Bound const& l, Bound const& h);

  Bound const& low() const;
  Bound const& high() const;

  bool contains(T const& v) const;
  Range<T, C> intersect(Range const& o) const;

private:
  Bound m_low;
  Bound m_high;
};

template<typename T, typename C>
Range<T, C>::Bound::Bound() : m_value(), m_any(true), m_open(false)
{}

template<typename T, typename C>
Range<T, C>::Bound::Bound(T const& v, const bool o)
  : m_value(v), m_any(false), m_open(o)
{}

template<typename T, typename C>
T const&
Range<T, C>::Bound::value() const
{
  return m_value;
}

template<typename T, typename C>
bool
Range<T, C>::Bound::any() const
{
  return m_any;
}

template<typename T, typename C>
bool
Range<T, C>::Bound::open() const
{
  return m_open;
}

template<typename T, typename C>
template<typename K>
bool
Range<T, C>::Bound::bounds(T const& v) const
{
  K comp = K(C());
  if (m_any) {
    return true;
  }
  /**
   * @brief Catch the bound equivalence early, as binary_negate(<) == (>=)
   */
  if (v == m_value) {
    return not m_open;
  }
  return comp(v, m_value);
}

template<typename T, typename C>
bool
Range<T, C>::Bound::operator==(Bound const& o) const
{
  if (m_any) {
    return m_any == o.m_any;
  }
  return m_value == o.m_value and m_open == o.m_open;
}

template<typename T, typename C>
bool
Range<T, C>::Bound::operator!=(Bound const& o) const
{
  return not operator==(o);
}

template<typename T, typename C>
bool
Range<T, C>::Bound::operator<(Bound const& o) const
{
  if (m_any or o.m_any) {
    return true;
  }
  if (m_value == o.m_value) {
    return m_open != o.m_open;
  }
  return m_value < o.m_value;
}

template<typename T, typename C>
bool
Range<T, C>::parse(std::string const& s, Range<T, C>& r)
{
  auto v(s);
  auto t = String::trim(v);
  if (t.empty()) {
    return false;
  }
  char lc = *t.begin();
  char hc = *t.rbegin();
  if ((lc != '[' and lc != '(') or (hc != ']' and hc != ')')) {
    return false;
  }
  t = t.substr(1, t.length() - 2);
  std::vector<std::string> items;
  String::split(t, ',', items);
  if (items.size() != 2) {
    return false;
  }
  auto slo = String::trim(items[0]);
  auto shi = String::trim(items[1]);
  Bound lb;
  if (slo != "*") {
    if (not String::is<T>(slo)) {
      return false;
    }
    auto lo = String::value<T>(slo);
    lb = { lo, lc == '(' };
  }
  Bound hb;
  if (shi != "*") {
    if (not String::is<T>(shi)) {
      return false;
    }
    auto hi = String::value<T>(shi);
    hb = { hi, hc == ')' };
  }
  if (not lb.any() and not hb.any() and hb < lb) {
    return false;
  }
  r = { lb, hb };
  return true;
}

template<typename T, typename C>
Range<T, C>::Range(Bound const& l, Bound const& h) : m_low(l), m_high(h)
{}

template<typename T, typename C>
typename Range<T, C>::Bound const&
Range<T, C>::low() const
{
  return m_low;
}

template<typename T, typename C>
typename Range<T, C>::Bound const&
Range<T, C>::high() const
{
  return m_high;
}

template<typename T, typename C>
bool
Range<T, C>::contains(T const& v) const
{
  return m_high.template bounds<C>(v) and
         m_low.template bounds<std::binary_negate<C>>(v);
}

template<typename T, typename C>
Range<T, C>
Range<T, C>::intersect(Range<T, C> const& o) const
{
  Bound lb = m_low;
  Bound hb = m_high;
  if (m_low < o.m_low) {
    lb = o.m_low;
  }
  if (o.m_high < m_high) {
    hb = o.m_high;
  }
  return Range<T, C>(lb, hb);
}

template<typename T, typename C>
std::ostream&
operator<<(std::ostream& o, Range<T, C> const& r)
{
  o << (r.low().open() ? "( " : "[ ");
  o << r.low().value();
  o << ", ";
  o << r.high().value();
  o << (r.high().open() ? " )" : " ]");
  return o;
}

}}
