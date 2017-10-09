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

#include <ace/tree/Lexer.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <stdlib.h>

#define NEVER(__arg__) __arg__
#include "Parser.c"

namespace {

Token *
getIndex(const char * const beg, const char * const end) {
  size_t result = 0;
  std::istringstream(std::string(beg).substr(0, end-beg)) >> result;
  return new IndexToken { result };
}

Token *
getMember(const char * const beg, const char * const end) {
  return new MemberToken { std::string(beg).substr(0, end-beg) };
}

} // namespace

%%{

machine JSONPath;

action groot_tok {
  Parse(lparser, GROOT, 0, nullptr);
}

action lroot_tok {
  Parse(lparser, LROOT, 0, nullptr);
}

action dot_tok {
  Parse(lparser, DOT, 0, nullptr);
}

action openc_tok {
  Parse(lparser, OPENC, 0, nullptr);
}

action closec_tok {
  Parse(lparser, CLOSEC, 0, nullptr);
}

action slice_tok {
  Parse(lparser, SLICE, 0, nullptr);
}

action wildcard_tok {
  Parse(lparser, WILDCARD, 0, nullptr);
}

action index_tok {  
  Parse(lparser, INDEX, getIndex(ts, te), nullptr);
}

action comma_tok {  
  Parse(lparser, COMMA, 0, nullptr);
}

action member_tok {  
  Parse(lparser, MEMBER, getMember(ts, te), nullptr);
}

groot     = '$';
lroot     = '@';
dot       = '.';
openc     = '[';
closec    = ']';
slice     = ':';
wildcard  = '*';
index     = [0-9]+;
comma     = ',';
member    = [a-zA-Z_\-][a-zA-Z0-9_\-]*;

main := |*
  groot     => groot_tok;
  lroot     => lroot_tok;
  dot       => dot_tok;
  openc     => openc_tok;
  closec    => closec_tok;
  slice     => slice_tok;
  wildcard  => wildcard_tok;
  index     => index_tok;
  comma     => comma_tok;
  member    => member_tok;
*|;

}%%

%% write data;

namespace ace {
namespace tree {
namespace path {

Scan::Scan()
  : cs(0)
  , act(0)
  , ts(0)
  , te(0)
  , lparser(0) {
  lparser = ParseAlloc(malloc);
  %% write init;
}

Scan::~Scan() {
  ParseFree(lparser, free);
}

Path
Scan::parse(std::string const & str) {
  const char* p = str.c_str();
  const char* pe = str.c_str() + str.length();
  const char* eof = pe;
  %% write exec;
  ace::tree::Path * path = nullptr;
  Parse(lparser, 0, 0, &path);
  if (path == nullptr) throw std::invalid_argument(str);
  ace::tree::Path result(*path);
  delete path;
  return result;
}

} // namespace path
} // namespace tree
} // namespace ace

