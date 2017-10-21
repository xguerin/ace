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

#include <ace/filesystem/Directory.h>
#include <ace/filesystem/File.h>
#include <iostream>
#include <string>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

namespace ace {
namespace fs {

Directory::Directory() : Node() {
  char buffer[PATH_MAX];
  bzero(buffer, PATH_MAX);
  getcwd(buffer, PATH_MAX);
  std::string base(buffer);
  if (*base.rbegin() != '/') base += "/";
  m_path = fs::Path(base);
  m_fd = open(m_path.toString().c_str(), 0, O_RDWR);
}

Directory::Directory(fs::Path const & p) : Node() {
  if (not p.empty() and p.isDirectory()) {
    fs::Path fullPath(p);
    if (not p.isAbsolute()) {
      char buffer[PATH_MAX];
      bzero(buffer, PATH_MAX);
      getcwd(buffer, PATH_MAX);
      std::string base(buffer);
      if (*base.rbegin() != '/') base += "/";
      fullPath = fs::Path(base) / p;
    }
    DIR * dir = opendir(fullPath.toString().c_str());
    if (dir != nullptr) {
      m_fd = open(fullPath.toString().c_str(), 0, O_RDWR);
      m_path = fullPath;
      closedir(dir);
    }
  }
}

bool Directory::has(fs::Path const & p) const {
  return has(p, p.begin());
}

void Directory::each(std::function<void(Node const &)> op) const {
  struct dirent * ep;
  int ofd = dup(m_fd);
  if (ofd < 0) return;
  DIR * dir = fdopendir(ofd);
  if (dir == nullptr) {
    close(ofd);
    return;
  }
  rewinddir(dir);
  while ((ep = readdir(dir)) != nullptr) {
    if (ep->d_type == DT_DIR) {
      op(fs::Directory(m_path / fs::Path(ep->d_name, true)));
    } else {
      op(fs::File(m_path / fs::Path(ep->d_name)));
    }
  }
  /**
   * It is unclear whether or not closedir() actually close the underlying FD.
   * Coverity does not think so, so we do it here as well.
   */
  closedir(dir);
  close(ofd);
}

bool Directory::has(fs::Path const & p, fs::Path::const_iterator const & i) const {
  if (p.isAbsolute()) return false;
  if (i->empty()) return true;
  int ofd = dup(m_fd);
  if (ofd < 0) return false;
  DIR * dir = fdopendir(ofd);
  if (dir == nullptr) {
    close(ofd);
    return false;
  }
  rewinddir(dir);
  bool result = false;
  struct dirent * ep;
  while ((ep = readdir(dir)) != nullptr) if (*i == ep->d_name) {
    if (p.down(i) != p.end()) {
      result = Directory(m_path / fs::Path(*i, true)).has(p, p.down(i));
      break;
    } else {
      result = ep->d_type != DT_DIR;
      break;
    }
  }
  /**
   * It is unclear whether or not closedir() actually close the underlying FD.
   * Coverity does not think so, so we do it here as well.
   */
  closedir(dir);
  close(ofd);
  return result;
}

Node
Directory::parent() const {
  if (m_fd == -1) return Node();
  fs::Path up("/");
  if (m_path != up) up = m_path.prune();
  return fs::Directory(up);
}

} // namespace ace
} // namespace fs

