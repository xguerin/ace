/**
 * Copyright (c) 2016 Xavier R. Guerin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
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

#include <ace/common/Log.h>
#include <gtest/gtest.h>
#include <fcntl.h>
#include <cstdlib>

int main(int argc, char* argv[]) {
  /**
   * Get the target build path
   */
  char * build_path = getenv("ACE_TESTS_PATH");
  if (build_path == NULL) {
    std::cerr << "ACE_TESTS_PATH must be defined (check CMakeLists.txt)" << std::endl;
    return 1;
  }
  std::string path_str(build_path);
  /**
   * Set LOG environment"
   */
  setenv("ACE_LOG_STREAM", "FILE", 1);
  ace::common::Log::get().recycle();
  ace::common::Log::get().changeFileStreamDestination(path_str + "/tests.log");
  /**
   * Update the log level
   */
  ace::common::Log::Level l = ace::common::Log::Info;
  const char * ll = getenv("ACE_LOG_LEVEL");
  if (ll != 0) l = ace::common::Log::parseLogLevel(ll);
  ace::common::Log::get().setLogLevel(l);
  /**
   * Initialize and run the tests
   */
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
