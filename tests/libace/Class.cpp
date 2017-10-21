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
#include <ace/engine/Master.h>
#include <ace/model/Model.h>
#include <ace/tree/Value.h>
#include <vector>

class Class : public ::testing::Test {
 public:
  static void SetUpTestCase() {
    MASTER.reset();
    ace::fs::Path incPath = ace::fs::Directory().path() / ace::fs::Path("class/");
    MASTER.addModelDirectory(incPath);
    incPath = ace::fs::Directory().path() / ace::fs::Path("includes/");
    MASTER.addModelDirectory(incPath);
  }

 protected:
  static const char * prgnam;
};

const char * Class::prgnam = "tests";

TEST_F(Class, Fail_BadDefault) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadDefault.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Class, Fail_InvalidModel) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_InvalidModel.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Class, Fail_MissingModel) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_MissingModel.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Class, Pass_Ok) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Class, Pass_Ok_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("class/01_Ok.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Class, Pass_Defaulted_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_Ok.json");
  auto svr = res->validate("class/02_Ok.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
  std::vector<ace::tree::Value::Ref> values;
  auto path0 = ace::tree::Path::parse("$.var0.var0");
  ASSERT_TRUE(svr->has(path0));
  svr->get(path0, values);
  ASSERT_EQ(values.size(), 1);
  ASSERT_EQ(values[0]->type(), ace::tree::Value::Type::Float);
  auto const & val0 = dynamic_cast<ace::tree::Primitive const &>(*values[0]);
  ASSERT_EQ(val0.value<double>(), -1);
  values.clear();
  auto path1 = ace::tree::Path::parse("$.var1.var0");
  ASSERT_TRUE(svr->has(path1));
  svr->get(path1, values);
  ASSERT_EQ(values.size(), 1);
  ASSERT_EQ(values[0]->type(), ace::tree::Value::Type::Float);
  auto const & val1 = dynamic_cast<ace::tree::Primitive const &>(*values[0]);
  ASSERT_EQ(val1.value<double>(), -1);
}

