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

#include <ace/model/FileModeAttribute.h>
#include <string>
#include <fcntl.h>

namespace ace {
namespace model {

FileModeAttribute::FileModeAttribute(std::string const & n, bool o)
  : Attribute(n, o), m_value(), m_flags(0) { }

bool
FileModeAttribute::checkModel(tree::Value const & t) const {
  if (t.type() != tree::Value::typeOf<std::string>()) return false;
  std::string v = static_cast<tree::Primitive const &>(t).value<std::string>();
  if (v.empty() or v.length() > 2) return false;
  switch (v[0]) {
    case 'r': case 'w': case 'a': break;
    default: return false;
  }
  if (v.length() == 2 and v[1] != '+') return false;
  return true;
}

// r  Open text file for reading.
//    The stream is positioned at the beginning of the file.
// r+ Open for reading and writing.
//    The stream is positioned at the beginning of the file.
// w  Truncate file to zero length or create text file for writing.
//    The stream is positioned at the beginning of the file.
// w+ Open for reading and writing.
//    The file is created if it does not exist, otherwise it is truncated.
//    The stream is positioned at the beginning of the file.
// a  Open for appending (writing at end of file).
//    The file is created if it does not exist.
//    The stream is positioned at the end of the file.
// a+ Open for reading and appending (writing at end of file).
//    The file is created if it does not exist.
//    The initial file position for reading is at the beginning of the file,
//    but output is always appended to the end of the file.

void
FileModeAttribute::loadModel(tree::Value const & t) {
  m_value = static_cast<tree::Primitive const &>(t).value<std::string>();
  if (m_value == "r") {
    m_flags = O_RDONLY;
  } else if (m_value == "r+") {
    m_flags = O_RDWR;
  } else if (m_value == "w") {
    m_flags = O_CREAT | O_WRONLY | O_TRUNC;
  } else if (m_value == "w+") {
    m_flags = O_CREAT | O_RDWR | O_TRUNC;
  } else if (m_value == "a") {
    m_flags = O_CREAT | O_WRONLY | O_APPEND;
  } else if (m_value == "a+") {
    m_flags = O_CREAT | O_RDWR | O_APPEND;
  }
}

void
FileModeAttribute::load(Attribute const & a) {
  FileModeAttribute const & ra = static_cast<FileModeAttribute const &>(a);
  m_value = ra.m_value;
  m_flags = ra.m_flags;
}

bool
FileModeAttribute::validate(tree::Object const & r, tree::Value const & v) const {
  return true;
}

FileModeAttribute::operator tree::Checker::Pattern() const {
  return tree::Checker::Pattern(tree::Value::Type::String, false);
}

FileModeAttribute::operator std::string() const {
  return m_value;
}

Attribute::Ref
FileModeAttribute::clone() const {
  return Attribute::Ref(new FileModeAttribute(*this));
}

std::string const &
FileModeAttribute::value() const {
  return m_value;
}

int
FileModeAttribute::flags() const {
  return m_flags;
}

} // namespace model
} // namespace ace

