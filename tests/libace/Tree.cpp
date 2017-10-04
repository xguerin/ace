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

#include "Common.h"
#include <ace/tree/Array.h>
#include <ace/tree/Object.h>
#include <ace/tree/Primitive.h>
#include <ace/tree/Value.h>
#include <string>
#include <vector>

class Tree : public ::testing::Test {
 protected:
  static size_t
  get(ace::tree::Value::Ref const & v, std::string const & p) {
    std::vector<ace::tree::Value::Ref> result;
    v->get(ace::tree::Path::parse(p), result);
    return result.size();
  }
};

TEST_F(Tree, Path) {
  auto root = ace::tree::Object::build("");
  auto ini = ace::tree::Object::build("var0");
  ini->put(ace::tree::Primitive::build("var0", 3.14));
  ini->put(ace::tree::Primitive::build("var1", "hello"));
  root->put(ini);
  auto array = ace::tree::Array::build("var1");
  for (int i = 0; i < 4; i += 1) {
    auto ini = ace::tree::Object::build("");
    ini->put(ace::tree::Primitive::build("var0", 3.14));
    ini->put(ace::tree::Primitive::build("var1", "hello"));
    array->push_back(ini);
  }
  root->put(array);

  // Basic accessors

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var0")));
  ASSERT_EQ(get(root, "$.var0"), 1);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var1")));
  ASSERT_EQ(get(root, "$.var1"), 1);

  ASSERT_FALSE(root->has(ace::tree::Path::parse("$.var2")));
  ASSERT_EQ(get(root, "$.var2"), 0);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var0.var0")));
  ASSERT_EQ(get(root, "$.var0.var0"), 1);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var0.var1")));
  ASSERT_EQ(get(root, "$.var0.var1"), 1);

  ASSERT_FALSE(root->has(ace::tree::Path::parse("$.var0.var2")));
  ASSERT_EQ(get(root, "$.var0.var2"), 0);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var0.*")));
  ASSERT_EQ(get(root, "$.var0.*"), 2);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var1[0]")));
  ASSERT_EQ(get(root, "$.var1[0]"), 1);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var1[1]")));
  ASSERT_EQ(get(root, "$.var1[1]"), 1);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var1[2]")));
  ASSERT_EQ(get(root, "$.var1[2]"), 1);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var1[3]")));
  ASSERT_EQ(get(root, "$.var1[3]"), 1);

  ASSERT_FALSE(root->has(ace::tree::Path::parse("$.var1[4]")));
  ASSERT_EQ(get(root, "$.var1[4]"), 0);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var1[0].var0")));
  ASSERT_EQ(get(root, "$.var1[0].var0"), 1);

  ASSERT_FALSE(root->has(ace::tree::Path::parse("$.var1.var0")));
  ASSERT_EQ(get(root, "$.var1.var0"), 0);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var1[*]")));
  ASSERT_EQ(get(root, "$.var1[*]"), 4);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var1.*")));
  ASSERT_EQ(get(root, "$.var1.*"), 4);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var1[0,1,2,3]")));
  ASSERT_EQ(get(root, "$.var1[0,1,2,3]"), 4);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var1[0,1,2,3,4]")));
  ASSERT_EQ(get(root, "$.var1[0,1,2,3,4]"), 4);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var1[0:4:1]")));
  ASSERT_EQ(get(root, "$.var1[0:4:1]"), 4);

  ASSERT_FALSE(root->has(ace::tree::Path::parse("$.var1[0:5:1]")));
  ASSERT_EQ(get(root, "$.var1[0:5:1]"), 0);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$.var1[0:3:2]")));
  ASSERT_EQ(get(root, "$.var1[0:3:2]"), 2);

  // Recursive accessors

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$..var0")));
  ASSERT_EQ(get(root, "$..var0"), 6);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$..var1")));
  ASSERT_EQ(get(root, "$..var1"), 6);

  ASSERT_FALSE(root->has(ace::tree::Path::parse("$..var2")));
  ASSERT_EQ(get(root, "$..var2"), 0);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$..var0.var0")));
  ASSERT_EQ(get(root, "$..var0.var0"), 1);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$..var0.var1")));
  ASSERT_EQ(get(root, "$..var0.var1"), 1);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$..var1..var0")));
  ASSERT_EQ(get(root, "$..var1..var0"), 4);

  ASSERT_TRUE(root->has(ace::tree::Path::parse("$..var1..var1")));
  ASSERT_EQ(get(root, "$..var1..var1"), 4);
}

TEST_F(Tree, PrimitiveCharArrayAsString) {
  auto ref = ace::tree::Primitive::build("hello", "hello");
  ASSERT_TRUE(ref->is<std::string>());
}

TEST_F(Tree, PrimitiveConstCharPointerAsString) {
  const char * v = "hello";
  auto ref = ace::tree::Primitive::build("hello", v);
  ASSERT_TRUE(ref->is<std::string>());
}

TEST_F(Tree, PrimitiveCharPointerAsString) {
  char * v = const_cast<char *>("hello");
  auto ref = ace::tree::Primitive::build("hello", v);
  ASSERT_TRUE(ref->is<std::string>());
}

TEST_F(Tree, PrimitiveStringAsString) {
  auto ref = ace::tree::Primitive::build("hello", std::string("hello"));
  ASSERT_TRUE(ref->is<std::string>());
}

TEST_F(Tree, UnsignedShort) {
  uint16_t val = 10;
  auto ref = ace::tree::Primitive::build("hello", val);
  ASSERT_TRUE(ref->is<uint16_t>());
}
