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

%extra_argument              { ace::sexp::Value ** ppExp }

%token_type                  { std::string * }
%token_destructor            { delete $$; }

%type expr                   { ace::sexp::Value * }
%destructor expr             {  }

%type value                  { ace::sexp::Value * }
%destructor value            { }

%type value_list             { ace::sexp::Value * }
%destructor value_list       { }

%type value_list_items       { ace::sexp::Value * }
%destructor value_list_items {  }

%include {
  #include "Parser.h"
  #include "Sexp.h"
  #include <ace/common/Log.h>
  #include <cassert>
  #include <iostream>
  #include <string>
}

%syntax_error {
  ACE_LOG(Error, "Recoverable SexpFile syntax error");
}

%parse_failure {
  ACE_LOG(Error, "Critical SexpFile syntax error");
}

// Statement definition

statement ::= expr(A).
{
  *ppExp = A;
}

// Expression definition

expr(A) ::= value(B).
{
  A = B;
}

expr(A) ::= value_list(B).
{
  A = B;
}

// Value list definition

value_list(A) ::= OPENP CLOSEP.
{
  A = new ace::sexp::List;
}

value_list(A) ::= OPENP value_list_items(B) CLOSEP.
{
  A = B;
}

value_list_items(A) ::= expr(B).
{
  A = new ace::sexp::List(ace::sexp::Value::Ref(B));
}

value_list_items(A) ::= expr(B) DOT expr(C).
{
  A = new ace::sexp::List(ace::sexp::Value::Ref(B));
  static_cast<ace::sexp::List &>(*A).append(ace::sexp::Value::Ref(C));
}

value_list_items(A) ::= value_list_items(B) expr(C).
{
  auto p = ace::sexp::Value::Ref(new ace::sexp::List(ace::sexp::Value::Ref(C)));
  static_cast<ace::sexp::List &>(*B).append(p);
  A = B;
}

// Value definition

value(A) ::= STRING(B).
{
  size_t len = B->length() - 2;
  auto content = B->substr(1, len);
  A = new ace::sexp::String(content);
}

value(A) ::= INTEGER_DEC(B).
{
  A = new ace::sexp::Integer(ace::sexp::Integer::Base::Decimal, *B);
}

value(A) ::= INTEGER_OCT(B).
{
  A = new ace::sexp::Integer(ace::sexp::Integer::Base::Octal, *B);
}

value(A) ::= INTEGER_HEX(B).
{
  A = new ace::sexp::Integer(ace::sexp::Integer::Base::Hexadecimal, *B);
}

value(A) ::= BOOL(B).
{
  A = new ace::sexp::Boolean(*B);
}

value(A) ::= KEY(B).
{
  A = new ace::sexp::Symbol(*B);
}

value(A) ::= FLOAT(B).
{
  A = new ace::sexp::Float(*B);
}
