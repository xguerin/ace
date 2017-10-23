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

#include "Lexer.h"
#include "Sexp.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <stdlib.h>

#define NEVER(__arg__) __arg__
#include "Parser.c"

namespace {

std::string *
getString(const char * const beg, const char * const end) {
  return new std::string(std::string(beg).substr(0, end-beg));
}

}

%%{

machine SexpFile;

action dot_tok {
  Parse(lparser, DOT, 0, nullptr);
}

action key_tok {
  Parse(lparser, KEY, getString(ts, te), nullptr);
}

action string_tok {
  Parse(lparser, STRING, getString(ts, te), nullptr);
}

action int_tok {
  Parse(lparser, INTEGER_DEC, getString(ts, te), nullptr);
}

action oct_tok {
  Parse(lparser, INTEGER_OCT, getString(ts, te), nullptr);
}

action hex_tok {
  Parse(lparser, INTEGER_HEX, getString(ts, te), nullptr);
}

action openp_tok {
  Parse(lparser, OPENP, 0, nullptr);
}

action closep_tok {
  Parse(lparser, CLOSEP, 0, nullptr);
}

action bool_tok {
  Parse(lparser, BOOL, getString(ts, te), nullptr);
}

action float_tok {
  Parse(lparser, FLOAT, getString(ts, te), nullptr);
}

wspace      = [\n\r\t\f' ']*;
dot         = '.';
string      = '"' [^"\n]* '"';
key         = [a-zA-Z_][a-zA-Z0-9_\-]*;
sign        = [+\-];
int         = sign? ('0' | [1-9] [0-9]*);
oct         = sign? ('0' [0-7]+);
hex         = sign? ("0x" [0-9a-fA-F]+);
fract_const = digit* '.' digit+ | digit+ '.';
exponent    = [eE] [+\-]? digit+;
float       = sign? (fract_const exponent? | digit+ exponent);
openp       = '(';
closep      = ')';
bool        = "true" | "false";

main := |*
  # INI rules
  dot    => dot_tok;
  bool   => bool_tok;
  key    => key_tok;
  string => string_tok;
  int    => int_tok;
  oct    => oct_tok;
  hex    => hex_tok;
  float  => float_tok;
  openp  => openp_tok;
  closep => closep_tok;
  # White spaces and comments
  wspace;
  '#' [^\n]* '\n';
*|;

}%%

%% write data;

namespace ace {
namespace sexpfmt {

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

sexp::Value::Ref
Scan::parse(std::string const & str, const bool dbg) {
  const char* p = str.c_str();
  const char* pe = str.c_str() + str.length();
  const char* eof = pe;
  %% write exec;
  sexp::Value * stmt = nullptr;
  if (dbg) ParseTrace(stdout, (char *)"GRAMMAR>");
  Parse(lparser, 0, 0, &stmt);
  if (stmt == nullptr) throw std::invalid_argument(str);
  return sexp::Value::Ref(stmt);
}

} // namespace sexpfmt
} // namespace ace
