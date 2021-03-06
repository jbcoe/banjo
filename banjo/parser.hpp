// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_PARSER_HPP
#define BANJO_PARSER_HPP

#include "lexer.hpp"
#include "token.hpp"
#include "scope.hpp"
#include "language.hpp"
#include "context.hpp"


namespace banjo
{

// The parser is responsible for transforming a stream of tokens
// into nodes. The parser owns a reference to the buffer for its
// tokens. This supports the resolution of source code locations.
struct Parser
{
  Parser(Context& cxt, Token_stream& ts)
    : cxt(cxt), build(cxt), tokens(ts), state()
  { }

  Term& operator()();

  // Syntactic forms
  Operator_kind any_operator();

  // Syntax
  // Unresolved names
  Name& id();
  Name& unqualified_id();
  Name& destructor_id();
  Name& operator_id();
  Name& conversion_id();
  Name& literal_id();
  Name& template_id();
  Name& concept_id();
  Name& qualified_id();

  // Name helpers
  Name& simple_template_id();
  Term_list template_argument_list();
  Term& template_argument();

  // Nested name specifiers
  Decl& leading_name_specifier();
  Decl& nested_name_specifier();
  Decl* nested_name_specifier_opt();

  // Resolved names
  Type& class_name();
  Type& union_name();
  Type& enum_name();
  Type& type_name();
  Type& type_alias();
  Decl& namespace_name();
  Decl& namespace_alias();
  Decl& template_name();
  Decl& concept_name();

  // Types
  Type& type();
  Type& simple_type();
  Type& decltype_type();
  Type& function_type();
  Type& grouped_type();
  Type& postfix_type();
  Type& sequence_type();
  Type& reference_type();

  // Type helpers
  Type& return_type();
  Type_list type_list();

  // Expressions
  Expr& expression();
  Expr& logical_or_expression();
  Expr& logical_and_expression();
  Expr& equality_expression();
  Expr& relational_expression();
  Expr& multiplicative_expression();
  Expr& additive_expression();
  Expr& unary_expression();
  Expr& postfix_expression();
  Expr& call_expression(Expr&);
  Expr& subscript_expression(Expr&);
  Expr& primary_expression();
  Expr& id_expression();
  Expr& grouped_expression();
  Expr& lambda_expression();
  Expr& requires_expression();
  Expr_list expression_list();

  // Statements
  Stmt& statement();
  Stmt& compound_statement();
  Stmt& return_statement();
  Stmt& declaration_statement();
  Stmt& expression_statement();
  Stmt_list statement_seq();

  // Declarations
  Name& declarator();
  Decl& declaration();
  Decl_list declaration_seq();
  Decl& empty_declaration();

  Decl& enum_declaration();
  Decl& namespace_declaration();
  Decl& axiom_declaration();

  // Initializers
  Decl& variable_declaration();
  Expr& initializer(Decl&);
  Expr& equal_initializer(Decl&);
  Expr& paren_initializer(Decl&);
  Expr& brace_initializer(Decl&);

  // Definitions
  Decl& function_declaration();
  Decl& parameter_declaration();
  Decl_list parameter_list();
  Def& function_definition(Decl&);

  // Classes
  Decl& class_declaration();
  Def& class_definition(Decl&);
  Decl_list member_seq();
  Decl& member_declaration();

  // Templates
  Decl& template_declaration();
  Decl& template_parameter();
  Decl& type_template_parameter();
  Decl& value_template_parameter();
  Decl& template_template_parameter();
  Decl_list template_parameter_list();

  // Constraints, preconditions, and postconditions
  Expr& requires_clause();

  // Concepts
  Decl& concept_declaration();
  Def& concept_definition(Decl&);
  Req& concept_member();
  Req_list concept_member_seq();

  // Requirements
  Req& requirement();
  Req& type_requirement();
  Req& syntactic_requirement();
  Req& semantic_requirement();
  Req& expression_requirement();
  Req& usage_requirement();
  Req_list usage_seq();

  // Modules
  Term& translation_unit();

  // Semantics

  // Identifiers
  Name& on_simple_id(Token);
  Name& on_destructor_id(Token, Type&);
  Name& on_operator_id(Token, Operator_kind);
  Name& on_conversion_id();
  Name& on_literal_id();
  Name& on_template_id(Token, Decl&, Term_list const&);
  Name& on_concept_id(Decl&, Term_list const&);
  Name& on_qualified_id(Decl&, Name&);

