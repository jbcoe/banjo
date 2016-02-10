
// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_BUILDER_HPP
#define BANJO_BUILDER_HPP

#include "prelude.hpp"
#include "context.hpp"
#include "token.hpp"
#include "ast.hpp"

#include <lingo/token.hpp>


namespace banjo
{

// An interface to an AST builder.
//
// TODO: Factor all the checking into a policy class provided
// as a template parameter?
//
// TODO: Make intelligent decisions about canonicalization. If a
// term (e.g., type/constant) is constructed without a source
// location, then it can be uniqued.
struct Builder
{
  Builder(Context& cxt)
    : cxt(cxt)
  { }

  // Names
  //
  // TODO: Implement more of these.
  Simple_id&      get_id(char const*);
  Simple_id&      get_id(std::string const&);
  Simple_id&      get_id(Symbol const&);
  Simple_id&      get_id(Symbol const*);
  Simple_id&      get_id(Token tok);
  Placeholder_id& get_id();
  // Operator_id&    get_id();
  // Conversion_id&  get_id();
  // Literal_id&     get_id();
  Destructor_id&  get_destructor_id(Type const&);
  Template_id&    get_template_id(Template_decl&, Term_list const&);
  Concept_id&     get_concept_id(Concept_decl&, Term_list const&);
  Qualified_id&   get_qualified_id(Decl&, Name&);
  Global_id&      get_global_id();

  // Types
  Void_type&      get_void_type();
  Boolean_type&   get_bool_type();
  Integer_type&   get_integer_type(bool, int);
  Integer_type&   get_int_type();
  Integer_type&   get_uint_type();
  Float_type&     get_float_type();
  Auto_type&      get_auto_type();
  Decltype_type&  get_decltype_type(Expr&);
  Declauto_type&  get_declauto_type();
  Function_type&  get_function_type(Decl_list const&, Type&);
  Function_type&  get_function_type(Type_list const&, Type&);
  Qualified_type& get_qualified_type(Type&, Qualifier_set);
  Qualified_type& get_const_type(Type&);
  Qualified_type& get_volatile_type(Type&);
  Pointer_type&   get_pointer_type(Type&);
  Reference_type& get_reference_type(Type&);
  Array_type&     get_array_type(Type&, Expr&);
  Sequence_type&  get_sequence_type(Type&);
  Class_type&     get_class_type(Decl&);
  Union_type&     get_union_type(Decl&);
  Enum_type&      get_enum_type(Decl&);
  Typename_type&  get_typename_type(Decl&);
  Synthetic_type& synthesize_type(Decl&);

  // Expressions
  Boolean_expr&   get_bool(bool);
  Boolean_expr&   get_true();
  Boolean_expr&   get_false();
  Integer_expr&   get_integer(Type&, Integer const&);
  Integer_expr&   get_zero(Type&);
  Integer_expr&   get_int(Integer const&);
  Integer_expr&   get_uint(Integer const&);
  Reference_expr& make_reference(Variable_decl& d);
  Reference_expr& make_reference(Constant_decl& d);
  Reference_expr& make_reference(Function_decl& d);
  Check_expr&     make_check(Concept_decl& d, Term_list const&);

  And_expr&       make_and(Type&, Expr&, Expr&);
  Or_expr&        make_or(Type&, Expr&, Expr&);
  Not_expr&       make_not(Type&, Expr&);
  Eq_expr&        make_eq(Type&, Expr&, Expr&);
  Ne_expr&        make_ne(Type&, Expr&, Expr&);
  Lt_expr&        make_lt(Type&, Expr&, Expr&);
  Gt_expr&        make_gt(Type&, Expr&, Expr&);
  Le_expr&        make_le(Type&, Expr&, Expr&);
  Ge_expr&        make_ge(Type&, Expr&, Expr&);
  Call_expr&      make_call(Type&, Expr&, Expr_list const&);
  Call_expr&      make_call(Type&, Function_decl&, Expr_list const&);
  Synthetic_expr& synthesize_expression(Decl&);

  // Statements
  Compound_stmt&    make_compound_statement(Stmt_list const&);
  Return_stmt&      make_return_statement(Expr&);
  Expression_stmt&  make_expression_statement(Expr&);
  Declaration_stmt& make_declaration_statement(Decl&);

