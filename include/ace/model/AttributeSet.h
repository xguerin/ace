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

#ifndef ACE_MODEL_ATTRIBUTESET_H_
#define ACE_MODEL_ATTRIBUTESET_H_

#include "Attribute.h"
#include "Object.h"
#include <ace/tree/Checker.h>
#include <functional>
#include <list>
#include <map>
#include <set>
#include <string>

namespace ace {
namespace model {

// Attribute ID less comparator

struct AttributeLess : public std::binary_function<Attribute::Ref, Attribute::Ref, bool> {
  bool operator() (Attribute::Ref const & a, Attribute::Ref const & b) const;
};

// Attribute set

class AttributeSet : public Object {
 public:

  using Map = std::map<std::string, Attribute::Ref>;
  using Set = std::set<Attribute::Ref, AttributeLess>;
  using List = std::map<std::string, std::set<std::string> >;

  using iterator = Set::iterator;
  using const_iterator = Set::const_iterator;

  AttributeSet();
  explicit AttributeSet(AttributeSet const & o);

  tree::Path path() const;

  bool checkModel(tree::Value const & t) const;
  void loadModel(tree::Value const & t);
  bool flattenModel();
  bool validateModel();

  bool validate(tree::Object const & r, tree::Value const & v) const;

  // Set operation

  template<typename T> void define(std::string const & n, bool opt);
  void exclude(std::string const & a, std::string const & b);

  bool has(std::string const & n) const;
  Attribute::Ref const & operator[](std::string const & n) const;

  void activate(std::string const & n);

  bool merge(AttributeSet const & s);
  bool override(AttributeSet const & s);

  // Iterators

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  // Accessors

  tree::Checker::Schema schema() const;

 private:

  bool clearConflicts() const;

  Map   m_schema;
  Set   m_schemaSet;
  Map   m_instances;
  Set   m_instanceSet;
  List  m_xorList;
};

template<typename T>
void
AttributeSet::define(std::string const & n, bool opt) {
  Attribute::Ref ar(new T(n, opt));
  ar->setParent(this);
  m_schema[n] = ar;
  m_schemaSet.insert(ar);
}

} // namespace model
} // namespace ace

#endif  // ACE_MODEL_ATTRIBUTESET_H_
