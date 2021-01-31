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

#include <ace/filesystem/Node.h>
#include <map>
#include <string>
#include <pwd.h>
#include <unistd.h>

namespace ace { namespace fs {

Node::Node() : m_fd(-1), m_path() {}

Node::Node(Node const& o) : m_fd(o.m_fd), m_path(o.m_path)
{
  if (o.m_fd != -1) {
    m_fd = dup(o.m_fd);
  }
}

Node::~Node()
{
  if (m_fd != -1) {
    close(m_fd);
  }
}

bool
Node::isValid() const
{
  return m_fd != -1;
}

fs::Path const&
Node::path() const
{
  return m_path;
}

Node::Type
Node::type() const
{
  if (m_fd == -1) {
    return Type::Unknown;
  }
  struct stat st;
  if (fstat(m_fd, &st) != 0) {
    return Type::Unknown;
  }
  return type(st.st_mode);
}

Node::Permission
Node::permissions() const
{
  if (m_fd == -1) {
    return Permission::None;
  }
  struct stat st;
  if (fstat(m_fd, &st) != 0) {
    return Permission::None;
  }
  return permissions(st.st_mode);
}

Node::Type
Node::type(fs::Path const& p, const bool follow)
{
  struct stat st;
  if (follow) {
    if (stat(p.toString().c_str(), &st) != 0) {
      return Type::Unknown;
    }
  } else {
    if (lstat(p.toString().c_str(), &st) != 0) {
      return Type::Unknown;
    }
  }
  return type(st.st_mode);
}

Node::Permission
Node::permissions(fs::Path const& p, const bool follow)
{
  struct stat st;
  if (follow) {
    if (stat(p.toString().c_str(), &st) != 0) {
      return Permission::None;
    }
  } else {
    if (lstat(p.toString().c_str(), &st) != 0) {
      return Permission::None;
    }
  }
  return permissions(st.st_mode);
}

Node::Type
Node::parseType(std::string const& s)
{
  const std::map<std::string, ace::fs::Node::Type> stringToType = {
    { "block", ace::fs::Node::Type::Block },
    { "character", ace::fs::Node::Type::Character },
    { "directory", ace::fs::Node::Type::Directory },
    { "fifo", ace::fs::Node::Type::FIFO },
    { "regular", ace::fs::Node::Type::Regular }
  };
  if (stringToType.count(s) != 0) {
    return stringToType.at(s);
  }
  return Type::Unknown;
}

std::string
Node::toString(const Type t)
{
  const std::map<ace::fs::Node::Type, std::string> typeToString = {
    { ace::fs::Node::Type::Block, "block" },
    { ace::fs::Node::Type::Character, "character" },
    { ace::fs::Node::Type::Directory, "directory" },
    { ace::fs::Node::Type::FIFO, "fifo" },
    { ace::fs::Node::Type::Regular, "regular" }
  };
  if (typeToString.count(t) != 0) {
    return typeToString.at(t);
  }
  return "unknown";
}

uid_t
Node::userID() const
{
  if (m_fd == -1) {
    return -1;
  }
  struct stat st;
  if (fstat(m_fd, &st) != 0) {
    return -1;
  }
  return st.st_uid;
}

uid_t
Node::userID(fs::Path const& p, const bool follow)
{
  struct stat st;
  if (follow) {
    if (stat(p.toString().c_str(), &st) != 0) {
      return -1;
    }
  } else {
    if (lstat(p.toString().c_str(), &st) != 0) {
      return -1;
    }
  }
  return st.st_uid;
}

gid_t
Node::groupID() const
{
  if (m_fd == -1) {
    return -1;
  }
  struct stat st;
  if (fstat(m_fd, &st) != 0) {
    return -1;
  }
  return st.st_gid;
}

gid_t
Node::groupID(fs::Path const& p, const bool follow)
{
  struct stat st;
  if (follow) {
    if (stat(p.toString().c_str(), &st) != 0) {
      return -1;
    }
  } else {
    if (lstat(p.toString().c_str(), &st) != 0) {
      return -1;
    }
  }
  return st.st_gid;
}

bool
Node::readable() const
{
  auto perms = permissions();
  if (perms & Permission::OtherRead) {
    return true;
  }
  auto uid = getuid();
  auto gid = getgid();
  if (userID() == uid and perms & Permission::UserRead) {
    return true;
  }
  if (groupID() == gid and perms & Permission::GroupRead) {
    return true;
  }
  return false;
}

bool
Node::readable(fs::Path const& p, const bool follow)
{
  auto perms = permissions(p, follow);
  if (perms & Permission::OtherRead) {
    return true;
  }
  auto uid = getuid();
  auto gid = getgid();
  if (userID(p, follow) == uid and perms & Permission::UserRead) {
    return true;
  }
  if (groupID(p, follow) == gid and perms & Permission::GroupRead) {
    return true;
  }
  return false;
}

bool
Node::writeable() const
{
  auto perms = permissions();
  if (perms & Permission::OtherWrite) {
    return true;
  }
  auto uid = getuid();
  auto gid = getgid();
  if (userID() == uid and perms & Permission::UserWrite) {
    return true;
  }
  if (groupID() == gid and perms & Permission::GroupWrite) {
    return true;
  }
  return false;
}

bool
Node::writeable(fs::Path const& p, const bool follow)
{
  auto perms = permissions(p, follow);
  if (perms & Permission::OtherWrite) {
    return true;
  }
  auto uid = getuid();
  auto gid = getgid();
  if (userID(p, follow) == uid and perms & Permission::UserWrite) {
    return true;
  }
  if (groupID(p, follow) == gid and perms & Permission::GroupWrite) {
    return true;
  }
  return false;
}

Node
Node::parent() const
{
  return Node();
}

Node&
Node::operator=(Node const& o)
{
  m_path = o.m_path;
  m_fd = o.m_fd;
  if (o.m_fd != -1) {
    m_fd = dup(o.m_fd);
  }
  return *this;
}

Node::Type
Node::type(mode_t mode)
{
  return static_cast<Type>(mode & S_IFMT);
}

Node::Permission
Node::permissions(mode_t mode)
{
  return static_cast<Permission>(mode & ~S_IFMT);
}

std::ostream&
operator<<(std::ostream& o, Node::Type const& t)
{
  switch (t) {
    case Node::Type::Block:
      o << "Block";
      break;
    case Node::Type::Character:
      o << "Character";
      break;
    case Node::Type::Directory:
      o << "Directory";
      break;
    case Node::Type::FIFO:
      o << "FIFO";
      break;
    case Node::Type::Regular:
      o << "Regular";
      break;
    case Node::Type::Link:
      o << "Link";
      break;
    case Node::Type::Socket:
      o << "Socket";
      break;
    case Node::Type::Unknown:
      o << "Unknown";
      break;
  }
  return o;
}

}}
