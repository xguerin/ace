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

class Arity : public ::testing::Test {
 public:
  static void SetUpTestCase() {
    MASTER.reset();
    ace::fs::Path incPath = ace::fs::Directory().path() / ace::fs::Path("arity/");
    MASTER.addModelDirectory(incPath);
  }

 protected:
  static const char * prgnam;
};

const char * Arity::prgnam = "tests";

TEST_F(Arity, Fail_BadArity) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadArity.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Arity, Fail_BadRange) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadRange.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Arity, Fail_NegativeArityLeft) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_NegativeArityLeft.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Arity, Fail_NegativeArityRight) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_NegativeArityRight.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Arity, Pass_Ok) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Arity, Pass_Ok_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("arity/01_All.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Arity, Pass_LeftOpenRange_Empty) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_LeftOpenRange.json");
  auto svr = res->validate("arity/02_Empty.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Arity, Pass_LeftOpenRange_Small) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_LeftOpenRange.json");
  auto svr = res->validate("arity/02_Small.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Arity, Fail_LeftOpenRange_Large) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_LeftOpenRange.json");
  auto svr = res->validate("arity/02_Large.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Arity, Fail_RightOpenRange_Empty) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_RightOpenRange.json");
  auto svr = res->validate("arity/02_Empty.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Arity, Fail_RightOpenRange_Small) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_RightOpenRange.json");
  auto svr = res->validate("arity/02_Small.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Arity, Pass_RightOpenRange_Large) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_RightOpenRange.json");
  auto svr = res->validate("arity/02_Large.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

