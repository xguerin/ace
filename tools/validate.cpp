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

#include <ace/common/Arguments.h>
#include <ace/common/Log.h>
#include <ace/engine/Master.h>
#include <ace/model/Model.h>
#include <tclap/CmdLine.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

template<typename T>
using UA = TCLAP::UnlabeledMultiArg<T>;
template<typename T>
using MA = TCLAP::MultiArg<T>;
template<typename T>
using VA = TCLAP::ValueArg<T>;
using SA = TCLAP::SwitchArg;

int
main(int argc, char* argv[])
try {
  char** nargv = ace::common::Arguments::normalize(argc, argv);
  TCLAP::CmdLine cmd("Advanced Configuration Validator", ' ', ACE_VERSION);
  MA<std::string> libPath("I", "include", "Model include path", false, "string",
                          cmd);
  VA<std::string> dumpPath("D", "dump", "Resolved configuration file", false,
                           "", "string", cmd);
  SA optArg("o", "show-optional", "Show undefined optional values", cmd);
  SA unexArg("x", "show-unexpected", "Show unexpected values (garbage)", cmd);
  SA verbArg("v", "verbose", "Show summary", cmd);
  SA stctArg("s", "strict", "Strict mode", cmd);
  VA<std::string> cfgPath("c", "config", "Configuration file", false, "",
                          "string", cmd);
  UA<std::string> mdlPath("models", "Model files", true, "string", cmd);
  cmd.parse(argc, nargv);

  // Update parameters

  for (auto& p : libPath.getValue()) {
    MASTER.addModelDirectory(ace::fs::Path(p, true));
  }

  // Load the models

  std::vector<ace::model::Model::Ref> models(mdlPath.getValue().size());
  ace::model::Model::Ref mdl;
  for (auto& m : mdlPath.getValue()) {
    mdl = ace::model::Model::load(m);
    if (mdl == nullptr) {
      return -1;
    }
    models.push_back(mdl);
  }
  ACE_LOG(Warning, "*** Models are VALID ***");

  // validate the configuration file

  if (cfgPath.isSet()) {
    ace::tree::Value::Ref svr;
    std::string cfgName = cfgPath.getValue();
    if (!MASTER.hasScannerByExtension(cfgName)) {
      ACE_LOG(Error, "Unsupported configuration file format: ", cfgName);
      return -1;
    }
    svr = mdl->validate(cfgName, argc, argv);
    if (svr.get() == nullptr) {
      ACE_LOG(Error, "Invalid configuration file \"" + cfgName + "\"");
      return -1;
    }
    if (stctArg.isSet() && !MASTER.unexpected().empty()) {
      ACE_LOG(Error, "Strict checks failed");
      return -1;
    }

    int filters = ace::engine::Master::Option::Relevant;
    if (optArg.isSet()) {
      filters |= ace::engine::Master::Option::Undefined;
    }
    if (unexArg.isSet()) {
      filters |= ace::engine::Master::Option::Unexpected;
    }

    if (verbArg.isSet()) {
      MASTER.summarize(std::cout, filters);
    }
    ACE_LOG(Warning, "*** Configuration file is VALID ***");

    if (dumpPath.isSet()) {
      std::string dn = dumpPath.getValue();
      if (!MASTER.hasScannerByExtension(dn)) {
        ACE_LOG(Error, "Unsupported configuration file format: ", dn);
        return -1;
      }
      std::ofstream odf;
      odf.open(dn);
      if (odf.fail()) {
        ACE_LOG(Error, "Cannot open \"", dn, "\" for writing");
        return -1;
      }
      MASTER.scannerByExtension(dn).dump(
        *svr, ace::tree::Scanner::Format::Default, odf);
      odf.close();
    }
  }

  // Free the normalized arguments resource

  for (int i = 0; i < argc; i += 1) {
    free(nargv[i]);
  }
  free(nargv);

  return 0;
} catch (TCLAP::ArgException const& e) {
  ACE_LOG(Error, e.error(), " for argument ", e.argId());
  return -1;
} catch (std::invalid_argument const& e) {
  ACE_LOG(Error, "Invalid argument: ", e.what());
  return -1;
} catch (std::runtime_error const& e) {
  ACE_LOG(Error, "Runtime error: ", e.what());
  return -1;
}
