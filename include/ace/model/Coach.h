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

#include <ace/tree/Path.h>
#include <ostream>
#include <cstdint>

namespace ace { namespace model {

class Coach
{
public:
  class Branch
  {
  public:
    typedef enum Type
    {
      None = 0x0,
      Straight = 0x1,
      Corner = 0x2,
      Tee = 0x3
    } Type;

    static const Branch Root;

    Branch() = default;

    Branch push(const Type t) const;
    Branch pop() const;

    std::ostream& print(std::ostream& o) const;

  private:
    Branch(const uint64_t mask, const size_t count);

    Branch append(const Type t) const;
    Type front() const;

    uint64_t m_mask;
    size_t m_count;
  };

public:
  Coach() = default;
  explicit Coach(Coach const&) = default;
  virtual ~Coach() {}

  /**
   * @brief Display object as part of a tree
   */
  virtual void display(Branch const& br) const;

  /**
   * @brief Explain something to the user
   */
  virtual bool explain(tree::Path const& p,
                       tree::Path::const_iterator const& i) const;
};

}}
