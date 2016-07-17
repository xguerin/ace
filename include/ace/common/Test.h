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

#ifndef ACE_COMMON_TEST_H_
#define ACE_COMMON_TEST_H_

#include <iostream>

namespace ace {
namespace common {

#define STR(v) #v

#define T(a) if (not (a)) return __LINE__
#define F(a) if ((a)) return __LINE__

#define E(a, b) if ((a) != (b)) {                                  \
  std::cerr << STR(a) << " (" << a << ") != " << b << std::endl;   \
  return __LINE__;                                                 \
}

#define N(a, b) if ((a) == (b)) {                                  \
  std::cerr << STR(a) << " (" << a << ") == " << b << std::endl;   \
  return __LINE__;                                                 \
}

} // namespace common
} // namespace ace


#endif  // ACE_COMMON_TEST_H_
