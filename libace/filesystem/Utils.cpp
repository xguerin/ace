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

#include <ace/filesystem/Utils.h>
#include <ace/filesystem/Directory.h>
#include <ace/filesystem/File.h>
#include <cstring>
#include <iostream>
#include <string>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <libgen.h>
#include <limits.h>
#include <unistd.h>

namespace ace {
namespace fs {
namespace Utils {

bool
canSelfCreate(common::Path const & p) {
  common::Path fullPath(p);
  if (not p.isAbsolute()) fullPath = fs::Directory().path() / p;

  if (fullPath.isDirectory() and fs::Directory(fullPath).isValid()) {
    return false;
  } else if (fs::File(p).isValid()) {
    return false;
  }

  uid_t uid = getuid();
  gid_t gid = getgid();

  do {
    fullPath = fullPath.prune();
    fs::Directory dir(fullPath);
    if (dir.isValid()) {
      if (dir.permissions() & Node::Permission::OtherWrite) return true;
      if (dir.groupID() == gid and dir.permissions() & Node::Permission::GroupWrite) return true;
      if (dir.userID() == uid and dir.permissions() & Node::Permission::UserWrite) return true;
      return false;
    }
  } while (fullPath != common::Path("/"));

  return false;
}

std::string
processPrefix() {
  char buffer[4096];
  ssize_t n = readlink("/proc/self/exe", buffer, sizeof(buffer));
  buffer[n] = '\0';
  return std::string(dirname(dirname(buffer)));
}

std::string
libraryPrefix() {
  Dl_info libInfo;
  if (dladdr(reinterpret_cast<void *>(&libraryPrefix), &libInfo) != 0) {
    char buffer[4096];
#if defined(__OpenBSD__)
    strlcpy(buffer, libInfo.dli_fname, 4096);
#else
    strcpy(buffer, libInfo.dli_fname);
#endif
    return std::string(dirname(dirname(buffer)));
  }
  return "";
}

} // namespace Utils
} // namespace fs
} // namespace ace

