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

#include <ace/types/File.h>
#include <ace/common/String.h>
#include <ace/filesystem/Directory.h>
#include <ace/filesystem/File.h>  // NOLINT
#include <ace/filesystem/Node.h>
#include <ace/filesystem/Utils.h>
#include <ace/tree/Checker.h>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <fcntl.h>
#include <unistd.h>

namespace ace {
namespace model {

// File comparator

bool
FileTypeCompare::operator() (std::string const & a, std::string const & b) const {
  if (a == "auto") return true;
  common::Path path(a);
  fs::Node::Type t = fs::Node::type(a);
  if (t != fs::Node::Type::Unknown) {
    return fs::Node::toString(t) == b;
  } else if (path.isDirectory()) {
    return b == "directory";
  } else {
    return true;
  }
}

// File format checker

FileFormatChecker::FileFormatChecker(const BasicType * o) : FormatChecker(o) { }

bool
FileFormatChecker::operator() (tree::Object const & r, tree::Value const & v) const {
  if (not FormatChecker<std::string>::operator()(r, v)) return false;
  int score = 0;
  v.each([&](tree::Value const & w) {
    tree::Primitive const & p = static_cast<tree::Primitive const &>(w);
    if (p.value() != "auto" and not checkFormat(v.name(), p.value())) score += 1;
  });
  return score == 0;
}

bool
FileFormatChecker::checkFormat(std::string const & n, std::string const & b) const {
  File const & file = *dynamic_cast<const File *>(m_owner);
  int flags = file.fileModeAttribute().flags();
  auto path = common::Path(b);
  switch (fs::Node::type(path)) {
    case fs::Node::Type::Unknown : {
      if ((flags & O_CREAT) == 0) {
        ERROR_O(m_owner, "file \"", n, "\": ", ERR_FILE_NO_SUCH_FILE(b));
        return false;
      }
      if (not fs::Utils::canSelfCreate(b)) {
        ERROR_O(m_owner, "file \"", n, "\": ", ERR_FILE_CANNOT_BE_CREATED(b));
        return false;
      }
    } break;
    default: {
      std::string mode = file.fileModeAttribute().value();
      if ((flags & O_RDONLY or flags & O_RDWR) and not fs::Node::readable(path)) {
        ERROR_O(m_owner, "file \"", n, "\": ", ERR_FILE_CANNOT_OPEN_MODE(b, mode));
      }
      if ((flags & O_WRONLY or flags & O_RDWR) and not fs::Node::writeable(path)) {
        ERROR_O(m_owner, "file \"", n, "\": ", ERR_FILE_CANNOT_OPEN_MODE(b, mode));
      }
    } break;
  }
  return true;
}

// File class

File::File() : Type(BasicType::Kind::File), EnumeratedType(BasicType::Kind::File) {
  m_attributes.define<FileModeAttribute>("mode", false);
}

bool
File::validateModel() {
  if (not EnumeratedType::validateModel()) return false;
  int score = 0;
  if (hasEitherAttribute()) for (auto & e : eitherAttribute().values()) {
    if (fs::Node::parseType(e) == fs::Node::Type::Unknown) {
      ERROR(ERR_INVALID_EITHER_FILE_TYPE(e));
      score += 1;
    }
  }
  return score == 0;
}

void
File::collectInterfaceIncludes(std::set<std::string> & i) const {
  BasicType::collectInterfaceIncludes(i);
  i.insert("<string>");
}

void
File::collectImplementationIncludes(std::set<std::string> & i) const {
  BasicType::collectInterfaceIncludes(i);
  i.insert("<string>");
}

BasicType::Ref
File::clone(std::string const & n) const {
  File * uri = new File(*this);
  uri->setName(n);
  return BasicType::Ref(uri);
}

FileModeAttribute const &
File::fileModeAttribute() const {
  Attribute const & attr = *this->m_attributes["mode"];
  return static_cast<FileModeAttribute const &>(attr);
}

} // namespace model
} // namespace ace
