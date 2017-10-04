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

class Dependency : public ::testing::Test {
 public:
  static void SetUpTestCase() {
    MASTER.reset();
    ace::common::Path incPath = ace::fs::Directory().path() / ace::common::Path("dependency/");
    MASTER.addModelDirectory(incPath);
    incPath = ace::fs::Directory().path() / ace::common::Path("includes/");
    MASTER.addModelDirectory(incPath);
  }

 protected:
  static const char * prgnam;
};

const char * Dependency::prgnam = "tests";

TEST_F(Dependency, Fail_BadWhen) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_BadWhen.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Dependency, Fail_UnboundWildcard) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("00_UnboundWildcard.json");
  ASSERT_EQ(res.get(), nullptr);
}

TEST_F(Dependency, Pass_Ok) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_Ok_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("dependency/01_Ok.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_Ok_MissingDep) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  auto svr = res->validate("dependency/01_MissingDep.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_NestedWildcard) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_NestedWildcard.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_NestedWildcard_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_NestedWildcard.json");
  auto svr = res->validate("dependency/02_NestedWildcard.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_NestedWildcard_NestedWildcardMissing) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_NestedWildcard.json");
  auto svr = res->validate("dependency/02_NestedWildcardMissing.lua", 1,
                           const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_Template) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("03_Template.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_Template_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("03_Template.json");
  auto svr = res->validate("dependency/03_Template.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_Arrays) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("04_Arrays.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_Arrays_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("04_Arrays.json");
  auto svr = res->validate("dependency/04_Arrays.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_Arrays_MissingDeps) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("04_Arrays.json");
  auto svr = res->validate("dependency/04_ArraysMissingDeps.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_ArrayOfStrings) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("05_ArrayOfStrings.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Fail_ArrayOfStrings_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("05_ArrayOfStrings.json");
  auto svr = res->validate("dependency/05_ArrayOfStrings.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_Constraint) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("06_Constraint.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_Constraint_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("06_Constraint.json");
  auto svr = res->validate("dependency/06_Constraint.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_Constraint_ConstraintOutsideEnum) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("06_Constraint.json");
  auto svr = res->validate("dependency/06_ConstraintOutsideEnum.lua", 1,
                           const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_Constraint_ConstraintOutsideRange) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("06_Constraint.json");
  auto svr = res->validate("dependency/06_ConstraintOutsideRange.lua", 1,
                           const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_Disable) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("07_Disable.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_Disable_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("07_Disable.json");
  auto svr = res->validate("dependency/07_Disable.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_Disable_ValueInUse) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("07_Disable.json");
  auto svr = res->validate("dependency/07_DisableValueInUse.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_Disable_ChainedDeps) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("07_Disable.json");
  auto svr = res->validate("dependency/07_DisableChainedDeps.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_Disable_Conflict) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("07_Disable.json");
  auto svr = res->validate("dependency/07_DisableConflict.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_BadFormat) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("08_BadFormat.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Fail_BadFormat_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("08_BadFormat.json");
  auto svr = res->validate("dependency/08_BadFormat.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_InvalidConstraintRange) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("09_InvalidConstraintRange.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_InvalidConstraintRangeFormat) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("09_InvalidConstraintRangeFormat.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Fail_InvalidConstraintRange_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("09_InvalidConstraintRange.json");
  auto svr = res->validate("dependency/09_InvalidConstraintRange.lua", 1,
                           const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_InvalidConstraintRangeFormat_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("09_InvalidConstraintRangeFormat.json");
  auto svr = res->validate("dependency/09_InvalidConstraintRangeFormat.lua", 1,
                           const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_InvalidConstraintEither) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("10_InvalidConstraintEither.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_InvalidConstraintEitherFormat) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("10_InvalidConstraintEitherFormat.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Fail_InvalidConstraintEither_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("10_InvalidConstraintEither.json");
  auto svr = res->validate("dependency/10_InvalidConstraintEither.lua", 1,
                           const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_InvalidConstraintEitherFormat_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("10_InvalidConstraintEitherFormat.json");
  auto svr = res->validate("dependency/10_InvalidConstraintEitherFormat.lua", 1,
                           const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_UndefinedPath) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("11_UndefinedPath.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Fail_UndefinedPath_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("11_UndefinedPath.json");
  auto svr = res->validate("dependency/11_UndefinedPath.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_MismatchedConstraintType) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("12_MismatchedConstraintType.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Fail_MismatchedConstraintType_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("12_MismatchedConstraintType.json");
  auto svr = res->validate("dependency/12_MismatchedConstraintType.lua", 1,
                           const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_UnboundNestedWildcard) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("13_UnboundNestedWildcard.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Fail_UnboundNestedWildcard_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("13_UnboundNestedWildcard.json");
  auto svr = res->validate("dependency/13_UnboundNestedWildcard.lua", 1,
                           const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_Wildcard) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("14_Wildcard.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_Wildcard_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("14_Wildcard.json");
  auto svr = res->validate("dependency/14_Wildcard.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_Wildcard_Missing) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("14_Wildcard.json");
  auto svr = res->validate("dependency/14_MissingWildcard.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_Recursive) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("15_Recursive.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_Recursive_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("15_Recursive.json");
  auto svr = res->validate("dependency/15_Recursive.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_Recursive_MissingL1) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("15_Recursive.json");
  auto svr = res->validate("dependency/15_MissingRecursiveL1.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_Recursive_MissingL2) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("15_Recursive.json");
  auto svr = res->validate("dependency/15_MissingRecursiveL2.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_Selector) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("16_Selector.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_Selector_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("16_Selector.json");
  auto svr = res->validate("dependency/16_Selector.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_RecursiveSelector) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("17_RecursiveSelector.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_RecursiveSelector_All) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("17_RecursiveSelector.json");
  auto svr = res->validate("dependency/17_RecursiveSelector.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_RecursiveSelector_MissingL1) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("17_RecursiveSelector.json");
  auto svr = res->validate("dependency/17_MissingRecursiveSelectorL1.lua", 1,
                           const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_RecursiveSelector_MissingL2) {
  WRITE_HEADER;
  auto res = ace::model::Model::load("17_RecursiveSelector.json");
  auto svr = res->validate("dependency/17_MissingRecursiveSelectorL2.lua", 1,
                           const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_Plugin) {
  WRITE_HEADER;
  auto plg = ace::model::Model::load("../includes/Trigger.json");
  ASSERT_NE(plg.get(), nullptr);
  auto res = ace::model::Model::load("18_Plugin.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_Plugin_All) {
  WRITE_HEADER;
  ace::model::Model::load("../includes/Trigger.json");
  auto res = ace::model::Model::load("18_Plugin.json");
  auto svr = res->validate("dependency/18_Plugin.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_Plugin_MissingL1) {
  WRITE_HEADER;
  ace::model::Model::load("../includes/Trigger.json");
  auto res = ace::model::Model::load("18_Plugin.json");
  auto svr = res->validate("dependency/18_MissingPluginL1.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_Plugin_MissingL2) {
  WRITE_HEADER;
  ace::model::Model::load("../includes/Trigger.json");
  auto res = ace::model::Model::load("18_Plugin.json");
  auto svr = res->validate("dependency/18_MissingPluginL2.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Dependency, Pass_RecursivePlugin) {
  WRITE_HEADER;
  auto plg = ace::model::Model::load("../includes/Trigger.json");
  ASSERT_NE(plg.get(), nullptr);
  auto res = ace::model::Model::load("19_RecursivePlugin.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Dependency, Pass_RecursivePlugin_All) {
  WRITE_HEADER;
  ace::model::Model::load("../includes/Trigger.json");
  auto res = ace::model::Model::load("19_RecursivePlugin.json");
  auto svr = res->validate("dependency/19_RecursivePlugin.lua", 1, const_cast<char **>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Dependency, Fail_RecursivePlugin_Missing) {
  WRITE_HEADER;
  ace::model::Model::load("../includes/Trigger.json");
  auto res = ace::model::Model::load("19_RecursivePlugin.json");
  auto svr = res->validate("dependency/19_MissingRecursivePlugin.lua", 1,
                           const_cast<char **>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

