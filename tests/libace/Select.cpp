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

class Select : public ::testing::Test {
 public:
  static void SetUpTestCase() {
    MASTER.reset();
    ace::common::Path incPath = ace::fs::Directory().path() / ace::common::Path("select/");
    MASTER.addModelDirectory(incPath);
    incPath = ace::fs::Directory().path() / ace::common::Path("includes/");
    MASTER.addModelDirectory(incPath);
  }

 protected:
  static const char * prgnam;
};

const char * Select::prgnam = "tests";

TEST_F(Select, Fail_BadTemplate) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadTemplate.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Select, Pass_Ok) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Select, Pass_Ok_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("select/01_Ok.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Select, Pass_Dependencies) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_Dependencies.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Select, Pass_Dependencies_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_Dependencies.json");
  auto svr = res->validate("select/02_Ok.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Select, Fail_Dependencies_Missing) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_Dependencies.json");
  auto svr = res->validate("select/02_Missing.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Select, Pass_Complex) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("03_Complex.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Select, Pass_Complex_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("03_Complex.json");
  auto svr = res->validate("select/03_Ok.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Select, Fail_Complex_MissingLeafObject) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("03_Complex.json");
  auto svr = res->validate("select/03_MissingLeafObject.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}
