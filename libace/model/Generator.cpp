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

#include <ace/model/Generator.h>
#include <string>

namespace ace {
namespace model {

template<>
std::string
Generator::nameFor<bool>() {
  return "bool";
}

template<>
std::string
Generator::nameFor<int>() {
  return "int";
}

template<>
std::string
Generator::nameFor<long>() {
  return "long";
}

template<>
std::string
Generator::nameFor<float>() {
  return "float";
}

template<>
std::string
Generator::nameFor<double>() {
  return "double";
}

template<>
std::string
Generator::nameFor<std::string>() {
  return "std::string";
}

std::ostream &
Generator::indent(std::ostream & o, int level) {
  for (int i = 0; i < level; i += 1) o << " ";
  return o;
}

std::string
Generator::tempName() {
  return "__tmp_" + std::to_string(s_tmpId++);
}

size_t Generator::s_tmpId = 1;

} // namespace model
} // namespace ace

