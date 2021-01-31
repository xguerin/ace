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

#include <ace/filesystem/File.h>
#include <ace/filesystem/Directory.h>
#include <string>
#include <fcntl.h>
#include <limits.h>
#include <strings.h>
#include <unistd.h>

namespace ace { namespace fs {

File::File(fs::Path const& p) : Node()
{
  if (not p.empty() and not p.isDirectory()) {
    fs::Path fullPath(p);
    if (not p.isAbsolute()) {
      char buffer[PATH_MAX];
      bzero(buffer, PATH_MAX);
      getcwd(buffer, PATH_MAX);
      std::string base(buffer);
      if (*base.rbegin() != '/') {
        base += "/";
      }
      fullPath = fs::Path(base) / p;
    }
    m_fd = open(fullPath.toString().c_str(), 0, O_RDWR);
    m_path = fullPath;
  }
}

Node
File::parent() const
{
  if (m_fd == -1) {
    return Node();
  }
  fs::Path up("/");
  if (m_path != up) {
    up = m_path.prune();
  }
  return fs::Directory(up);
}

}}
