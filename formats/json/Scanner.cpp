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

#include <ace/formats/json/Scanner.h>
#include <ace/formats/json/Common.h>
#include <ace/formats/json/Object.h>
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <ace/engine/Master.h>
#include <string>
#include <vector>

namespace ace {
namespace jsonfmt {

tree::Value::Ref
Scanner::open(std::string const & fn, int argc, char ** argv) {
  return Common::parseFile(fn);
}

tree::Value::Ref
Scanner::parse(std::string const & s, int argc, char ** argv) {
  return Common::parseString(s);
}

void
Scanner::dump(tree::Value const & v, const Format f, std::ostream & o) const {
  Common::dump(v, f, o, 0, false);
  if (f == Format::Default) o << std::endl;
}

std::string
Scanner::name() const {
  return "json";
}

std::string
Scanner::extension() const {
  return "json";
}

} // namespace jsonfmt
} // namespace ace

extern "C" {
void * loadPlugin() {
  return new ace::jsonfmt::Scanner();
}
}
