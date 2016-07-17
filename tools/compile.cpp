/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <ace/common/Arguments.h>
#include <ace/common/Log.h>
#include <ace/engine/Master.h>
#include <ace/model/Model.h>
#include <tclap/CmdLine.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

template<typename T> using UA = TCLAP::UnlabeledMultiArg<T>;
template<typename T> using MA = TCLAP::MultiArg<T>;
template<typename T> using VA = TCLAP::ValueArg<T>;
using SA = TCLAP::SwitchArg;

int main(int argc, char * argv[]) try {
  char ** nargv = ace::common::Arguments::normalize(argc, argv);
  TCLAP::CmdLine cmd("Advanced Configuration Compiler", ' ', ACE_VERSION);
  MA<std::string> libA("I", "include", "Model include path", false, "string", cmd);
  VA<std::string> genA("o", "output", "Output directory", false, "", "string", cmd);
  SA              depA("D", "deps", "Generate dependencies", cmd);
  VA<std::string> dphA("F", "depfile", "Dependency file", false, "", "string", cmd);
  UA<std::string> mdlA("model", "Model file name", true, "string", cmd);
  cmd.parse(argc, nargv);
  /**
   * Tentatively update the output of the log
   */
  if (ace::common::Log::get().isFileStream()) {
    std::vector<std::string> names;
    for (auto & m : mdlA.getValue()) {
      std::vector<std::string> elems;
      ace::common::String::split(m, '/', elems);
      names.push_back(elems.rbegin()->substr(0, elems.rbegin()->find('.')));
    }
    auto target = ace::common::String::join(names, '_') + ".ac.log";
    if (genA.isSet()) target = genA.getValue() + "/" + target;
    ace::common::Log::get().changeFileStreamDestination(target);
  }
  /**
   * Output some information about the tool
   */
  ACE_LOG(Info, "Advanced Configuration Compiler - ", ACE_VERSION);
  ACE_LOG(Info, "-------------------------------");
  /**
   * Prepare dependencies
   */
  std::ofstream odf;
  if (depA.isSet() && dphA.isSet()) {
    odf.open(dphA.getValue());
    if (odf.fail()) {
      std::cerr << "Can!open \"" << dphA.getValue() << "\" for writing" << std::endl;
      return -1;
    }
  }
  std::ostream & depOut = dphA.isSet() ? odf : std::cout;
  /**
   * Compute generation path
   */
  ace::common::Path oPath(ace::fs::Directory().path());
  if (genA.isSet()) {
    oPath = ace::common::Path(genA.getValue(), true);
    if (!ace::fs::Directory(oPath).isValid()) {
      ACE_LOG(Error, "Invalid output directory \"", oPath, "\"");
      return -1;
    }
  }
  /**
   * Process the models
   */
  for (auto & m : mdlA.getValue()) {
    MASTER.reset();
    for (auto & p : libA.getValue()) {
      MASTER.addModelDirectory(ace::common::Path(p, true));
    }
    ace::model::Model::Ref mdl = ace::model::Model::load(m);
    if (mdl == nullptr) {
      ACE_LOG(Error, "Invalid model \"" + m + "\"");
      return -1;
    }
    mdl->generateInterface(oPath);
    mdl->generateImplementation(oPath);
    if (depA.isSet()) {
      std::set<std::string> deps;
      mdl->collectModelFileDependencies(deps);
      auto generator = [&depOut, &oPath, &deps, &mdl](std::string const & p) {
        depOut << (oPath / p).compress() << ":";
        ace::common::Path srcpath = MASTER.modelPathFor(mdl->filePath());
        depOut << "  \\" << std::endl;
        depOut << "  " << srcpath.compress();
        for (auto & d : deps) {
          depOut << "  \\" << std::endl;
          depOut << "  " << MASTER.modelPathFor(d).compress();
        }
        depOut << std::endl;
        depOut << std::endl;
      };
      /**
       * the following generate the dependency files in the Makefile deps format. We only
       * generate dependencies for the implementation source. If needed, dependencies
       * for the other files can be generated through:
       *
       *  generator(mdl->interfaceHeaderName());
       *  generator(mdl->implementationHeaderName());
       *
       * It should !be necessary though, as header deps are generated by the default
       * GCC/Clang header dependency generator.
       */
      generator(mdl->implementationSourceName());
    }
  }
  if (depA.isSet() && dphA.isSet()) {
    odf.close();
  }
  /**
   * Free the normalized arguments resource
   */
  for (int i = 0; i < argc; i += 1) free(nargv[i]);
  free(nargv);
  return 0;
}
catch (TCLAP::ArgException const & e) {
  std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
}
catch (std::invalid_argument const & e) {
  std::cerr << "Invalid argument: " << e.what() <<std::endl;
  return -1;
}
catch (std::runtime_error const & e) {
  std::cerr << "Runtime error: " << e.what() <<std::endl;
  return -1;
}
