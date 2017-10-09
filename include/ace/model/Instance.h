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

#pragma once

#include <ace/tree/Object.h>
#include <ace/tree/Value.h>

#include <string>

namespace ace {
namespace model {

class Instance {
 public:

  Instance() = default;
  explicit Instance(Instance const &) = default;
  virtual ~Instance() { }

  /**
   * @brief   Check the integrity of an instance before expansion
   * @param r the root object
   * @param v the input value
   * @return  true in case of success, false otherwise
   */
  virtual bool checkInstance(tree::Object const & r, tree::Value const & v) const = 0;

  /**
   * @brief   Expand the instance based on default/deps
   * @param r the root object
   * @param v the input value
   */
  virtual void expandInstance(tree::Object & r, tree::Value & v) = 0;

  /**
   * @brief   Flatten modifications that incurred to the model, if any
   * @param r the root object
   * @param v the input value
   * @return  true in case of success, false otherwise
   */
  virtual bool flattenInstance(tree::Object & r, tree::Value & v) = 0;

  /**
   * @brief   Validate an input value
   * @param m the configuration master
   * @param r the root object
   * @param v the input value
   * @return  true in case of success, false otherwise
   */
  virtual bool resolveInstance(tree::Object const & r, tree::Value const & v) const = 0;
};

} // namespace model
} // namespace ace
