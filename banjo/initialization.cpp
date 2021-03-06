// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "initialization.hpp"
#include "ast_type.hpp"
#include "ast_expr.hpp"
#include "conversion.hpp"
#include "inheritance.hpp"
#include "equivalence.hpp"
#include "builder.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// FIXME: Initialization does not apply to dependent types or
// expressions. We should either extend the definition to accommodate
// them gracefully, or we should simply return a placeholder for
// the notation.


// Select a zero-initialization procedure for an object or
// reference of type `t`. Note that zero initialization never
// chooses a constructor.
//
// TODO: What about sequence types and incomplete types?
// Presumably object types are required to be complete prior
// to initialization (i.e., we need the size for memset).
//
// It's possible that T[] counts as a scalar (that's what it
// code-gens to).
Expr&
zero_initialize(Context& cxt, Type& t)
{
  lingo_assert(!is_function_type(t));

  Builder build(cxt);

  // No initialization is performed for reference types.
  if (is_reference_type(t))
    return build.make_trivial_init(t);

  // Zero initialize each sub-object in turn.
  if (is_array_type(t))
    lingo_unimplemented();

  Type& u = t.unqualified_type();

  // Zero each sub-object in turn.
  if (is_class_type(u))
    lingo_unimplemented();

  // Zero initialize the first sub-object.
  if (is_union_type(u))
    lingo_unimplemented();

  // FIXME: Determine the kind of zero that best matches the
  // type (i.e., produce an appropriate literal).
  if (is_scalar_type(u))
    return build.make_copy_init(t, build.get_zero(t));

  // FIXME: I'm not sure that we should have an error here.
  throw std::runtime_error("cannot zero initialize type");
}


// Select a default initialization procedure for an object
// of type `t`.
//
// TODO: What about sequence types? What does it mean to
// default initialize a T[].
//
// TODO: Consider making these 0 initialized by defualt and
// using a special syntax to select trivial initialization.
Expr&
default_initialize(Context& cxt, Type& t)
{
  Builder build(cxt);

  if (is_reference_type(t))
    throw std::runtime_error("default initialization of reference");

  // Select a default initializer for each sub-object.
  if (is_array_type(t))
    lingo_unimplemented();

  // Select a (possibly synthesized) default constructor for u.
  //
  // FIXME: This is totally incorrect. Select the default
  // construtor from t.
  if (is_maybe_qualified_class_type(t))
    return build.make_trivial_init(t);

  // Select a (possibly synthesized) default constructor for u.
  if (is_maybe_qualified_union_type(t))
    lingo_unimplemented();

  // Otherwise, no initialization is performed.
  return build.make_trivial_init(t);
}


// Select a procedure to value-initialize an object.
Expr&
value_initialize(Context& cxt, Type& t)
{
  Builder build(cxt);

  if (is_reference_type(t))
    throw Translation_error("value initialization of reference");

  // FIXME: Can you value initialize a T[]?
  if (is_array_type(t))
    lingo_unimplemented();

  // Either zero-initialize or default-initialize based on
  // the presence of user-defined constructors.
  if (is_maybe_qualified_class_type(t) || is_maybe_qualified_union_type(t))
    lingo_unimplemented();

  // Are we sure that there are no other categories of types?
  return zero_initialize(cxt, t);
}


// Select a procedure to copy initialize an object or reference of type
// `t` by an expression `e`. This corresponds to the initialization of
// a variable by the syntax:
//
//    T x = e;
//
// This also applies in parameter passing, function return, exception
// throwing, and handling.
Expr&
copy_initialize(Context& cxt, Type& t, Expr& e)
{
  Builder build(cxt);

  // If the destination type is a T&, then perform reference
  // initialization.
  if (is_reference_type(t))
    return reference_initialize(cxt, cast<Reference_type>(t), e);

  // Otherwise, If the target type is dependent, perform dependent
  // conversions.
  if (is_dependent_type(t)) {
    Expr& c = dependent_conversion(cxt, e, t);
    return cxt.make_copy_init(t, c);
  }

  // If the destination type is T[N] or T[] and the initializer
  // is `= s` where `s` is a string literal, perform string
  // initialization.
  if (is_array_type(t) || is_sequence_type(t))
    banjo_unhandled_case(t);

  // Find a constructor taking `e` as an argument or if not available,
  // find a user-defined conversion from `e` to `t`.
  if (is_maybe_qualified_class_type(t) || is_maybe_qualified_union_type(t))
    banjo_unhandled_case(t);

  // If the initializer has a source type, then try to find a
  // user-defined conversion from s to the destination type, which
  // should be a (possibly qualified) fundamental type.
  Type& s = e.type();

  // If the source type is dependent, search for dependent conversions.
  if (is_dependent_type(s)) {
    Expr& c = dependent_conversion(cxt, e, t);
    return cxt.make_copy_init(t, c);
  }

  // Search for user-defined conversions to a fundamental type.
  if (is_maybe_qualified_class_type(s) || is_maybe_qualified_union_type(s))
    banjo_unhandled_case(t);

  // If all else fails, try a standard conversion. This should be the
  // case that we have a non-class, fundamental, or dependent type.
  //
  // TODO: Catch exceptions and restructure the error with
  // the conversion error as an explanation.
  Expr& c = standard_conversion(e, t);
  return build.make_copy_init(t, c);
}


