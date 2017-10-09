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

#include <ace/types/Integer.h>
#include <ace/tree/Checker.h>
#include <ace/tree/Primitive.h>
#include <string>

namespace ace {
namespace model {

Integer::Integer()
    : Type(BasicType::Kind::Integer),
    RangedType(BasicType::Kind::Integer),
    EnumeratedType(BasicType::Kind::Integer) { }

bool
Integer::validateModel() {
  return RangedType::validateModel() and EnumeratedType::validateModel();
}

bool
Integer::checkInstance(tree::Object const & r, tree::Value const & v) const {
  return RangedType::checkInstance(r, v) and EnumeratedType::checkInstance(r, v);
}

BasicType::Ref
Integer::clone(std::string const & n) const {
  Integer * it = new Integer(*this);
  it->setName(n);
  return BasicType::Ref(it);
}

} // namespace model
} // namespace ace

