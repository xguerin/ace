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

#include <ace/common/Log.h>
#include <ace/tree/Path.h>
#include <tclap/CmdLine.h>

using SA = TCLAP::SwitchArg;
template<typename T> using VA = TCLAP::ValueArg<T>;
template<typename T> using MA = TCLAP::MultiArg<T>;
template<typename T> using UA = TCLAP::UnlabeledMultiArg<T>;

using namespace ace::tree;

std::string
dot(path::Item const & i) {
  if (i.recursive()) return "..";
  return ".";
}

void
parse(std::vector<std::string> const & paths) {
  for (auto const & s : paths) {
    auto path = Path::parse(s);
    /**
     * Print the path
     */
    for (auto const & e : path) {
      /*
       * Check the item
       */
      switch (e->type()) {
        case path::Item::Type::Any : {
          std::cout << dot(*e) << "Any(*)";
        } break;
        case path::Item::Type::Global : {
          std::cout << "Global($)";
        } break;
        case path::Item::Type::Local : {
          std::cout << "Local(@)";
        } break;
        case path::Item::Type::Named : {
          std::cout << dot(*e) << "Named(" << e->value() << ")";
        } break;
        case path::Item::Type::Indexed : {
          std::cout << dot(*e) << "Indexed(";
          auto & indexes = e->indexes();
          for (size_t i = 0; i < indexes.size(); i += 1) {
            std::cout << indexes[i];
            if (i + 1 < indexes.size()) std::cout << ",";
          }
          std::cout << ")";
        } break;
        case path::Item::Type::Ranged : {
          std::cout << dot(*e) << "Ranged(";
          std::cout << e->range().low;
          std::cout << ":";
          std::cout << e->range().high;
          std::cout << ":";
          std::cout << e->range().steps;
          std::cout << ")";
        } break;
      }
    }
    std::cout << std::endl;
  }
}

void
match(std::vector<std::string> const & paths) {
  if (paths.size() != 2) {
    ACE_LOG(Error, "Exactly two paths must be provided for matching");
  } else {
    auto a = Path::parse(paths[0]);
    auto b = Path::parse(paths[1]);
    if (a.match(b)) {
      std::cout << "Match" << std::endl;
    } else {
      std::cout << "Not a match" << std::endl;
    }
  }
}

int main(int argc, char * argv[]) try {
  TCLAP::CmdLine cmd("Advanced Configuration Path Checker", ' ', ACE_VERSION);
  SA              cmpA("m", "match", "Match two paths", cmd);
  UA<std::string> pthA("path", "JSONPath", true, "string", cmd);
  cmd.parse(argc, argv);
  /**
   * Output some information about the tool
   */
  ACE_LOG(Info, "Advanced Configuration Patch Checker - ", ACE_VERSION);
  ACE_LOG(Info, "-------------------------------");
  /**
   * Parse the paths
   */
  if (cmpA.isSet()) {
    match(pthA.getValue());
  } else {
    parse(pthA.getValue());
  }
  /**
   * Return
   */
  return 0;
}
catch (std::invalid_argument const & e) {
  return __LINE__;
}
