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

#include <ace/filesystem/Path.h>
#include <ace/tree/Scanner.h>
#include <string>

namespace ace { namespace pyfmt {

class Scanner : public tree::Scanner
{
public:
  Scanner() = default;

  tree::Value::Ref open(std::string const& fn, int argc, char** argv);
  tree::Value::Ref parse(std::string const& s, int argc, char** argv);
  void dump(tree::Value const& v, const Format f, std::ostream& o) const;

  bool openAll(std::string const& fn, int argc, char** argv,
               std::list<tree::Value::Ref>& values);
  bool parseAll(std::string const& s, int argc, char** argv,
                std::list<tree::Value::Ref>& values);
  bool dumpAll(std::list<tree::Value::Ref>& values, const Format f,
               std::ostream& o) const;

  std::string name() const;
  std::string extension() const;

private:
  bool setUp(fs::Path const& path, int argc, char** argv) noexcept;
};

}}
