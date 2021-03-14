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

%extra_argument       { ace::tree::Path ** ppPath }

%token_type           { Token * }
%token_destructor     { delete $$; }

%type path            { ace::tree::Path * }
%destructor path      { }

%type root            { ace::tree::path::Item * }
%destructor root      { }

%type accessorL       { std::list<ace::tree::path::Item *> * }
%destructor accessorL { delete $$; }

%type member          { ace::tree::path::Item * }
%destructor member    { }

%type index           { std::vector<size_t> * }
%destructor index     { delete $$; }

%type range           { ace::tree::path::Item::Range * }
%destructor range     { delete $$; }

%type entries         { std::vector<size_t> * }
%destructor entries   { delete $$; }

%include {
  #include "Parser.h"
  #include <ace/tree/Item.h>
  #include <ace/tree/Token.h>
  #include <ace/common/Log.h>
  #include <cassert>
  #include <iostream>
  #include <list>
#if defined(__linux__)
  #include <malloc.h>
#elif defined(__OpenBSD__)
  #include <stdlib.h>
#elif defined(__MACH__)
  #include <malloc/malloc.h>
#else
#error "Unsupported operating system"
#endif
}

%syntax_error {
  ACE_LOG(Error, "Recoverable JSONPath syntax error");
}

%parse_failure {
  ACE_LOG(Error, "Critical JSONPath syntax error");
}

// Path definition

path ::= root(A) accessorL(B).
{
  auto * p = new ace::tree::Path();
  p->push(ace::tree::path::Item::Ref(A));
  for (auto & i : *B) {
    p->push(ace::tree::path::Item::Ref(i));
  }
  delete B;
  *ppPath = p;
}

// Root definition

root(A) ::= GROOT.
{
  ACE_LOG(Extra, "root ::= GROOT");
  A = new ace::tree::path::Item(ace::tree::path::Item::Type::Global);
}

root(A) ::= LROOT.
{
  ACE_LOG(Extra, "root ::= LROOT");
  A = new ace::tree::path::Item(ace::tree::path::Item::Type::Local);
}

// Accessor list

accessorL(A) ::= .
{
  A = new std::list<ace::tree::path::Item *>();
}

accessorL(A) ::= accessorL(B) member(C).
{
  ACE_LOG(Extra, "accessor ::= accessorL member");
  B->push_back(C);
  A = B;
}

accessorL(A) ::= accessorL(B) DOT member(C).
{
  ACE_LOG(Extra, "accessor ::= accessorL DOT member");
  C->setRecursive();
  B->push_back(C);
  A = B;
}

// Member

member(A) ::= DOT MEMBER(B).
{
  ACE_LOG(Extra, "member ::= DOT MEMBER");
  auto const & n = dynamic_cast<MemberToken *>(B)->value;
  A = new ace::tree::path::Item(ace::tree::path::Item::Type::Named, n);
  delete B;
}

member(A) ::= index(B).
{
  ACE_LOG(Extra, "member ::= index");
  A = new ace::tree::path::Item(ace::tree::path::Item::Type::Indexed, *B);
  delete B;
}

member(A) ::= range(B).
{
  ACE_LOG(Extra, "member ::= MEMBER range");
  A = new ace::tree::path::Item(ace::tree::path::Item::Type::Ranged, *B);
  delete B;
}

member(A) ::= OPENC WILDCARD CLOSEC.
{
  ACE_LOG(Extra, "member ::= OPENC WILDCARD CLOSEC");
  A = new ace::tree::path::Item(ace::tree::path::Item::Type::Any);
}

member(A) ::= DOT WILDCARD.
{
  ACE_LOG(Extra, "member ::= DOT WILDCARD");
  A = new ace::tree::path::Item(ace::tree::path::Item::Type::Any);
}

// Index

index(A) ::= OPENC entries(B) CLOSEC.
{
  ACE_LOG(Extra, "index ::= [a,b,..]");
  A = B;
}

// Index

range(A) ::= OPENC INDEX(B) SLICE INDEX(C) CLOSEC.
{
  ACE_LOG(Extra, "index ::= [i:j]");
  A = new ace::tree::path::Item::Range {
    dynamic_cast<IndexToken *>(B)->value,
    dynamic_cast<IndexToken *>(C)->value,
    1 
  };
  delete B;
  delete C;
}

range(A) ::= OPENC INDEX(B) SLICE INDEX(C) SLICE INDEX(D) CLOSEC.
{
  ACE_LOG(Extra, "index ::= [i:j:k]");
  A = new ace::tree::path::Item::Range {
    dynamic_cast<IndexToken *>(B)->value,
    dynamic_cast<IndexToken *>(C)->value,
    dynamic_cast<IndexToken *>(D)->value
  };
  delete B;
  delete C;
  delete D;
}

// Entries

entries(A) ::= INDEX(B).
{
  ACE_LOG(Extra, "entry");
  A = new std::vector<size_t>();
  A->push_back(dynamic_cast<IndexToken *>(B)->value);
  delete B;
}

entries(A) ::= entries(B) COMMA INDEX(C).
{
  ACE_LOG(Extra, "entries, entry");
  B->push_back(dynamic_cast<IndexToken *>(C)->value);
  delete C;
  A = B;
}
