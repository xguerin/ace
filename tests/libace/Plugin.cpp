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

class Plugin : public ::testing::Test {
 public:
  static void SetUpTestCase() {
    MASTER.reset();
    ace::fs::Path incPath = ace::fs::Directory().path() / ace::fs::Path("plugin/");
    MASTER.addModelDirectory(incPath);
    incPath = ace::fs::Directory().path() / ace::fs::Path("includes/");
    MASTER.addModelDirectory(incPath);
  }

 protected:
  static const char * prgnam;
};

const char * Plugin::prgnam = "tests";

TEST_F(Plugin, Fail_BadAncestor) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadAncestor.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Plugin, Fail_BadArityPlus) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadArityPlus.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Plugin, Fail_BadArityStar) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadArityStar.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Plugin, Fail_BadDefault) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadDefault.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Plugin, Fail_BadTargetArity) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadTargetArity.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Plugin, Fail_InvalidModel) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_InvalidModel.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Plugin, Fail_MissingModel) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_MissingModel.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Plugin, Pass_Ok) {
  WRITE_HEADER;
  auto plg = ace::model::Model::load("../includes/Trigger.json");
  ASSERT_NE(plg.get(), nullptr);
  auto res = ace::model::Model::load("01_Ok.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Plugin, Pass_Ok_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("plugin/01_Ok.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Plugin, Fail_Ok_BadTargetArity) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("plugin/01_BadTargetArity.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Plugin, Fail_Ok_Derived) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("plugin/01_DerivedOk.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Plugin, Pass_Dependencies) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_Dependencies.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Plugin, Pass_Dependencies_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_Dependencies.json");
  auto svr = res->validate("plugin/02_Dependencies.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Plugin, Pass_TriggeredMultiTargetArity) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("03_TriggeredMultiTargetArity.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Plugin, Pass_TriggeredMultiTargetArity_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("03_TriggeredMultiTargetArity.json");
  auto svr = res->validate("plugin/03_MultiTargetArity.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Plugin, Pass_MultipleUseOfBase) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("04_MultipleUseOfBase.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Plugin, Pass_MultipleUseOfBase_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("04_MultipleUseOfBase.json");
  auto svr = res->validate("plugin/04_MultipleUseOfBase.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

class PluginWithPreload : public ::testing::Test {
 public:
  static void SetUpTestCase() {
    MASTER.reset();
    ace::fs::Path incPath = ace::fs::Directory().path() / ace::fs::Path("plugin/");
    MASTER.addModelDirectory(incPath);
    incPath = ace::fs::Directory().path() / ace::fs::Path("includes/");
    MASTER.addModelDirectory(incPath);
  }

 protected:
  static const char * prgnam;
};

const char * PluginWithPreload::prgnam = "tests";

TEST_F(PluginWithPreload, Pass_Ok) {
  WRITE_HEADER;
  auto plf = ace::model::Model::load("../includes/DerivedTrigger.json");
  ASSERT_NE(plf.get(), nullptr);
  auto plg = ace::model::Model::load("../includes/Trigger.json");
  ASSERT_NE(plg.get(), nullptr);
  auto res = ace::model::Model::load("01_Ok.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(PluginWithPreload, Pass_Ok_Derived) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("plugin/01_DerivedOk.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

