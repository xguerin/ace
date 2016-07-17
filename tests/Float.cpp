/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation floats (the "Software"), to deal in the Software
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

class Float : public ::testing::Test {
 public:
  static void SetUpTestCase() {
    MASTER.reset();
    ace::common::Path incPath = ace::fs::Directory().path() / ace::common::Path("float/");
    MASTER.addModelDirectory(incPath);
  }

 protected:
  static const char * prgnam;
};

const char * Float::prgnam = "tests";

TEST_F(Float, Fail_BadConstrainedDefault) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadConstrainedDefault.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Float, Fail_BadDefault) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadDefault.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Float, Fail_BadEither) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadEither.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Float, Fail_BadRange) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadRange.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Float, Fail_EitherAndRange) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_EitherAndRange.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Float, Pass_Ok) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Float, Pass_Ok_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("float/01_Ok.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Float, Fail_Ok_BadArity) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("float/01_BadArity.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Float, Fail_Ok_BadEither) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("float/01_BadEither.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Float, Fail_Ok_BadRange) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("float/01_BadRange.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Float, Fail_Ok_BadValue) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("float/01_BadValue.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}