  // Initializers
  Trivial_init&   make_trivial_init(Type&);
  Copy_init&      make_copy_init(Type&, Expr&);
  Bind_init&      make_bind_init(Type&, Expr&);
  Direct_init&    make_direct_init(Type&, Decl&, Expr_list const&);
  Aggregate_init& make_aggregate_init(Type&, Expr_list const&);

  // Definitions
  Deleted_def&    make_deleted_definition();
  Defaulted_def&  make_defaulted_definition();
  Expression_def& make_expression_definition(Expr&);
  Function_def&   make_function_definition(Stmt&);
  Class_def&      make_class_definition(Decl_list const&);
  Concept_def&    make_concept_definition(Stmt_list const&);

  Namespace_decl& make_namespace(Name&);
  Namespace_decl& make_namespace(char const*);
  Namespace_decl& get_global_namespace();
  Variable_decl&  make_variable(Name&, Type&);
  Variable_decl&  make_variable(char const*, Type&);
  Variable_decl&  make_variable(Name&, Type&, Expr&);
  Variable_decl&  make_variable(char const*, Type&, Expr&);
  Function_decl&  make_function(Name&, Decl_list const&, Type&);
  Function_decl&  make_function(char const*, Decl_list const&, Type&);
  Class_decl&     make_class(Name&);
  Class_decl&     make_class(char const*);
  Template_decl&  make_template(Decl_list const&, Decl&);
  Concept_decl&   make_concept(Name&, Decl_list const&);
  Concept_decl&   make_concept(Name&, Decl_list const&, Def&);
  Concept_decl&   make_concept(Name&, Decl_list const&, Expr&);
  Concept_decl&   make_concept(char const*, Decl_list const&, Def&);
  Concept_decl&   make_concept(char const*, Decl_list const&, Expr&);

  Object_parm& make_object_parm(Name&, Type&);
  Object_parm& make_object_parm(char const*, Type&);
  Value_parm&  make_value_parm(Name&, Type&);
  Value_parm&  make_value_parm(char const*, Type&);
  Type_parm&   make_type_parameter(Name&);
  Type_parm&   make_type_parameter(char const*);
  Type_parm&   make_type_parameter(Name&, Type&);
  Type_parm&   make_type_parameter(char const*, Type&);

  // Constraints.
  // Note that constraints are canonicalized in order
  // ensure efficient hashingn and equivalence comparison.
  Concept_cons&     get_concept_constraint(Decl&, Term_list&);
  Predicate_cons&   get_predicate_constraint(Expr&);
  Conjunction_cons& get_conjunction_constraint(Cons&, Cons&);
  Disjunction_cons& get_disjunction_constraint(Cons&, Cons&);

  // Resources
  Symbol_table& symbols() { return cxt.symbols(); }

  // Allocate an objet of the given type.
  //
  // TODO: This is a placeholder for using a legitimate object
  // pool in the context (or somewhere else).
  template<typename T, typename... Args>
  T& make(Args&&... args)
  {
    return *new T(std::forward<Args>(args)...);
  }

