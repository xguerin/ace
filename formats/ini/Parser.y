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

%extra_argument        { ace::ini::Statement ** ppStatement }

%token_type            { std::string * }
%token_destructor      { delete $$; }

%type section          { ace::ini::Statement * }
%destructor section    { }

%type section_id       { std::list<std::string> * }
%destructor section_id { delete $$; }

%type key              { ace::ini::Statement * }
%destructor key        { }

%type value_list       { ace::ini::ValueList::Data * }
%destructor value_list { delete $$; }

%type value            { ace::ini::Value * }
%destructor value      { }

%include {
  #include "Parser.h"
  #include <ace/common/Log.h>
  #include <cassert>
}

%syntax_error {
  ACE_LOG(Error, "Recoverable INIFile syntax error");
}

%parse_failure {
  ACE_LOG(Error, "Critical INIFile syntax error");
}

// Statement definition

statement ::= section(A).
{
  *ppStatement = A;
}

statement ::= value_list(A).
{
  *ppStatement = new ace::ini::ValueList(*A);
}

// Section definition

section(A) ::= OPENC section_id(B) CLOSEC.
{
  auto path = ace::tree::Path::build();
  path->push(ace::tree::path::Item::build(ace::tree::path::Item::Type::Global));
  for (auto const & e : *B) {
    auto item = ace::tree::path::Item::build(ace::tree::path::Item::Type::Named, e);
    path->push(item);
  }
  A = new ace::ini::Section((*(path->rbegin()))->value(), path);
}

// section_id definition

section_id(A) ::= KEY(B).
{
  A = new std::list<std::string>();
  A->push_back(*B);
}

section_id(A) ::= section_id(B) DOT KEY(C).
{
  B->push_back(*C);
  A = B;
}

// Value list definition

value_list(A) ::= value(B).
{
  A = new ace::ini::ValueList::Data();
  A->push_back(ace::ini::Value::Ref(B));
}

value_list(A) ::= value_list(B) COMMA value(C).
{
  B->push_back(ace::ini::Value::Ref(C));
  A = B;
}

// Value definition

value(A) ::= KEY(B).
{
  A = new ace::ini::Key(*B);
}

value(A) ::= STRING(B).
{
  size_t len = B->length() - 2;
  auto content = B->substr(1, len);
  A = new ace::ini::String(content);
}

value(A) ::= INTEGER_DEC(B).
{
  A = new ace::ini::Integer(ace::ini::Integer::Base::Decimal, *B);
}

value(A) ::= INTEGER_OCT(B).
{
  A = new ace::ini::Integer(ace::ini::Integer::Base::Octal, *B);
}

value(A) ::= INTEGER_HEX(B).
{
  A = new ace::ini::Integer(ace::ini::Integer::Base::Hexadecimal, *B);
}

value(A) ::= REF(B).
{
  size_t len = B->length() - 1;
  auto content = B->substr(1, len);
  A = new ace::ini::Reference("$." + content);
}

value(A) ::= VAR(B).
{
  size_t len = B->length() - 3;
  auto content = B->substr(2, len);
  A = new ace::ini::Environment(content);
}

value(A) ::= SYSTEM(B).
{
  size_t len = B->length() - 9;
  auto content = B->substr(8, len);
  A = new ace::ini::System(content);
}

value(A) ::= BOOL(B).
{
  A = new ace::ini::Boolean(*B);
}

value(A) ::= FLOAT(B).
{
  A = new ace::ini::Float(*B);
}
