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

#include <ostream>
#include <set>
#include <string>

namespace ace {
namespace model {

// Generator class

class Generator {
 public:

  Generator() = default;
  explicit Generator(Generator const &) = default;
  virtual ~Generator() { }

  virtual void collectModelFileDependencies(std::set<std::string> & d) const = 0;

  virtual void collectInterfaceIncludes(std::set<std::string> & i) const = 0;
  virtual void collectImplementationIncludes(std::set<std::string> & i) const = 0;

  virtual bool hasPrivateNamespaceDefinition() const = 0;
  virtual void doPrivateNamespaceDefinition(std::ostream & o, int l) const = 0;

  virtual bool hasTypeDeclaration() const = 0;
  virtual void doTypeDeclaration(std::ostream & o, int l) const = 0;
  virtual void doTypeDefinition(std::ostream & o, int l) const = 0;

  virtual void doBuildDefinition(std::string const & e, std::ostream & o, int l) const = 0;
  virtual void doBuildDefinition(std::string const & s, std::string const & v,
                                 std::string const & e, std::ostream & o, int l) const = 0;

  /**
   * @brief Generate the serializer definition
   *
   * @param c the name receiving container
   * @param n the name of the option
   * @param o the output stream
   * @param l the indentation level
   */
  virtual void doSerializerDefinition(std::string const & c, std::string const & n,
                                      std::ostream & o, int l) const = 0;

  /**
   * @brief Generate the serializer definition
   *
   * @param c the name receiving container
   * @param n the name of the option
   * @param v the value of the option
   * @param b bypass the presence check
   * @param o the output stream
   * @param l the indentation level
   */
  virtual void doSerializerDefinition(std::string const & c, std::string const & n,
                                      std::string const & v, const bool b,
                                      std::ostream & o, int l) const = 0;

  virtual void doCheckerInterface(std::ostream & o, int l) const = 0;
  virtual void doCheckerDeclaration(std::ostream & o, int l) const = 0;
  virtual void doCheckerDefinition(std::ostream & o, int l) const = 0;

  virtual void doGetterInterface(std::ostream & o, int l) const = 0;
  virtual void doGetterDeclaration(std::ostream & o, int l) const = 0;
  virtual void doGetterDefinition(std::ostream & o, int l) const = 0;

  template<typename T> static std::string nameFor();
  static std::ostream & indent(std::ostream & o, int level);

 protected:

  static std::string tempName();

 private:

  static size_t s_tmpId;
};

template<typename T>
std::string
Generator::nameFor() {
  return "";
}

template<>
std::string
Generator::nameFor<bool>();

template<>
std::string
Generator::nameFor<int>();

template<>
std::string
Generator::nameFor<long>();

template<>
std::string
Generator::nameFor<float>();

template<>
std::string
Generator::nameFor<double>();

template<>
std::string
Generator::nameFor<std::string>();

} // namespace model
} // namespace ace
