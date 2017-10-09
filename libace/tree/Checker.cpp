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

#include <ace/tree/Checker.h>
#include <ace/tree/Object.h>
#include <ace/common/Log.h>
#include <iostream>
#include <string>
#include <vector>

namespace ace {
namespace tree {

// Pattern

Checker::Pattern::Pattern()
  : m_types(), m_opt(false) { }

Checker::Pattern::Pattern(const Value::Type t, const bool o)
  : m_types({ t }), m_opt(o) { }

Checker::Pattern::Pattern(std::vector<Value::Type> const & t, const bool o)
  : m_types(t), m_opt(o) { }

std::vector<Value::Type> const &
Checker::Pattern::types() const {
  return m_types;
}

bool
Checker::Pattern::optional() const {
  return m_opt;
}

// Checker

Checker::Checker(std::string const & hdr, Value const & t)
    : m_hdr(hdr), m_token(t) { }

bool Checker::validate(Schema const & schm) const {
  size_t score = 0;
  Schema wSchm(schm);
  if (not m_token.isObject()) {
    ACE_LOG(Error, "must be an Object");
    return false;
  }
  auto const & obj = static_cast<tree::Object const &>(m_token);
  for (auto & t : obj) {
    if (wSchm.find(t.first) != wSchm.end()) {
      bool match = false;
      for (auto & u : wSchm[t.first].types()) if (t.second->type() == u) {
        match = true;
        break;
      }
      if (not match) {
        ACE_LOG(Error, "[" + m_hdr + "] Wrong type for \"", t.first, "\"");
        score += 1;
      } else {
        wSchm.erase(t.first);
      }
    } else {
      ACE_LOG(Error, "[" + m_hdr + "] Invalid entry \"", t.first, "\"");
      score += 1;
    }
  }
  for (auto & e : wSchm) if (not e . second . optional()) {
    ACE_LOG(Error, "missing \"", e.first, "\"");
    score += 1;
  }
  return score == 0;
}

} // namespace tree
} // namespace ace
