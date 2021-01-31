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
#include <ace/filesystem/Path.h>
#include <ace/model/Model.h>
#include <tclap/CmdLine.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int
main(int argc, char* argv[])
try {
  char** nargv = ace::common::Arguments::normalize(argc, argv);
  TCLAP::CmdLine cmd("Advanced Configuration Explainer", ' ', ACE_VERSION);

  TCLAP::MultiArg<std::string> libPath("I", "include", "Model include path",
                                       false, "string");
  cmd.add(libPath);

  TCLAP::ValueArg<std::string> optPath("p", "path", "Option path", false, "/",
                                       "string");
  cmd.add(optPath);

  TCLAP::UnlabeledMultiArg<std::string> mdlPath("models", "Model file names",
                                                true, "string");
  cmd.add(mdlPath);

  cmd.parse(argc, nargv);

  // update parameters

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

  // Explain the model

  if (!optPath.isSet()) {
    mdl->display(ace::model::Coach::Branch::Root);
  } else {
    ace::tree::Path path = ace::tree::Path::parse(optPath.getValue());
    if (!mdl->explain(path)) {
      ACE_LOG(Error, "Invalid model path: ", optPath.getValue());
      return -1;
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
} catch (std::invalid_argument const& e) {
  ACE_LOG(Error, "Invalid argument: ", e.what());
  return -1;
} catch (std::runtime_error const& e) {
  ACE_LOG(Error, "Runtime error: ", e.what());
  return -1;
}
