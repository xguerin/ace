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

#include "Array.h"
#include "Common.h"
#include <ace/common/Log.h>
#include <ace/common/String.h>
#include <ace/tree/Array.h> // NOLINT
#include <string>

namespace ace {
namespace inifmt {
namespace Array {

void
dump(tree::Value const & v, std::ostream & o) {
  auto const & a = static_cast<tree::Array const &>(v);
  for (size_t i = 0; i < a.size(); i += 1) {
    if (a.at(i)->type() == tree::Value::Type::Array) {
      ACE_LOG(Error, "Arrays of arrays not supported in the INI format");
    } else if (a.at(i)->type() == tree::Value::Type::Object) {
      ACE_LOG(Error, "Arrays of objects not supported in the INI format");
    } else {
      Common::dump(*a.at(i), o);
      if (i < a.size() - 1) o << ", ";
    }
  }
}

} // namespace Array
} // namespace inifmt
} // namespace ace
