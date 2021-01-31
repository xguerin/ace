/** Copyright (c) 2016 Xavier R. Guerin
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

#include "Scanner.h"
#include "Common.h"
#include "Object.h"
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <ace/engine/Master.h>
#include <string>
#include <vector>

namespace ace { namespace yamlfmt {

tree::Value::Ref
Scanner::open(std::string const& fn, int argc, char** argv)
{
  return Common::parseFile(fn);
}

tree::Value::Ref
Scanner::parse(std::string const& s, int argc, char** argv)
{
  return Common::parseString(s);
}

void
Scanner::dump(tree::Value const& v, const Format f, std::ostream& o) const
{
  YAML::Emitter e;
  e << YAML::BeginDoc;
  Common::dump(v, e);
  e << YAML::EndDoc;
  o << e.c_str();
}

bool
Scanner::openAll(std::string const& fn, int argc, char** argv,
                 std::list<tree::Value::Ref>& values)
{
  return false;
}

bool
Scanner::parseAll(std::string const& s, int argc, char** argv,
                  std::list<tree::Value::Ref>& values)
{
  return false;
}

bool
Scanner::dumpAll(std::list<tree::Value::Ref>& values, const Format f,
                 std::ostream& o) const
{
  /*
   * Make sure there is something to dump.
   */
  if (values.empty()) {
    return false;
  }
  /*
   * Dump the values.
   */
  YAML::Emitter e;
  for (auto const& r : values) {
    e << YAML::BeginDoc;
    Common::dump(*r, e);
    e << YAML::EndDoc;
  }
  o << e.c_str();
  return true;
}

std::string
Scanner::name() const
{
  return "yaml";
}

std::string
Scanner::extension() const
{
  return "yaml";
}

}}

extern "C" {
void*
loadPlugin()
{
  return new ace::yamlfmt::Scanner();
}
}