  Context& cxt;
};


// -------------------------------------------------------------------------- //
// Names

// Returns a simple identifier with the given spelling.
//
// TODO: Unique this?
inline Simple_id&
Builder::get_id(char const* s)
{
  Symbol const* sym = symbols().put_identifier(identifier_tok, s);
  return make<Simple_id>(*sym);
}


// Returns a simple identifier with the given spelling.
inline Simple_id&
Builder::get_id(std::string const& s)
{
  Symbol const* sym = symbols().put_identifier(identifier_tok, s);
  return make<Simple_id>(*sym);
}


// Returns a simple identifier for the given symbol.
inline Simple_id&
Builder::get_id(Symbol const& sym)
{
  lingo_assert(is<Identifier_sym>(&sym));
  return make<Simple_id>(sym);
}


// Returns a simple identifier for the symbol.
inline Simple_id&
Builder::get_id(Symbol const* sym)
{
  return get_id(*sym);
}


// Returns a simple id for the given token.
inline Simple_id&
Builder::get_id(Token tok)
{
  return get_id(tok.symbol());
}


// Returns a placeholder for a name.
//
// TODO: Make placeholders unique. Globally?
inline Placeholder_id&
Builder::get_id()
{
  return make<Placeholder_id>();
}


// Returns a destructor-id for the given type.
inline Destructor_id&
Builder::get_destructor_id(Type const& t)
{
  lingo_unimplemented();
}


inline Template_id&
Builder::get_template_id(Template_decl& d, Term_list const& t)
{
  return make<Template_id>(d, t);
}


inline Concept_id&
Builder::get_concept_id(Concept_decl& d, Term_list const& t)
{
  return make<Concept_id>(d, t);
}


// Returns a qualified-id.
inline Qualified_id&
Builder::get_qualified_id(Decl& d, Name& n)
{
  return make<Qualified_id>(d, n);
}


// Return the global identifier.
inline Global_id&
Builder::get_global_id()
{
  // TODO: Global or no?
  static Global_id n;
  return n;
}


// -------------------------------------------------------------------------- //
// Types

inline Void_type&
Builder::get_void_type()
{
  return make<Void_type>();
}


inline Boolean_type&
Builder::get_bool_type()
{
  return make<Boolean_type>();
}


inline Integer_type&
Builder::get_integer_type(bool s, int p)
{
  return make<Integer_type>(s, p);
}


// TODO: Default precision depends on configuration.
inline Integer_type&
Builder::get_int_type()
{
  return get_integer_type(true, 32);
}


// TODO: Default precision depends on configuration.
inline Integer_type&
Builder::get_uint_type()
{
  return get_integer_type(false, 32);
}


inline Float_type&
Builder::get_float_type()
{
  return make<Float_type>();
}


inline Auto_type&
Builder::get_auto_type()
{
  return make<Auto_type>();
}


inline Decltype_type&
Builder::get_decltype_type(Expr&)
{
  lingo_unimplemented();
}


inline Declauto_type&
Builder::get_declauto_type()
{
  return make<Declauto_type>();
}


inline Function_type&
Builder::get_function_type(Decl_list const& ps, Type& r)
{
  Type_list ts;
  for (Decl& d : *modify(&ps)) {
    Object_parm& p = cast<Object_parm>(d);
    ts.push_back(p.type());
  }
  return get_function_type(ts, r);
}


inline Function_type&
Builder::get_function_type(Type_list const& ts, Type& r)
{
  return make<Function_type>(ts, r);
}


// TODO: Do not build qualified types for functions or arrays.
// Is that a hard error, or do we simply fold the const into
// the return type and/or element type?
inline Qualified_type&
Builder::get_qualified_type(Type& t, Qualifier_set qual)
{
  if (Qualified_type* q = as<Qualified_type>(&t)) {
    q->qual |= qual;
    return *q;
  }
  return make<Qualified_type>(t, qual);
}


inline Qualified_type&
Builder::get_const_type(Type& t)
{
  return get_qualified_type(t, const_qual);
}


inline Qualified_type&
Builder::get_volatile_type(Type& t)
{
  return get_qualified_type(t, volatile_qual);
}


inline Pointer_type&
Builder::get_pointer_type(Type& t)
{
  return make<Pointer_type>(t);
}


inline Reference_type&
Builder::get_reference_type(Type& t)
{
  return make<Reference_type>(t);
}


inline Array_type&
Builder::get_array_type(Type&, Expr&)
{
  lingo_unimplemented();
}


inline Sequence_type&
Builder::get_sequence_type(Type& t)
{
  return make<Sequence_type>(t);
}


// FIXME: Canonicalize class types?
inline Class_type&
Builder::get_class_type(Decl& d)
{
  return make<Class_type>(d);
}


inline Union_type&
Builder::get_union_type(Decl& d)
{
  lingo_unimplemented();
}


inline Enum_type&
Builder::get_enum_type(Decl& d)
{
  lingo_unimplemented();
}


inline Typename_type&
Builder::get_typename_type(Decl& d)
{
  return make<Typename_type>(d);
}


inline Synthetic_type&
Builder::synthesize_type(Decl& d)
{
  return make<Synthetic_type>(d);
}


// -------------------------------------------------------------------------- //
// Expressions

inline Boolean_expr&
Builder::get_bool(bool b)
{
  return make<Boolean_expr>(get_bool_type(), b);
}


inline Boolean_expr&
Builder::get_true()
{
  return get_bool(true);
}


inline Boolean_expr&
Builder::get_false()
{
  return get_bool(false);
}


// TODO: Verify that T can have an integer value?
// I think that all scalars can have integer values.
inline Integer_expr&
Builder::get_integer(Type& t, Integer const& n)
{
  return make<Integer_expr>(t, n);
}


// Returns the 0 constant, with scalar type `t`.
//
// TODO: Verify that t is scalar.
//
// TODO: Produce zero interpratations for any T?
inline Integer_expr&
Builder::get_zero(Type& t)
{
  return get_integer(t, 0);
}


inline Integer_expr&
Builder::get_int(Integer const& n)
{
  return get_integer(get_int_type(), n);
}


inline Integer_expr&
Builder::get_uint(Integer const& n)
{
  // lingo_assert(n.is_nonnegative(n));
  return get_integer(get_uint_type(), n);
}


// Get an expression that refers to a variable. The type
// is a reference to the declared type of the variable.
inline Reference_expr&
Builder::make_reference(Variable_decl& d)
{
  return make<Reference_expr>(get_reference_type(d.type()), d);
}


inline Reference_expr&
Builder::make_reference(Function_decl& d)
{
  return make<Reference_expr>(get_reference_type(d.type()), d);
}


// Make a concept check. The type is bool.
inline Check_expr&
Builder::make_check(Concept_decl& d, Term_list const& as)
{
  return make<Check_expr>(get_bool_type(), d, as);
}


inline And_expr&
Builder::make_and(Type& t, Expr& e1, Expr& e2)
{
  return make<And_expr>(t, e1, e2);
}


inline Or_expr&
Builder::make_or(Type& t, Expr& e1, Expr& e2)
{
  return make<Or_expr>(t, e1, e2);
}


inline Not_expr&
Builder::make_not(Type& t, Expr& e)
{
  return make<Not_expr>(t, e);
}


inline Eq_expr&
Builder::make_eq(Type& t, Expr& e1, Expr& e2)
{
  return make<Eq_expr>(t, e1, e2);
}


inline Ne_expr&
Builder::make_ne(Type& t, Expr& e1, Expr& e2)
{
  return make<Ne_expr>(t, e1, e2);
}


inline Lt_expr&
Builder::make_lt(Type& t, Expr& e1, Expr& e2)
{
  return make<Lt_expr>(t, e1, e2);
}


inline Gt_expr&
Builder::make_gt(Type& t, Expr& e1, Expr& e2)
{
  return make<Gt_expr>(t, e1, e2);
}


inline Le_expr&
Builder::make_le(Type& t, Expr& e1, Expr& e2)
{
  return make<Le_expr>(t, e1, e2);
}


inline Ge_expr&
Builder::make_ge(Type& t, Expr& e1, Expr& e2)
{
  return make<Ge_expr>(t, e1, e2);
}


inline Call_expr&
Builder::make_call(Type& t, Expr& f, Expr_list const& a)
{
  return make<Call_expr>(t, f, a);
}


inline Call_expr&
Builder::make_call(Type& t, Function_decl& f, Expr_list const& a)
{
  return make_call(t, make_reference(f), a);
}


inline Synthetic_expr&
Builder::synthesize_expression(Decl& d)
{
  return make<Synthetic_expr>(declared_type(d), d);
}


// -------------------------------------------------------------------------- //
// Statements

inline Compound_stmt&
Builder::make_compound_statement(Stmt_list const& ss)
{
  return make<Compound_stmt>(ss);
}


inline Return_stmt&
Builder::make_return_statement(Expr& e)
{
  return make<Return_stmt>(e);
}


inline Expression_stmt&
Builder::make_expression_statement(Expr& e)
{
  return make<Expression_stmt>(e);
}


inline Declaration_stmt&
Builder::make_declaration_statement(Decl& d)
{
  return make<Declaration_stmt>(d);
}


// -------------------------------------------------------------------------- //
// Initializers

inline Trivial_init&
Builder::make_trivial_init(Type& t)
{
  return make<Trivial_init>(t);
}


inline Copy_init&
Builder::make_copy_init(Type& t, Expr& e)
{
  return make<Copy_init>(t, e);
}


inline Bind_init&
Builder::make_bind_init(Type& t, Expr& e)
{
  return make<Bind_init>(t, e);
}


inline Direct_init&
Builder::make_direct_init(Type& t, Decl& d, Expr_list const& es)
{
  return make<Direct_init>(t, d, es);
}


inline Aggregate_init&
Builder::make_aggregate_init(Type& t, Expr_list const& es)
{
  return make<Aggregate_init>(t, es);
}


// -------------------------------------------------------------------------- //
// Definitions



inline Deleted_def&
Builder::make_deleted_definition()
{
  return make<Deleted_def>();
}


inline Defaulted_def&
Builder::make_defaulted_definition()
{
  return make<Defaulted_def>();
}


inline Expression_def&
Builder::make_expression_definition(Expr& e)
{
  return make<Expression_def>(e);
}


inline Function_def&
Builder::make_function_definition(Stmt& s)
{
  return make<Function_def>(s);
}


inline Class_def&
Builder::make_class_definition(Decl_list const& ds)
{
  return make<Class_def>(ds);
}


inline Concept_def&
Builder::make_concept_definition(Stmt_list const& ss)
{
  return make<Concept_def>(ss);
}


// -------------------------------------------------------------------------- //
// Declarations

inline Variable_decl&
Builder::make_variable(Name& n, Type& t)
{
  return make<Variable_decl>(n, t);
}


inline Variable_decl&
Builder::make_variable(char const* s, Type& t)
{
  return make_variable(get_id(s), t);
}


inline Variable_decl&
Builder::make_variable(Name& n, Type& t, Expr& i)
{
  lingo_assert(is<Init>(&i));
  return make<Variable_decl>(n, t, i);
}


inline Variable_decl&
Builder::make_variable(char const* s, Type& t, Expr& i)
{
  return make_variable(get_id(s), t, i);
}


// Creates an undefined function with parameters ps and return
// type r.
inline Function_decl&
Builder::make_function(Name& n, Decl_list const& ps, Type& r)
{
  Type& t = get_function_type(ps, r);
  return make<Function_decl>(n, t, ps);
}


inline Function_decl&
Builder::make_function(char const* s, Decl_list const& ps, Type& r)
{
  return make_function(get_id(s), ps, r);
}


inline Class_decl&
Builder::make_class(Name& n)
{
  return make<Class_decl>(n);
}


inline Class_decl&
Builder::make_class(char const* s)
{
  return make<Class_decl>(get_id(s));
}


inline Namespace_decl&
Builder::make_namespace(Name& n)
{
  return make<Namespace_decl>(n);
}


inline Namespace_decl&
Builder::make_namespace(char const* s)
{
  return make_namespace(get_id(s));
}


// FIXME: This should probably be installed on the context.
inline Namespace_decl&
Builder::get_global_namespace()
{
  static Namespace_decl ns(get_global_id());
  return ns;
}


inline Template_decl&
Builder::make_template(Decl_list const& p, Decl& d)
{
  return make<Template_decl>(p, d);
}


inline Concept_decl&
Builder::make_concept(Name& n, Decl_list const& ps)
{
  return make<Concept_decl>(n, ps);
}


inline Concept_decl&
Builder::make_concept(Name& n, Decl_list const& ps, Def& d)
{
  return make<Concept_decl>(n, ps, d);
}


inline Concept_decl&
Builder::make_concept(Name& n, Decl_list const& ps, Expr& e)
{
  return make<Concept_decl>(n, ps, make_expression_definition(e));
}


inline Concept_decl&
Builder::make_concept(char const* s, Decl_list const& ps, Def& d)
{
  return make_concept(get_id(s), ps, d);
}


inline Concept_decl&
Builder::make_concept(char const* s, Decl_list const& ps, Expr& e)
{
  return make_concept(get_id(s), ps, make_expression_definition(e));
}


// TODO: Parameters can't be functions or void. Check this
// property or assert it.
inline Object_parm&
Builder::make_object_parm(Name& n, Type& t)
{
  return make<Object_parm>(n, t);
}


inline Object_parm&
Builder::make_object_parm(char const* s, Type& t)
{
  return make_object_parm(get_id(s), t);
}


inline Type_parm&
Builder::make_type_parameter(Name& n)
{
  return make<Type_parm>(n);
}


inline Type_parm&
Builder::make_type_parameter(char const* n)
{
  return make_type_parameter(get_id(n));
}


// Make a type parameter with a default type.
inline Type_parm&
Builder::make_type_parameter(Name& n, Type& t)
{
  return make<Type_parm>(n, t);
}


// Make a type parameter with a default type.
inline Type_parm&
Builder::make_type_parameter(char const* n, Type& t)
{
  return make_type_parameter(get_id(n), t);
}


inline Value_parm&
Builder::make_value_parm(Name& n, Type& t)
{
  return make<Value_parm>(n, t);
}


inline Value_parm&
Builder::make_value_parm(char const* s, Type& t)
{
  return make_value_parm(get_id(s), t);
}



} // namespace banjo


#endif
