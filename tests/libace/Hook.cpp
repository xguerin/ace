/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation hooks (the "Software"), to deal
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

#include "Common.h"
#include <ace/engine/Master.h>
#include <ace/model/Model.h>

class Hook : public ::testing::Test
{
public:
  static void SetUpTestCase()
  {
    MASTER.reset();
    ace::fs::Path incPath =
      ace::fs::Directory().path() / ace::fs::Path("hook/");
    MASTER.addModelDirectory(incPath);
    incPath = ace::fs::Directory().path() / ace::fs::Path("includes/");
    MASTER.addModelDirectory(incPath);
  }

protected:
  static const char* prgnam;
};

const char* Hook::prgnam = "tests";

TEST_F(Hook, Pass_Ok)
{
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Hook, Pass_Ok_All)
{
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  ASSERT_NE(res.get(), nullptr);
  auto svr = res->validate("hook/01_Ok.lua", 1, const_cast<char**>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Hook, Fail_Ok_MissingMatch)
{
  WRITE_HEADER;
  auto res = ace::model::Model::load("01_Ok.json");
  ASSERT_NE(res.get(), nullptr);
  auto svr =
    res->validate("hook/01_MissingMatch.lua", 1, const_cast<char**>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Hook, Pass_GlobalPath)
{
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_GlobalPath.json");
  ASSERT_NE(res.get(), nullptr);
}

TEST_F(Hook, Pass_GlobalPath_All)
{
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_GlobalPath.json");
  ASSERT_NE(res.get(), nullptr);
  auto svr = res->validate("hook/02_Ok.lua", 1, const_cast<char**>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Hook, Fail_GlobalPath_All)
{
  WRITE_HEADER;
  auto res = ace::model::Model::load("02_GlobalPath.json");
  ASSERT_NE(res.get(), nullptr);
  auto svr = res->validate("hook/02_Error.lua", 1, const_cast<char**>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}

TEST_F(Hook, Pass_Template)
{
  WRITE_HEADER;
  auto res = ace::model::Model::load("03_Template.json");
  ASSERT_NE(res.get(), nullptr);
  auto svr = res->validate("hook/03_Ok.lua", 1, const_cast<char**>(&prgnam));
  ASSERT_NE(svr.get(), nullptr);
}

TEST_F(Hook, Fail_Template)
{
  WRITE_HEADER;
  auto res = ace::model::Model::load("03_Template.json");
  ASSERT_NE(res.get(), nullptr);
  auto svr = res->validate("hook/03_Error.lua", 1, const_cast<char**>(&prgnam));
  ASSERT_EQ(svr.get(), nullptr);
}
