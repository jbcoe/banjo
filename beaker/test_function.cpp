// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include <beaker/ast.hpp>
#include <beaker/lexer.hpp>
#include <beaker/print.hpp>

#include <lingo/symbol.hpp>

#include <iostream>


using namespace lingo;
using namespace beaker;


// A symbol table.
Symbol_table syms;


// Create an identifier and return its symbol.
Name*
make_id(char const* n)
{
  Symbol* sym = syms.put_identifier(identifier_tok, n);
  return new Simple_id(sym);
}


Namespace_decl*
make_global_ns() { return new Namespace_decl(new Global_id()); }


Boolean_type*
make_bool_type() { return new Boolean_type(); }


Integer_type*
make_int_type() { return new Integer_type(); }


Function_type*
make_function_type(Decl_list* p, Type* r)
{
  Decl_list* ps = (Decl_list*)p;
  for (Decl* d : *ps)
    std::cout << *d << '\n';
  return nullptr;
}


Parameter_decl*
make_parameter(char const* n, Type* t)
{
  return new Parameter_decl(make_id(n), t, new Default_init());
}


Function_decl*
make_function(Decl* cxt, char const* n, Decl_list* p, Type* r)
{
  Type* t = make_function_type(p, r);
  Init* i = new Incomplete_init();
  return new Function_decl(cxt, make_id(n), t, p, i);
}


int
main(int argc, char* argv[])
{
  Namespace_decl* global = make_global_ns();
  Decl_list* ps = new Decl_list {
    make_parameter("p1", make_bool_type()),
    make_parameter("p2", make_int_type())
  };
  Decl* f = make_function(global, "f1", ps, make_int_type());
  std::cout << *f << '\n';
}
