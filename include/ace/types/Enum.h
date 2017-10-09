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
#include <ace/model/MapAttribute.h>
#include <functional>
#include <set>
#include <string>

namespace ace {
namespace model {

class Enum : public EnumeratedType<std::string, true> {
 public:

  using BindAttributeType = MapAttribute<int>;

  Enum();

  void collectInterfaceIncludes(std::set<std::string> & i) const;
  void collectImplementationIncludes(std::set<std::string> & i) const;

  bool hasPrivateNamespaceDefinition() const;
  void doPrivateNamespaceDefinition(std::ostream & o, int l) const;

  bool hasTypeDeclaration() const;
  void doTypeDeclaration(std::ostream & o, int l) const;

  using EnumeratedType::doBuildDefinition;
  void doBuildDefinition(std::string const & s, std::string const & v, std::string const & e,
                         std::ostream & o, int l) const;

  void doGetterInterface(std::ostream & o, int level = 0) const;
  void doGetterDeclaration(std::ostream & o, int level = 0) const;
  void doGetterDefinition(std::ostream & o, int level = 0) const;

  BasicType::Ref clone(std::string const & n) const;
  std::string typeName() const;

  BindAttributeType & bindAttribute();
  BindAttributeType const & bindAttribute() const;
};

} // namespace model
} // namespace ace
