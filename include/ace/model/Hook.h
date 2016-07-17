/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef ACE_MODEL_HOOK_H_
#define ACE_MODEL_HOOK_H_

#include <ace/tree/Path.h>
#include <string>

namespace ace {
namespace model {

class Hook {
 public:

  Hook() { }

  static bool validate(std::string const & s);
  void load(std::string const & s);

  bool match(std::string const & s) const;
  bool transform(std::string const & v, std::string & r) const;

  tree::Path const & path() const;
  std::string const & pattern() const;
  std::string const & value() const;

 private:

  tree::Path  m_path;
  std::string m_pattern;
  std::string m_value;
};

} // namespace model
} // namespace ace

#endif  // ACE_MODEL_HOOK_H_
