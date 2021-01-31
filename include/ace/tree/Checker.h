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
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace ace { namespace tree {

class Checker
{
public:
  class Pattern
  {
  public:
    Pattern();
    Pattern(const Value::Type t, const bool o);
    Pattern(std::vector<Value::Type> const& t, const bool o);

    std::vector<Value::Type> const& types() const;
    bool optional() const;

  private:
    std::vector<Value::Type> m_types;
    bool m_opt;
  };

  using Schema = std::map<std::string, Pattern>;

public:
  Checker() = delete;
  Checker(std::string const& hdr, Value const& t);

  bool validate(Schema const& schm) const;

private:
  const std::string m_hdr;
  Value const& m_token;
};

}}
