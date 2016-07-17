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

#include <ace/model/Coach.h>
#include <iostream>

namespace ace {
namespace model {

const Coach::Branch Coach::Branch::Root(0, 0);

Coach::Branch::Branch(const uint64_t mask, const size_t count)
    : m_mask(mask), m_count(count) { }

Coach::Branch
Coach::Branch::push(const Type t) const {
  Coach::Branch here = *this;
  switch (here.front()) {
    case Type::Corner : here = here.pop().push(Type::None);
                        break;
    case Type::Tee    : here = here.pop().push(Type::Straight);
                        break;
    default           : break;
  }
  return here.append(t);
}

Coach::Branch
Coach::Branch::pop() const {
  size_t count = m_count - 1;
  uint64_t erase = 0xFF;
  erase = ~(erase << (count * 2));
  uint64_t mask = erase & m_mask;
  return Branch(mask, count);
}

std::ostream &
Coach::Branch::print(std::ostream & o) const {
  uint64_t iter = m_mask;
  while (iter !=0) {
    switch (iter & 0x3) {
      case Branch::Type::Straight : o << "│   ";
                                    break;
      case Branch::Type::Corner   : o << "└── ";
                                    break;
      case Branch::Type::Tee      : o << "├── ";
                                    break;
      default                     : o << "    ";
                                    break;
    }
    iter = iter >> 2;
  }
  return o;
}

Coach::Branch
Coach::Branch::append(const Type t) const {
  uint64_t elem = t;
  uint64_t mask = (elem << (m_count * 2)) | m_mask;
  size_t count = m_count + 1;
  return Branch(mask, count);
}

Coach::Branch::Type
Coach::Branch::front() const {
  uint64_t sel = 0xFF;
  sel = sel << ((m_count - 1) * 2);
  Type t = static_cast<Type>((m_mask & sel) >> ((m_count - 1) * 2));
  return t;
}

void
Coach::display(Branch const & br) const {
}


bool
Coach::explain(tree::Path const & p, tree::Path::const_iterator const & i) const {
  if (i == p.end()) {
    std::cout << "[" << p << "]" << std::endl;
  }
  return true;
}

} // namespace model
} // namespace ace

