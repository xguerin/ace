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

#include <Python.h>
#include "Scanner.h"
#include "Common.h"
#include "Object.h"
#include <ace/common/Log.h>
#include <ace/common/Path.h>
#include <ace/common/String.h>
#include <ace/engine/Master.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace ace {
namespace pyfmt {

tree::Value::Ref
Scanner::open(std::string const & fn, int argc, char ** argv) {
  if (argc == 0 or argv == nullptr) {
    ACE_LOG(Error, "invalid ARGC/ARGV arguments");
    return nullptr;
  }

  Py_SetProgramName(const_cast<char *>(fn.c_str()));
  Py_InitializeEx(0);
  shift(fn, argc, argv);
  PySys_SetArgv(argc, argv);

  common::Path fnPath(fn), container = fnPath.prune();
  std::string path = container.toString();
  if (path.empty()) path = ".";
  path = path + ":" + std::string(Py_GetPath());
  PySys_SetPath(const_cast<char *>(path.c_str()));

  std::vector<std::string> fnParts;
  common::String::split(*fnPath.rbegin(), '.', fnParts);
  fnParts.erase(--fnParts.end());
  std::string mn = common::String::join(fnParts, '.');

  PyObject * pName = PyString_FromString(mn.c_str());
  PyObject * pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  PyObject * pConfig = nullptr;
  tree::Value::Ref obj;
  if (pModule == nullptr) {
    PyErr_Print();
    goto bad_file;
  }
  if (not PyObject_HasAttrString(pModule, "config")) {
    ACE_LOG(Error, "no \"config\" dictionary found in \"", fn, "\"");
    goto bad_config;
  }
  pConfig = PyObject_GetAttrString(pModule, "config");
  if (not PyDict_Check(pConfig)) {
    ACE_LOG(Error, "\"config\" is not a dictionary");
    goto bad_type;
  }
  obj = Object::build("", pConfig);

 bad_type:
  Py_DECREF(pConfig);
 bad_config:
  Py_XDECREF(pModule);
 bad_file:
  Py_Finalize();
  return obj;
}

tree::Value::Ref
Scanner::parse(std::string const & s, int argc, char ** argv) {
  if (argc == 0 or argv == nullptr) {
    ACE_LOG(Error, "invalid ARGC/ARGV arguments");
    return nullptr;
  }

  Py_SetProgramName(const_cast<char *>(""));
  Py_InitializeEx(0);
  shift("", argc, argv);
  PySys_SetArgv(argc, argv);

  PyObject * pModule = PyModule_New("inlined");
  PyModule_AddStringConstant(pModule, "__file__", "");
  PyObject * pGlobal = PyDict_New();

  if (PyDict_GetItemString(pGlobal, "__builtins__") == nullptr) {
    if (PyDict_SetItemString(pGlobal, "__builtins__", PyEval_GetBuiltins()) != 0) {
      ACE_LOG(Error, "cannot merge Python built-ins");
      return nullptr;
    }
  }

  PyObject * pLocal = PyModule_GetDict(pModule);
  PyObject * pValue = PyRun_String(s.c_str(), Py_file_input, pGlobal, pLocal);

  PyObject * pConfig = nullptr;
  tree::Value::Ref obj;
  if (pValue == nullptr) {
    PyErr_Print();
    goto bad_data;
  }
  Py_DECREF(pValue);
  if (not PyObject_HasAttrString(pModule, "config")) {
    ACE_LOG(Error, "no \"config\" dictionary found in inlined Python");
    goto bad_config;
  }
  pConfig = PyObject_GetAttrString(pModule, "config");
  if (not PyDict_Check(pConfig)) {
    ACE_LOG(Error, "\"config\" is not a dictionary");
    goto bad_type;
  }
  obj = Object::build("", pConfig);

 bad_type:
  Py_DECREF(pConfig);
 bad_config:
  Py_XDECREF(pModule);
 bad_data:
  Py_Finalize();
  return obj;
}

void
Scanner::dump(tree::Value const & v, const Format f, std::ostream & o) const {
  o << "config = ";
  dump_value(v, o, 2, true);
  o << std::endl;
}

std::string
Scanner::name() const {
  return "python";
}

std::string
Scanner::extension() const {
  return "py";
}

} // namespace pyfmt
} // namespace ace

extern "C" {
void * loadPlugin() {
  return new ace::pyfmt::Scanner();
}
}
