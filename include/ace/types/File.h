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

#include <ace/model/EnumeratedType.h>
#include <ace/model/FileModeAttribute.h>
#include <ace/model/FlagAttribute.h>
#include <ace/model/FormatChecker.h>
#include <functional>
#include <set>
#include <string>

namespace ace { namespace model {

// File comparator

struct FileTypeCompare
  : public std::binary_function<std::string, std::string, bool>
{
  bool operator()(std::string const& a, std::string const& b) const;
};

// File format checker

class FileFormatChecker : public FormatChecker<std::string>
{
public:
  FileFormatChecker() = delete;
  explicit FileFormatChecker(const BasicType* o);
  bool operator()(tree::Object const& r, tree::Value const& v) const;

private:
  bool checkFormat(std::string const& n, std::string const& b) const;
};

// File class

class File
  : public EnumeratedType<std::string, false, FileTypeCompare,
                          FileFormatChecker>
{
public:
  File();

  bool validateModel();

  void collectInterfaceIncludes(std::set<std::string>& i) const;
  void collectImplementationIncludes(std::set<std::string>& i) const;

  BasicType::Ref clone(std::string const& n) const;

private:
  FileModeAttribute const& fileModeAttribute() const;

  friend class FileFormatChecker;
};

}}
