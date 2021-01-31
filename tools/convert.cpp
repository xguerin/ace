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
#include <ace/common/Regex.h>
#include <ace/engine/Master.h>
#include <ace/model/Model.h>
#include <ace/tree/Array.h>
#include <ace/tree/Primitive.h>
#include <ace/types/Class.h>
#include <ace/types/Plugin.h>
#include <ace/types/Selector.h>
#include <tclap/CmdLine.h>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

static ace::tree::Value::Ref
load(std::string const& path, int argc, char* argv[])
{
  if (!MASTER.hasScannerByExtension(path)) {
    ACE_LOG(Error, "Unsupported configuration file format: ", path);
    return ace::tree::Value::Ref();
  }
  ace::tree::Value::Ref svr =
    MASTER.scannerByExtension(path).open(path, argc, argv);
  if (svr.get() == nullptr) {
    ACE_LOG(Error, "Cannot open configuration file \"" + path + "\"");
    return ace::tree::Value::Ref();
  }
  return svr;
}

static bool
convert(std::string const& filename, ace::tree::Value const& v, const bool c)
{
  std::ofstream ofs;
  ofs.open(filename);
  if (ofs.fail()) {
    ACE_LOG(Error, "Can!open file ", filename, " for writing");
    return false;
  }
  auto f = c ? ace::tree::Scanner::Format::Compact
             : ace::tree::Scanner::Format::Default;
  MASTER.scannerByExtension(filename).dump(v, f, ofs);
  ofs.close();
  return true;
}

using SA = TCLAP::SwitchArg;
template<typename T>
using VA = TCLAP::ValueArg<T>;
template<typename T>
using MA = TCLAP::MultiArg<T>;
template<typename T>
using UA = TCLAP::UnlabeledValueArg<T>;

int
main(int argc, char* argv[])
try {
  TCLAP::CmdLine cmd("Advanced Configuration Converter", ' ', ACE_VERSION);
  SA comA("c", "compact", "Compact output", cmd);
  VA<std::string> outA("o", "output", "Output file", true, "", "NAME.EXT", cmd);
  SA vrbA("v", "verbose", "Verbose mode", cmd);
  UA<std::string> cfgA("config", "Configuration file", true, "", "string", cmd);
  cmd.parse(argc, argv);
  /**
   * Output some information about the tool
   */
  ACE_LOG(Info, "Advanced Configuration Converter - ", ACE_VERSION);
  ACE_LOG(Info, "-------------------------------");
  /**
   * Check the availability of scanners
   */
  if (!MASTER.hasScannerByExtension(outA.getValue())) {
    ACE_LOG(Error, "Unsupported format: ", outA.getValue());
    return -1;
  }
  /**
   * Scan the configuration file
   */
  ace::tree::Value::Ref root = load(cfgA.getValue(), argc, argv);
  if (root.get() == nullptr) {
    return -1;
  }
  /**
   * convert the file
   */
  return convert(outA.getValue(), *root, comA.isSet()) ? 0 : 1;
} catch (TCLAP::ArgException const& e) {
  ACE_LOG(Error, e.error(), " for argument ", e.argId());
} catch (std::invalid_argument const& e) {
  ACE_LOG(Error, "Invalid argument: ", e.what());
  return -1;
} catch (std::runtime_error const& e) {
  ACE_LOG(Error, "Runtime error: ", e.what());
  return -1;
}