  Decl& on_nested_name_specifier();
  Decl& on_nested_name_specifier(Decl&);
  Decl& on_nested_name_specifier(Type&);
  Decl& on_nested_name_specifier(Decl&, Token);
  Decl& on_nested_name_specifier(Decl&, Name&);

  // Names
  Type& on_class_name(Token);
  Type& on_class_name(Name&);
  Type& on_union_name(Token);
  Type& on_union_name(Name&);
  Type& on_enum_name(Token);
  Type& on_enum_name(Name&);
  Type& on_type_alias(Token);
  Type& on_type_alias(Name&);
  Type& on_type_name(Token);
  Type& on_type_name(Name&);
  Decl& on_namespace_name(Token);
  Decl& on_namespace_name(Name&);
  Decl& on_namespace_alias(Token);
  Decl& on_namespace_alias(Name&);
  Decl& on_template_name(Token);
  Decl& on_concept_name(Token);

  // Types
  Type& on_void_type(Token);
  Type& on_bool_type(Token);
  Type& on_int_type(Token);
  Type& on_byte_type(Token);
  Type& on_decltype_type(Token, Expr&);
  Type& on_function_type(Type_list&, Type&);
  Type& on_pointer_type(Token, Type&);
  Type& on_qualified_type(Token, Type&, Qualifier_set);
  Type& on_const_type(Token, Type&);
  Type& on_volatile_type(Token, Type&);
  Type& on_sequence_type(Type&);
  Type& on_reference_type(Token, Type&);

  // Expressions
  Expr& on_logical_and_expression(Token, Expr&, Expr&);
  Expr& on_logical_or_expression(Token, Expr&, Expr&);
  Expr& on_logical_not_expression(Token, Expr&);
  Expr& on_eq_expression(Token, Expr&, Expr&);
  Expr& on_ne_expression(Token, Expr&, Expr&);
  Expr& on_lt_expression(Token, Expr&, Expr&);
  Expr& on_gt_expression(Token, Expr&, Expr&);
  Expr& on_le_expression(Token, Expr&, Expr&);
  Expr& on_ge_expression(Token, Expr&, Expr&);
  Expr& on_add_expression(Token, Expr&, Expr&);
  Expr& on_sub_expression(Token, Expr&, Expr&);
  Expr& on_mul_expression(Token, Expr&, Expr&);
  Expr& on_div_expression(Token, Expr&, Expr&);
  Expr& on_rem_expression(Token, Expr&, Expr&);
  Expr& on_call_expression(Expr&, Expr_list&);
  Expr& on_id_expression(Name&);
  Expr& on_boolean_literal(Token, bool);
  Expr& on_integer_literal(Token);
  Expr& on_requires_expression(Token, Decl_list&, Decl_list&, Req_list&);

  // Statements
  Compound_stmt& on_compound_statement(Stmt_list const&);
  Return_stmt& on_return_statement(Token, Expr&);
  Declaration_stmt& on_declaration_statement(Decl&);
  Expression_stmt& on_expression_statement(Expr&);

  // Declarations
  Decl& on_variable_declaration(Token, Name&, Type&);
  Decl& on_function_declaration(Token, Name&, Decl_list&, Type&);
  Decl& on_class_declaration(Token, Name&);
  Decl& on_namespace_declaration(Token, Name&, Decl_list&);
  Decl& on_concept_declaration(Token, Name&, Decl_list&);
  // Function parameters
  Object_parm& on_function_parameter(Name&, Type&);
  // Template parameters
  Type_parm& on_type_template_parameter(Name&, Type&);
  Type_parm& on_type_template_parameter(Name&);
  // Initializers
  Expr& on_default_initialization(Decl&);
  Expr& on_equal_initialization(Decl&, Expr&);
  Expr& on_paren_initialization(Decl&, Expr_list&);
  Expr& on_brace_initialization(Decl&, Expr_list&);
  // Definitions
  Def& on_function_definition(Decl&, Stmt&);
  Def& on_class_definition(Decl&, Decl_list&);
  Def& on_concept_definition(Decl&, Expr&);
  Def& on_concept_definition(Decl&, Req_list&);
  Def& on_deleted_definition(Decl&);
  Def& on_defaulted_definition(Decl&);

  Name& on_declarator(Name&);

  // Reqirements
  Req& on_type_requirement(Expr&);
  Req& on_syntactic_requirement(Expr&);
  Req& on_semantic_requirement(Decl&);
  Req& on_expression_requirement(Expr&);
  Req& on_basic_requirement(Expr&);
  Req& on_basic_requirement(Expr&, Type&);
  Req& on_conversion_requirement(Expr&, Type&);
  Req& on_deduction_requirement(Expr&, Type&);

