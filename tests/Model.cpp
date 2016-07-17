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

class Model : public ::testing::Test {
 public:
  static void SetUpTestCase() {
    MASTER.reset();
    ace::common::Path incPath = ace::fs::Directory().path() / ace::common::Path("model/");
    MASTER.addModelDirectory(incPath);
  }

 protected:
  static const char * prgnam;
};

const char * Model::prgnam = "tests";

TEST_F(Model, Fail_BadAuthor) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadAuthor.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Model, Fail_BadContent) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadContent.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Model, Fail_BadDoc) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadDoc.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Model, Fail_BadFormat) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadFormat.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Model, Fail_BadInclude) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadInclude.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Model, Fail_BadInheritanceMultipleRequiredToOptional) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadInheritanceMultipleRequiredToOptional.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Model, Fail_BadInheritanceSingleRequiredToOptional) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadInheritanceSingleRequiredToOptional.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Model, Fail_BadPackage) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadPackage.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Model, Fail_BadVarName) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadVarName.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Model, Fail_EmptyInclude) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_EmptyInclude.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Model, Fail_IncludeCircularReference) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_IncludeCircularReference.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Model, Fail_IncludeConflict) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_IncludeConflict.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Model, Fail_TypeCircularReference) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_TypeCircularReference.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Model, Pass_Ok) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Model, Pass_Ok_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("model/01_Ok.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

