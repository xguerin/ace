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

#include <ace/common/Log.h>
#include <ace/engine/Master.h>
#include <ace/tree/Utils.h>
#include <ace/tree/Value.h>
#include <memory>
#include <string>
#include <vector>

namespace ace {
namespace model {
namespace Helper {

using StrAlloc = std::allocator<std::string>;

/**
 * @name Parsing configuration
 * @{ */

/**
 * @internal
 * @brief Validate inlined model
 *
 * @param mp the model path
 * @param ver the model version
 * @param stct strict mode
 * @param svr the result tree hierarchy
 *
 * @return
 */
bool
validate(std::string const & mp, std::string const & ver, const bool stct, tree::Value::Ref & svr);

/**
 * @brief Validate a configuration file for a model
 *
 * @tparam T the model to parse into
 * @param file the file path
 * @param stms alterations statements
 * @param argc the number of argument to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return True in case of success, false otherwise
 */
template<typename T, template <class, class> class C>
bool
validateFile(std::string const & file,
             C<std::string, StrAlloc> const & stms = C<std::string, StrAlloc>(),
             const bool strict = false, int argc = 0, char ** argv = nullptr) {
  if (not MASTER.hasScannerByExtension(file)) {
    ACE_LOG(Error, "Unsupported configuration file format: ", file);
    return false;
  }
  tree::Value::Ref svr = MASTER.scannerByExtension(file).open(file, argc, argv);
  if (svr == nullptr) {
    ACE_LOG(Error, "Cannot open configuration file \"" + file + "\"");
    return false;
  }
  if (not tree::utils::apply(*svr, stms)) {
    ACE_LOG(Error, "Alteration of instance failed");
    return false;
  }
  return validate(T::PATH, T::VERSION, strict, svr);
}

/**
 * @brief Parse a configuration file into a model
 *
 * @tparam T the model to parse into
 * @param file the file path
 * @param stms alterations statements
 * @param argc the number of argument to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return A valid reference in case of success, nullptr otherwise
 */
template<typename T, template <class, class> class C>
typename T::Ref
parseFile(std::string const & file,
          C<std::string, StrAlloc> const & stms = C<std::string, StrAlloc>(),
          const bool strict = false, int argc = 0, char ** argv = nullptr) {
  if (not MASTER.hasScannerByExtension(file)) {
    ACE_LOG(Error, "Unsupported configuration file format: ", file);
    return nullptr;
  }
  tree::Value::Ref svr = MASTER.scannerByExtension(file).open(file, argc, argv);
  if (svr == nullptr) {
    ACE_LOG(Error, "Cannot open configuration file \"" + file + "\"");
    return nullptr;
  }
  if (not tree::utils::apply(*svr, stms)) {
    ACE_LOG(Error, "Alteration of instance failed");
    return nullptr;
  }
  if (validate(T::PATH, T::VERSION, strict, svr)) return T::build(*svr);
  return nullptr;
}

/**
 * @brief Validate a configuration file for a model
 *
 * @tparam T the model to parse into
 * @param file the file path
 * @param argc the number of argument to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return True in case of success, false otherwise
 */
template<typename T>
bool
validateFile(std::string const & file, const bool strict = false,
             int argc = 0, char ** argv = nullptr) {
  return validateFile<T>(file, std::vector<std::string>(), strict, argc, argv);
}

/**
 * @brief Parse a configuration file into a model
 *
 * @tparam T the model to parse into
 * @param file the file path
 * @param argc the number of argument to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return A valid reference in case of success, nullptr otherwise
 */
template<typename T>
typename T::Ref
parseFile(std::string const & file, const bool strict = false,
          int argc = 0, char ** argv = nullptr) {
  return parseFile<T>(file, std::vector<std::string>(), strict, argc, argv);
}

/**
 * @brief Validate configuration files for a model
 *
 * @tparam T the model to parse into
 * @param files the file paths
 * @param stms alterations statements
 * @param argc the number of argument to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return True in case of success, false otherwise
 */
template<typename T, template<class, class> class C, template<class, class> class D>
bool
validateFiles(C<std::string, StrAlloc> const & files,
              D<std::string, StrAlloc> const & stms = D<std::string, StrAlloc>(),
              const bool strict = false, int argc = 0, char ** argv = nullptr) {
  tree::Value::Ref res = nullptr;
  for (auto & file : files) {
    if (not MASTER.hasScannerByExtension(file)) {
      ACE_LOG(Error, "Unsupported configuration file format: ", file);
      return false;
    }
    tree::Value::Ref svr = MASTER.scannerByExtension(file).open(file, argc, argv);
    if (svr == nullptr) {
      ACE_LOG(Error, "Cannot open configuration file \"" + file + "\"");
      return false;
    }
    if (res == nullptr) {
      res = svr;
    } else {
      res->merge(*svr);
    }
  }
  if (not tree::utils::apply(*res, stms)) {
    ACE_LOG(Error, "Alteration of instance failed");
    return false;
  }
  return validate(T::PATH, T::VERSION, strict, res);
}

/**
 * @brief Parse configuration files into a model
 *
 * @tparam T the model to parse into
 * @param files the file paths
 * @param stms alterations statements
 * @param argc the number of argument to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return A valid reference in case of success, nullptr otherwise
 */
template<typename T, template<class, class> class C, template<class, class> class D>
typename T::Ref
parseFiles(C<std::string, StrAlloc> const & files,
           D<std::string, StrAlloc> const & stms = D<std::string, StrAlloc>(),
           const bool strict = false, int argc = 0, char ** argv = nullptr) {
  tree::Value::Ref res = nullptr;
  for (auto & file : files) {
    if (not MASTER.hasScannerByExtension(file)) {
      ACE_LOG(Error, "Unsupported configuration file format: ", file);
      return nullptr;
    }
    tree::Value::Ref svr = MASTER.scannerByExtension(file).open(file, argc, argv);
    if (svr == nullptr) {
      ACE_LOG(Error, "Cannot open configuration file \"" + file + "\"");
      return nullptr;
    }
    if (res == nullptr) {
      res = svr;
    } else {
      res->merge(*svr);
    }
  }
  if (not tree::utils::apply(*res, stms)) {
    ACE_LOG(Error, "Alteration of instance failed");
    return nullptr;
  }
  if (validate(T::PATH, T::VERSION, strict, res)) return T::build(*res);
  return nullptr;
}

/**
 * @brief Validate configuration files for a model
 *
 * @tparam T the model to parse into
 * @param files the file paths
 * @param argc the number of argument to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return True in case of success, false otherwise
 */
template<typename T, template<class, class> class C>
bool
validateFiles(C<std::string, StrAlloc> const & files,
              const bool strict = false, int argc = 0, char ** argv = nullptr) {
  return validateFiles<T>(files, std::vector<std::string>(), strict, argc, argv);
}

/**
 * @brief Parse configuration files into a model
 *
 * @tparam T the model to parse into
 * @param files the file paths
 * @param argc the number of argument to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return A valid reference in case of success, nullptr otherwise
 */
template<typename T, template<class, class> class C>
typename T::Ref
parseFiles(C<std::string, StrAlloc> const & files,
           const bool strict = false, int argc = 0, char ** argv = nullptr) {
  return parseFiles<T>(files, std::vector<std::string>(), strict, argc, argv);
}

/**
 * @brief Validate a configuration string for a model
 *
 * @tparam T the model to parse into
 * @param str  the configuration string
 * @param fmt  the format of the input
 * @param stms alterations statements
 * @param argc the number of argument to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return True in case of success, false otherwise
 */
template<typename T, template <class, class> class C>
bool
validateString(std::string const & str, std::string const & fmt,
               C<std::string, StrAlloc> const & stms = std::vector<std::string>(),
               const bool strict = false, int argc = 0, char ** argv = nullptr) {
  if (not MASTER.hasScannerByName(fmt)) {
    ACE_LOG(Error, "Unsupported configuration file format: ", fmt);
    return false;
  }
  tree::Value::Ref svr = MASTER.scannerByName(fmt).parse(str, argc, argv);
  if (svr.get() == nullptr) {
    ACE_LOG(Error, "Cannot parse inline configuration");
    return false;
  }
  if (not tree::utils::apply(*svr, stms)) {
    ACE_LOG(Error, "Alteration of inline configuration failed");
    return false;
  }
  validate(T::PATH, T::VERSION, strict, svr);
}

/**
 * @brief Parse a configuration string into a model
 *
 * @tparam T the model to parse into
 * @param str  the configuration string
 * @param fmt  the format of the input
 * @param stms alterations statements
 * @param argc the number of argument to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return A valid reference in case of success, nullptr otherwise
 */
template<typename T, template <class, class> class C>
typename T::Ref
parseString(std::string const & str, std::string const & fmt,
            C<std::string, StrAlloc> const & stms = std::vector<std::string>(),
            const bool strict = false, int argc = 0, char ** argv = nullptr) {
  if (not MASTER.hasScannerByName(fmt)) {
    ACE_LOG(Error, "Unsupported configuration file format: ", fmt);
    return nullptr;
  }
  tree::Value::Ref svr = MASTER.scannerByName(fmt).parse(str, argc, argv);
  if (svr.get() == nullptr) {
    ACE_LOG(Error, "Cannot parse inline configuration");
    return nullptr;
  }
  if (not tree::utils::apply(*svr, stms)) {
    ACE_LOG(Error, "Alteration of inline configuration failed");
    return nullptr;
  }
  if (validate(T::PATH, T::VERSION, strict, svr)) return T::build(*svr);
  return nullptr;
}

/**
 * @brief Validate a configuration string for a model
 *
 * @tparam T the model to parse into
 * @param str  the configuration string
 * @param fmt  the format of the input
 * @param argc the number of argument to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return True in case of success, false otherwise
 */
template<typename T>
bool
validateString(std::string const & str, std::string const & fmt,
               const bool strict = false, int argc = 0, char ** argv = nullptr) {
  return parseString<T>(str, fmt, std::vector<std::string>(), strict, argc, argv);
}

/**
 * @brief Parse a configuration string into a model
 *
 * @tparam T the model to parse into
 * @param str  the configuration string
 * @param fmt  the format of the input
 * @param argc the number of argument to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return A valid reference in case of success, nullptr otherwise
 */
template<typename T>
typename T::Ref
parseString(std::string const & str, std::string const & fmt,
            const bool strict = false, int argc = 0, char ** argv = nullptr) {
  return parseString<T>(str, fmt, std::vector<std::string>(), strict, argc, argv);
}

/**  @} */

/**
 * @name Explaining configurations
 * @{ */

/**
 * @internal
 * @brief Print a model as a tree
 *
 * @param mp the internal model path
 * @param ver the model version
 *
 * @return true if the operation succeeded, false otherwise
 */
bool printAsTree(std::string const & mp, std::string const & ver);

/**
 * @brief Print a model as a tree
 *
 * @tparam T the model class to print
 *
 * @return true if the operation succeeded, false otherwise
 */
template<typename T>
bool
printAsTree() {
  return printAsTree(T::PATH, T::VERSION);
}

/**
 * @internal
 * @brief Explain a path within a model
 *
 * @param mp the internal model path
 * @param ver the model version
 * @param cp the path within the model
 *
 * @return true if the path is valid, false otherwise
 */
bool explain(std::string const & mp, std::string const & ver, std::string const & cp);

/**
 * @brief Explain a path within a model
 *
 * @tparam T the model class to explain
 * @param path the path within the model
 *
 * @return true if the path is valid, false otherwise
 */
template<typename T>
bool
explain(std::string const & path) {
  return explain(T::PATH, T::VERSION, path);
}

/**  @} */

/**
 * @name Dumping configuration
 * @{ */

/**
 * @brief Dump a fully resolved configuration from a single file
 *
 * @tparam T the model class to dump
 * @param from the original configuration file
 * @param fmt the format of the output
 * @param to the target stream
 * @param stms alterations statements
 * @param argc the number of arguments to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return
 */
template<typename T, template <class, class> class C>
bool
dumpFile(std::string const & file, std::string const & fmt, std::ostream & to,
         C<std::string, StrAlloc> const & stms = C<std::string, StrAlloc>(),
         int argc = 0, char ** argv = nullptr) {
  if (not MASTER.hasScannerByName(fmt)) {
    ACE_LOG(Error, "Unsupported configuration file format: ", fmt);
    return false;
  }
  tree::Value::Ref svr = parseFiles<T>(file, stms, false, argc, argv);
  if (svr == nullptr) return false;
  MASTER.scannerByName(fmt).dump(*svr, tree::Scanner::Format::Default, to);
  return true;
}

/**
 * @brief Dump a fully resolved configuration from a single file
 *
 * @tparam T the model class to dump
 * @param from the original configuration file
 * @param fmt the format of the output
 * @param to the target stream
 * @param argc the number of arguments to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return
 */
template<typename T>
bool
dumpFile(std::string const & file, std::string const & fmt, std::ostream & to,
         int argc = 0, char ** argv = nullptr) {
  return dumpFile<T>(file, fmt, to, std::vector<std::string>(), argc, argv);
}

/**
 * @brief Dump a fully resolved configuration from multiple files
 *
 * @tparam T the model class to dump
 * @param from the original configuration files
 * @param fmt the format of the output
 * @param to the target stream
 * @param stms alterations statements
 * @param argc the number of arguments to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return
 */
template<typename T, template<class, class> class C, template<class, class> class D>
bool
dumpFiles(C<std::string, StrAlloc> const & files, std::string const & fmt, std::ostream & to,
          D<std::string, StrAlloc> const & stms = D<std::string, StrAlloc>(),
          int argc = 0, char ** argv = nullptr) {
  if (not MASTER.hasScannerByName(fmt)) {
    ACE_LOG(Error, "Unsupported configuration file format: ", fmt);
    return false;
  }
  tree::Value::Ref svr = parseFiles<T>(files, stms, false, argc, argv);
  if (svr == nullptr) return false;
  MASTER.scannerByName(fmt).dump(*svr, tree::Scanner::Format::Default, to);
  return true;
}

/**
 * @brief Dump a fully resolved configuration from multiple files
 *
 * @tparam T the model class to dump
 * @param from the original configuration files
 * @param fmt the format of the output
 * @param to the target stream
 * @param argc the number of arguments to pass to the underlying engine
 * @param argv the arguments to pass to the underlying engine
 *
 * @return
 */
template<typename T, template<class, class> class C>
bool
dumpFiles(C<std::string, StrAlloc> const & files, std::string const & fmt, std::ostream & to,
          int argc = 0, char ** argv = nullptr) {
  return dumpFiles<T>(files, fmt, to, std::vector<std::string>(), argc, argv);
}

/**  @} */

/**
 * @brief Get the version string of the library
 *
 * @return the version string of the library
 */
std::string version();

/**  @} */

} // namespace Helper
} // namespace model
} // namespace ace