  // Miscellaneous
  Namespace_decl& on_translation_unit(Decl_list&);

  // Token matching.
  Token      peek() const;
  Token_kind lookahead() const;
  Token_kind lookahead(int) const;
  bool       next_token_is(Token_kind);
  bool       next_token_is(char const*);
  bool       next_token_is_not(Token_kind);
  bool       next_token_is_not(char const*);
  Token      match(Token_kind);
  Token      match_if(Token_kind);
  Token      require(Token_kind);
  Token      require(char const*);
  Token      accept();

  // Tree matching.
  template<typename T> T* match_if(T& (Parser::* p)());

  // Resources
  Symbol_table& symbols();
  Context&      context();

  // Scope management
  Scope& current_scope();
  Decl&  current_context();

  // Declarations
  Decl& templatize_declaration(Decl&);

  // Maintains the current parse state. This is used to provide
  // context for various parsing routines, and is used by the
  // trial parser for caching parse state.
  struct State
  {
    Decl_list* template_parms = nullptr; // The current (innermost) template parameters
    Expr*      template_cons = nullptr;  // The current (innermost) template constraints

    // Parsing flags.
    bool parsing_declarator = false; // True if parsing a declarator.
    bool assume_typename = false;    // True if the following term is a type.
    bool assume_template = false;    // True if the next identifier is a template.
  };

  struct Assume_template;
  struct Parsing_template;

  Context&      cxt;
  Builder       build;
  Token_stream& tokens;
  State         state;
};


// An RAII helper that sets or clears the flag controlling
// whether or not the 'template' keyword was seen before
// a template-name.
struct Parser::Assume_template
{
  Assume_template(Parser& p, bool b)
    : parser(p), saved(p.state.assume_template)
  {
    parser.state.assume_template = b;
  }

  ~Assume_template()
  {
    parser.state.assume_template = saved;
  }

  Parser& parser;
  bool    saved;
};


// An RAII helper that manages parsing state related to the parsing
// of a declaration nested within a template.
//
// TODO: This can probably be removed and templatize_declaration could
// be implemented in terms of the template scope.
struct Parser::Parsing_template
{
  Parsing_template(Parser&, Decl_list*);
  Parsing_template(Parser&, Decl_list*, Expr*);
  ~Parsing_template();

  Parser&    parser;
  Decl_list* saved_parms;
  Expr*      saved_cons;
};


inline
Parser::Parsing_template::Parsing_template(Parser& p, Decl_list* ps)
  : parser(p)
  , saved_parms(p.state.template_parms)
  , saved_cons(p.state.template_cons)
{
  parser.state.template_parms = ps;
  parser.state.template_cons = nullptr;
}


inline
Parser::Parsing_template::Parsing_template(Parser& p, Decl_list* ps, Expr* c)
  : parser(p)
  , saved_parms(p.state.template_parms)
  , saved_cons(p.state.template_cons)
{
  parser.state.template_parms = ps;
  parser.state.template_cons = c;
}


inline
Parser::Parsing_template::~Parsing_template()
{
  parser.state.template_parms = saved_parms;
  parser.state.template_cons = saved_cons;
}



// The trial parser provides recovery information for the parser
// class. If the trial parse fails, then the state of the underlying
// parser is rewound to the state cached bythe trial parser.
//
// TODO: Can we automatically detect failures without needing
// an explicit indication of failure?
struct Trial_parser
{
  using Position = Token_stream::Position;
  using State = Parser::State;

  Trial_parser(Parser& p)
    : parser(p)
    , pos(p.tokens.position())
    , state(p.state)
    , scope(&p.current_scope())
    , fail(false)
  { }

  void failed() { fail = true; }

  ~Trial_parser()
  {
    // TODO: Manage diagnostics as part of the parser in order to
    // detet failures?
    if (fail) {
      parser.tokens.reposition(pos);
      parser.cxt.set_scope(*scope);
      parser.state = state;
    }
  }

  Parser&  parser;
  Position pos;
  State    state;
  Scope*   scope;
  bool     fail;
};


// -------------------------------------------------------------------------- //
// Implementation

// Match a given tree.
template<typename R>
inline R*
Parser::match_if(R& (Parser::* f)())
{
  Trial_parser p(*this);
  try {
    return &(this->*f)();
  } catch(Translation_error&) {
    p.failed();
  }
  return nullptr;
}


} // nammespace banjo


#endif
