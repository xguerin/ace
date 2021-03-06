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

#include "Attribute.h"
#include <ace/model/Hook.h>

namespace ace { namespace model {

class HookAttribute : public Attribute
{
public:
  HookAttribute() = delete;
  HookAttribute(std::string const& n, bool o);

  // Object

  bool checkModel(tree::Value const& t) const;
  void loadModel(tree::Value const& t);

  // Instance

  bool checkInstance(tree::Object const& r, tree::Value const& v) const;
  bool flattenInstance(tree::Object& r, tree::Value& v);
  bool resolveInstance(tree::Object const& r, tree::Value const& v) const;

  void load(Attribute const& a);

  // Attribute

  bool merge(Attribute const& b);

  operator tree::Checker::Pattern() const;
  operator std::string() const;

  Attribute::Ref clone() const;

  // Accessors

  Hook const& hook() const;

private:
  Hook m_hook;
};

}}
