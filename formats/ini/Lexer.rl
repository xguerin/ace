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
#include "Ini.h"
#include <ace/common/Log.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

std::string *
getString(const char * const beg, const char * const end) {
  return new std::string(std::string(beg).substr(0, end-beg));
}

}

namespace ace {
namespace inifmt {

#define NEVER(__arg__) __arg__
#include "Parser.c"

%%{

machine INIFile;

action dot_tok {
  Parse(lparser, DOT, 0, nullptr);
}

action comma_tok {
  Parse(lparser, COMMA, 0, nullptr);
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

action openc_tok {
  Parse(lparser, OPENC, 0, nullptr);
}

action closec_tok {
  Parse(lparser, CLOSEC, 0, nullptr);
}

action ref_tok {
  Parse(lparser, REF, getString(ts, te), nullptr);
}

action var_tok {
  Parse(lparser, VAR, getString(ts, te), nullptr);
}

action sys_tok {
  Parse(lparser, SYSTEM, getString(ts, te), nullptr);
}

action bool_tok {
  Parse(lparser, BOOL, getString(ts, te), nullptr);
}

action float_tok {
  Parse(lparser, FLOAT, getString(ts, te), nullptr);
}

wspace      = [\t\f' ']*;
dot         = '.';
comma       = ',';
key         = [a-zA-Z_][a-zA-Z0-9_\-]*;
env         = [A-Z_][A-Z_]*;
string      = '"' [^"\n]* '"';
sign        = [+\-];
int         = sign? ('0' | [1-9] [0-9]*);
oct         = sign? ('0' [0-7]+);
hex         = sign? ("0x" [0-9a-fA-F]+);
fract_const = digit* '.' digit+ | digit+ '.';
exponent    = [eE] [+\-]? digit+;
float       = sign? (fract_const exponent? | digit+ exponent);
openc       = '[';
closec      = ']';
ref         = '$' key;
var         = "$(" env ')';
system      = "$(system" [^\n]+ ')';
bool        = "true" | "false";

main := |*
  # INI rules
  dot    => dot_tok;
  comma  => comma_tok;
  bool   => bool_tok;
  key    => key_tok;
  string => string_tok;
  int    => int_tok;
  oct    => oct_tok;
  hex    => hex_tok;
  float  => float_tok;
  openc  => openc_tok;
  closec => closec_tok;
  ref    => ref_tok; 
  var    => var_tok; 
  system => sys_tok; 
  # White spaces and comments
  wspace;
  ';' [^\n]* '\n';
*|;

}%%

%% write data;

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

ini::Statement::Ref
Scan::parse(std::string const & str, const bool dbg) {
  const char* p = str.c_str();
  const char* pe = str.c_str() + str.length();
  const char* eof = pe;
  %% write exec;
  ini::Statement * stmt = nullptr;
  if (dbg) ParseTrace(stdout, (char *)"GRAMMAR>");
  Parse(lparser, 0, 0, &stmt);
  if (stmt == nullptr) throw std::invalid_argument(str);
  return ini::Statement::Ref(stmt);
}

} // namespace inifmt
} // namespace ace
