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

#pragma once

#include <ace/common/Path.h>
#include <string>
#include <sys/stat.h>

namespace ace {
namespace fs {

class Node {
 public:

  enum class Type {
    Block     = S_IFBLK,
    Character = S_IFCHR,
    Directory = S_IFDIR,
    FIFO      = S_IFIFO,
    Regular   = S_IFREG,
    Link      = S_IFLNK,
    Socket    = S_IFSOCK,
    Unknown   = 0
  };

  typedef enum _Permission {
    SetUID      = S_ISUID,
    SetGID      = S_ISGID,
    Sticky      = S_ISVTX,
    UserRead    = S_IRUSR,
    UserWrite   = S_IWUSR,
    UserExec    = S_IXUSR,
    GroupRead   = S_IRGRP,
    GroupWrite  = S_IWGRP,
    GroupExec   = S_IXGRP,
    OtherRead   = S_IROTH,
    OtherWrite  = S_IWOTH,
    OtherExec   = S_IXOTH,
    None        = 0
  } Permission;

  Node();
  Node(Node const & o); // NOLINT(runtime/explicit)
  virtual ~Node();

  virtual bool isValid() const;
  common::Path const & path() const;

  Type type() const;
  static Type type(common::Path const & p, const bool follow = true);

  Permission permissions() const;
  static Permission permissions(common::Path const & p, const bool follow = true);

  static Type parseType(std::string const & s);
  static std::string toString(const Type t);

  uid_t userID() const;
  static uid_t userID(common::Path const & p, const bool follow = true);

  gid_t groupID() const;
  static gid_t groupID(common::Path const & p, const bool follow = true);

  bool readable() const;
  static bool readable(common::Path const & p, const bool follow = true);

  bool writeable() const;
  static bool writeable(common::Path const & p, const bool follow = true);

  Node parent() const;

  Node & operator=(Node const & o);

 protected:

  static Type type(mode_t mode);
  static Permission permissions(mode_t mode);

  int           m_fd;
  common::Path  m_path;
};

std::ostream &
operator<<(std::ostream & o, Node::Type const & t);

} // namespace ace
} // namespace fs
