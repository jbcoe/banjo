// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"


namespace banjo
{

// FIXME: Use the builder.
Compound_stmt&
Parser::on_compound_statement(Stmt_list const& ss)
{
  return build.make_compound_statement(ss);
}


Return_stmt&
Parser::on_return_statement(Token, Expr& e)
{
  // FIXME: Verify that the return statement matches the
  // declared type of the function.
  return build.make_return_statement(e);
}


Declaration_stmt&
Parser::on_declaration_statement(Decl& d)
{
  return build.make_declaration_statement(d);
}


// FIXME: Use the builder.
Expression_stmt&
Parser::on_expression_statement(Expr& d)
{
  return build.make_expression_statement(d);
}


} // namespace banjo