// Select a procedure to direct-initialize an object or reference of
// type `t` by a paren-enclosed list of expressions `es`. This corresponds
// to the initialization of a variable by the syntax:
//
//    T x(e1, e2, ..., en);
//
// When the list of expressions is empty, this selects value
// initialization.
//
// This also applies in new expressions, etc.
Expr&
direct_initialize(Context& cxt, Type& t, Expr_list& es)
{
  // Arrays must be copy or list-initialized.
  //
  // FIXME: Provide a better diagnostic.
  if (is_array_type(t) || is_sequence_type(t))
    throw Translation_error("invalid array initialization");

  // If the initializer is (), the object is value initialized.
  if (es.empty())
    return value_initialize(cxt, t);

  // If t is not a class type, then there shall be a single
  // expression. Save the source expression for later.
  if (!is_maybe_qualified_class_type(t)) {
    if (es.size() > 1)
      throw Translation_error("scalar initialized from multiple arguments");
  }
  Expr& e = es.front();

  // If the destination type is a T&, then perform reference
  // initialization on the only element in the list of expressions.
  if (is_reference_type(t))
    return reference_initialize(cxt, cast<Reference_type>(t), e);

  // Otherwise, If the target type is dependent, perform dependent
  // conversions.
  //
  // FIXME: Why does this result in copy initialization?
  if (is_dependent_type(t)) {
    Expr& c = dependent_conversion(cxt, e, t);
    return cxt.make_copy_init(t, c);
  }

  // Find a constructor taking the given arguments.
  if (is_maybe_qualified_class_type(t) || is_maybe_qualified_union_type(t))
    lingo_unimplemented();

  // If the initializer has a source type, then try to find a
  // user-defined conversion from s to the destination type, which
  // should be a (possibly qualified) fundamental type.
  Type& s = e.type();

  // Otherwise, If the target type is dependent, perform dependent
  // conversions.
  //
  // FIXME: Why does this result in copy initialization?
  if (is_dependent_type(s)) {
    Expr& c = dependent_conversion(cxt, e, t);
    return cxt.make_copy_init(t, c);
  }

  // Search for user-defined conversion from the source expression.
  if (is_maybe_qualified_class_type(s))
    lingo_unimplemented();

  // If all else fails, try a a standard conversion. This should be
  // the case that we have a non-class, fundamental type.
  //
  // TODO: Catch exceptions and restructure the error with
  // the conversion error as an explanation.
  Expr& c = standard_conversion(e, t);
  return cxt.make_copy_init(t, c);
}


// Perform direct initialization from a single operand.
//
// TODO: We can optimize by simply duplicating cases from above.
Expr&
direct_initialize(Context& cxt, Type& t, Expr& e)
{
  Expr_list args {&e};
  return direct_initialize(cxt, t, args);
}



// -------------------------------------------------------------------------- //
// List initialization

// Select a procedure to direct-initialize an object or reference of
// type `t` by a brace-enclosed list of expressions `es`. This corresponds
// to the initialization of a variable by the syntax:
//
//    T x{e1, e2, ..., en};
//
// When the list of expressions is empty, this selects value
// initialization.
//
// TODO: Implement me.
Expr&
list_initialize(Context& cxt, Type& t, Expr_list& es)
{
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Reference initialization


// A type q-T1 is reference-related to a type q-T2 if T1 and T2 are
// the same type or T1 is a base class of T2.
//
// TODO: Implement the base class test.
bool
is_reference_related(Type const& t1, Type const& t2)
{
  Type const& u1 = t1.unqualified_type();
  Type const& u2 = t2.unqualified_type();
  return is_equivalent(u1, u2);
}


// Two types q1-t1 and q-t2 are reference compatible if 1 is
// reference-related to t2, and q1 is a superset of q2 (i.e.,
// t1 is as qualified as or more qualified than t2).
//
// TODO: Check for ambiguous base classes.
bool
is_reference_compatible(Type const& t1, Type const& t2)
{
  if (is_reference_related(t1, t2)) {
    Qualifier_set q1 = t1.qualifier();
    Qualifier_set q2 = t2.qualifier();
    return is_superset(q1, q2);
  }
  return false;
}


// Select an initialization of the refernce type `t1` by an expression
// `e`.
//
// NOTE: This doesn't currently handle rvalue references (because the
// language doesn't define them).
//
// TODO: A reference binding may invoke a conversion in order
// to bind to a sub-objet or a user-defined conversion. However,
// these aren't conversions in the standard sense.
//
// TODO: Finish implementing me.
Expr&
reference_initialize(Context& cxt, Reference_type& t1, Expr& e)
{
  Type& r1 = t1.non_reference_type();

  // The initializer has reference type.
  Type& t2 = e.type();

  if (is_reference_type(t2)) {
    Type& r2 = t2.non_reference_type();
    // If t1 is reference-compatible with t2, then t1 binds directly
    // to the initializer. This is true for dependent types also.
    //
    // TODO: If we bind to a base class, we might need to apply a
    // base class conversion in order to explicitly adjust pointer
    // offsets.
    if (is_reference_compatible(r1, r2))
      return cxt.make_bind_init(t1, e);

    // t2 has class type and has a user-defined conversion that is
    // reference compatible with t1, then bind the the to the
    // result of that conversion.
    if (is_class_type(t2))
      ; // Fall through for now...
  }

  // The reference must be a const reference.
  //
  // TODO: Handle const reference bindings to compound objects.
  if (t1.type().qualifier() == const_qual) {

  }

  // TODO: Handle bindings to temporaries.

  throw Type_error("reference binding");
}


// -------------------------------------------------------------------------- //
// Aggregate initialization

Expr&
aggregate_initialize(Context& cxt, Type& t, Expr_list& i)
{
  lingo_unimplemented();
}


} // namespace banjo
