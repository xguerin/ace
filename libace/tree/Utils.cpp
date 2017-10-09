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

#include <ace/tree/Utils.h>
#include <ace/common/Log.h>
#include <string>
#include <vector>

namespace ace {
namespace tree {
namespace utils {

static tree::Value::Ref
buildPrimitive(std::string const & n, std::string const & s) {
  if (ace::common::String::is<bool>(s)) {
    bool v = common::String::value<bool>(s);
    return tree::Primitive::build(n, v);
  } else if (common::String::is<double>(s)) {
    double v = common::String::value<double>(s);
    return tree::Primitive::build(n, v);
  } else if (common::String::is<long>(s)) {
    long v = common::String::value<long>(s);
    return tree::Primitive::build(n, v);
  } else {
    std::string v(s);
    if (not v.empty() and v.size() > 1) if (*v.begin() == '"' and *v.rbegin() == '"') {
      v.erase(v.begin());
      v.erase(--v.end());
    }
    return tree::Primitive::build(n, v);
  }
}

static tree::Value::Ref
buildArray(std::string const & n, std::vector<std::string> const & fields) {
  tree::Value::Ref v(nullptr);
  tree::Array::Ref array = tree::Array::build("");
  for (auto & f : fields) {
    v = tree::Value::Ref(buildPrimitive("", f));
    array->push_back(v);
  }
  return tree::Value::Ref(array);
}

enum class StringState {
  AnyCapture,
  ArraySplit,
  StringCapture
};

bool
processArrayString(std::string const & n, std::string const & in,
                   std::vector<std::string> & result) {
  StringState state = StringState::ArraySplit;
  std::string candidate;
  for (auto & c : in) switch (state) {
    case StringState::AnyCapture : {
      if (c == '"') {
        state = StringState::ArraySplit;
      }
      candidate.push_back(c);
    } break;
    case StringState::ArraySplit : {
      if (c == '"') {
        if (not candidate.empty()) {
          ACE_LOG(Error, "Duplicate double-quote in value \"", n, "\" at index ", result.size());
          return false;
        }
        state = StringState::AnyCapture;
        candidate.push_back(c);
      } else if (c == ',') {
        if (candidate.empty()) {
          ACE_LOG(Error, "Empty element in value \"", n, "\" at index ", result.size());
          return false;
        }
        result.push_back(common::String::trim(candidate));
        candidate.clear();
      } else {
        candidate.push_back(c);
        state = StringState::StringCapture;
      }
    } break;
    case StringState::StringCapture : {
      if (c == '"') {
        ACE_LOG(Error, "Misused double-quote in value \"", n, "\" at index ", result.size());
        return false;
      } else if (c == ',') {
        result.push_back(common::String::trim(candidate));
        candidate.clear();
        state = StringState::ArraySplit;
      } else {
        candidate.push_back(c);
      }
    } break;
  }
  if (not candidate.empty()) {
    result.push_back(common::String::trim(candidate));
    candidate.clear();
  }
  return true;
}

tree::Value::Ref
buildPrimitiveOrArray(std::string const & n, std::string const & content) {
  std::vector<std::string> fields;
  if (not processArrayString(n, content, fields) or fields.size() == 0) {
    return nullptr;
  }
  if (fields.size() == 1) {
    return buildPrimitive(n, content);
  } else {
    return buildArray(n, fields);
  }
}

void
illegalValueAccess(std::string const & n) {
  std::cerr << "FATAL: in " << n << std::endl;
  std::cerr << "Illegal access to absent option value ";
  std::cerr << "(checker function must be used before accessing (?,*) options)";
  std::cerr << std::endl << "Aborting !" << std::endl;
}

} // namespace utils
} // namespace tree
} // namespace ace

