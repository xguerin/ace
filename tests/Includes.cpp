/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation hooks (the "Software"), to deal in the Software
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

class Includes : public ::testing::Test {
 public:
  static void SetUpTestCase() {
    MASTER.reset();
    ace::common::Path incPath = ace::fs::Directory().path() / ace::common::Path("includes/");
    MASTER.addModelDirectory(incPath);
  }

 protected:
  static const char * prgnam;
};

const char * Includes::prgnam = "tests";

TEST_F(Includes, Pass_AllDefaulted) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("AllDefaulted.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Includes, Pass_Base) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("Base.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Includes, Fail_Conflict) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("Conflict.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Includes, Pass_DerivedTrigger) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("DerivedTrigger.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Includes, Pass_Empty) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("Empty.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Includes, Pass_Inheritance) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("Inheritance.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Includes, Fail_Invalid) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("Invalid.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Includes, Pass_Plugin) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("Plugin.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Includes, Pass_Simple) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("Simple.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Includes, Pass_Trigger) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("Trigger.json");
  ASSERT_NE(res.get(), nullptr);
}

