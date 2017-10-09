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

#include <ace/common/Arguments.h>
#include <cstdlib>
#include <cstring>

namespace ace {
namespace common {
namespace Arguments {

char **
normalize(const int argc, const char * const argv[]) {
  if (argc <= 0) return nullptr;
  char ** r = static_cast<char **>(calloc(argc, sizeof(char *)));
  for (int i = 0; i < argc; i += 1) {
    if (strncmp(argv[i], "-L", 2) == 0 or strncmp(argv[i], "-I", 2) == 0) {
      const size_t arglen = strlen(argv[i]) + 2;
      r[i] = static_cast<char *>(calloc(arglen, sizeof(char)));
      strncpy(r[i], argv[i], 2);
      r[i][2] = ' ';
#if defined(__OpenBSD__)
      strlcpy(&r[i][3], &argv[i][2], arglen);
#else
      strcpy(&r[i][3], &argv[i][2]);
#endif
    } else {
      r[i] = strdup(argv[i]);
    }
  }
  return r;
}

} // namespace Arguments
} // namespace common
} // namespace ace


