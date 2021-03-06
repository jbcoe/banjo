// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "print.hpp"

#include <iostream>

namespace banjo
{


// Parse an expression.
Expr&
Parser::expression()
{
  return logical_or_expression();
}


// Parse a logical or-expression.
//
//    logical-or-expression:
//      logical-and-expression:
//      logical-or-expression '||' logical-and-expression
Expr&
Parser::logical_or_expression()
{
  Expr* e1 = &logical_and_expression();
  while (true) {
    if (Token tok = match_if(bar_bar_tok)) {
      Expr& e2 = logical_and_expression();
      e1 = &on_logical_or_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parser a logical and-expression.
//
//    logical-and-expression:
//      equality-expression:
//      logical-and-expression '&&' equality-expression
//
// FIXME: This skips the bitwise expressions.
Expr&
Parser::logical_and_expression()
{
  Expr* e1 = &equality_expression();
  while (true) {
    if (Token tok = match_if(amp_amp_tok)) {
      Expr& e2 = equality_expression();
      e1 = &on_logical_and_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parser an equality expression.
//
//    equality-expression:
//      relational-expression:
//      equality-expression '==' relational-expression
//      equality-expression '!=' relational-expression
Expr&
Parser::equality_expression()
{
  Expr* e1 = &relational_expression();
  while (true) {
    if (Token tok = match_if(eq_eq_tok)) {
      Expr& e2 = relational_expression();
      e1 = &on_eq_expression(tok, *e1, e2);
    } else if (Token tok = match_if(bang_eq_tok)) {
      Expr& e2 = relational_expression();
      e1 = &on_ne_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse a relational expression.
//
//    relational-expression:
//      unary-expression:
//      relational-expression '<' unary-expression
//      relational-expression '>' unary-expression
//      relational-expression '<=' unary-expression
//      relational-expression '>=' unary-expression
Expr&
Parser::relational_expression()
{
  Expr* e1 = &unary_expression();
  while (true) {
    // Use a switch?
    if (Token tok = match_if(lt_tok)) {
      Expr& e2 = unary_expression();
      e1 = &on_lt_expression(tok, *e1, e2);
    } else if (Token tok = match_if(gt_tok)) {
      Expr& e2 = unary_expression();
      e1 = &on_gt_expression(tok, *e1, e2);
    } else if (Token tok = match_if(lt_eq_tok)) {
      Expr& e2 = unary_expression();
      e1 = &on_le_expression(tok, *e1, e2);
    } else if (Token tok = match_if(gt_eq_tok)) {
      Expr& e2 = unary_expression();
      e1 = &on_ge_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse an additive expression.
//
//    additive-expression:
//      multiplicative-expression:
//      additive-expression '+' multiplicative-expression
//      additive-expression '-' multiplicative-expression
Expr&
Parser::additive_expression()
{
  Expr* e1 = &multiplicative_expression();
  while (true) {
    // Use a switch?
    if (Token tok = match_if(plus_tok)) {
      Expr& e2 = multiplicative_expression();
      e1 = &on_add_expression(tok, *e1, e2);
    } else if (Token tok = match_if(minus_tok)) {
      Expr& e2 = unary_expression();
      e1 = &on_sub_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse a multiplicative expression.
//
//    multiplicative-expression:
//      unary-expression:
//      multiplicative-expression '*' unary-expression
//      multiplicative-expression '/' unary-expression
//      multiplicative-expression '%' unary-expression
Expr&
Parser::multiplicative_expression()
{
  Expr* e1 = &unary_expression();
  while (true) {
    // Use a switch?
    if (Token tok = match_if(star_tok)) {
      Expr& e2 = unary_expression();
      e1 = &on_mul_expression(tok, *e1, e2);
    } else if (Token tok = match_if(slash_tok)) {
      Expr& e2 = unary_expression();
      e1 = &on_div_expression(tok, *e1, e2);
    } else if (Token tok = match_if(percent_tok)) {
      Expr& e2 = unary_expression();
      e1 = &on_rem_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse a unary expression.
//
//    unary-expression:
//      postfix-expression
//      '!' unary-expression
//
// TODO: This omits the arithmetic, bitwise, and object unary operators.
//
// TODO: The recursive part might include cast epxressions.
Expr&
Parser::unary_expression()
{
  // Use a switch?
  if (Token tok = match_if(bang_tok)) {
    Expr& e = unary_expression();
    return on_logical_not_expression(tok, e);
  } else {
    return postfix_expression();
  }
}


// Parse a postfix-expression.
//
//    postfix-expression:
//      primary-expression
//      postfix-expression '(' [expression-list] ')'
//      postfix-expression '[' [expression-list] ']'
//
// TODO: Add lots of stuff here.
Expr&
Parser::postfix_expression()
{
  Expr* e = &primary_expression();
  while (true) {
    if (lookahead() == lparen_tok)
      e = &call_expression(*e);
    else
      break;
  }
  return *e;
}


// Parse a call expression. This is a subroutine of the postfix
// expression parser.
//
//    postfix-expression:
//      postfix-expression '(' [expression-list] ')'
Expr&
Parser::call_expression(Expr& e)
{
  Expr_list es;
  require(lparen_tok);
  if (lookahead() != rparen_tok)
    es = expression_list();
  match(rparen_tok);
  return on_call_expression(e, es);
}


// Parse a comma-separated list of expressions.
//
//    expression-list;
//      expression
//      expression-list ',' expression
Expr_list
Parser::expression_list()
{
  Expr_list es;
  do {
    Expr& e = expression();
    es.push_back(e);
  } while (match_if(comma_tok));
  return es;
}


// Parse a primary expression.
//
//    primary-expression:
//      literal
//      id-expression
//      requires-expression
//      lambda-expression
//      '(' expression ')'
Expr&
Parser::primary_expression()
{
  switch (lookahead()) {
    case true_tok:
      return on_boolean_literal(accept(), true);
    case false_tok:
      return on_boolean_literal(accept(), false);
    case integer_tok:
      return on_integer_literal(accept());
    case identifier_tok:
      return id_expression();
    case requires_tok:
      return requires_expression();
    default:
      break;
  }

  if (lookahead() == lparen_tok)
    return grouped_expression();

  error(tokens.location(), "expected primary-expression");
  throw Syntax_error("primary");
}


// Parse an id-expression.
//
//    id-expression:
//      variable-name
//      function-name
//      overloaded-name
//      concept-check
//
// A concept-check is a sequence of template arguments applied
// to a concept-name.
Expr&
Parser::id_expression()
{
  Name& n = id();
  return on_id_expression(n);
}


// Parse a requires expression.
//
//    requires-expression:
//      'requires' ['<' template-parameter-list '>'] ['(' parameter-list ')'] requires-body
//
Expr&
Parser::requires_expression()
{
  Token tok = require(requires_tok);

  // Match template parameter.
  //
  // TODO: Introduce a new scope.
  Decl_list tparms;
  if (match_if(lt_tok)) {
    tparms = template_parameter_list();
    match(gt_tok);
  }

  // Parse parameters in a new block scope.
  Enter_scope scope(cxt, cxt.make_requires_scope());
  Decl_list parms;
  if (match_if(lparen_tok)) {
    parms = parameter_list();
    match(rparen_tok);
  }

  match(lbrace_tok);
  Req_list reqs = usage_seq();
  match(rbrace_tok);

  return on_requires_expression(tok, tparms, parms, reqs);
}


// Parse a paren-enclosed expression.
//
//    grouped-expr ::= '(' expr ')'
Expr&
Parser::grouped_expression()
{
  require(lparen_tok);
  Expr& e = expression();
  match(rparen_tok);
  return e;
}


} // namespace banjo
