// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast_type.hpp"
#include "ast_expr.hpp"
#include "ast_decl.hpp"
#include "context.hpp"
#include "type.hpp"
#include "template.hpp"
#include "constraint.hpp"
#include "lookup.hpp"
#include "deduction.hpp"
#include "subsumption.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// Attempt to resolve a dependent call to a (single) function template.
Expr&
make_dependent_template_call(Context& cxt, Template_ref& e, Expr_list& args)
{
  Template_decl& temp = e.declaration();

  Decl& pd = temp.parameterized_declaration();

  // FIXME: Factor this into smaller bits. We'll need it when we try
  // to handle overloads.
  if (Function_decl* f = as<Function_decl>(&pd)) {
    // Perform template argument deduction using the parameters of
    // the function template and the given arguments.
    Substitution sub(temp.parameters());
    try {
      deduce_from_call(cxt, f->parameters(), args, sub);
      Decl& tspec = specialize_template(cxt, temp, sub);
      Function_decl& spec = cast<Function_decl>(tspec);
      Type& t = spec.return_type();

      // If the template is constrained, then ensure that the current
      // constraints subsume those of the declaration.
      //
      // Note that an unconstrained template is awlays admissible.
      //
      // TODO: Do this here, or in specialize_template?
      if (temp.is_constrained()) {
        Expr& fcons = temp.constraint();
        Expr& ccons = *cxt.current_template_constraints();

        // The call is admissible iff the current constraints subsume
        // those of the the candidate function.
        if (!subsumes(cxt, ccons, fcons))
          warning(cxt, "call to function template '{}' not covered by constraints", e);
      }
      return cxt.make_call(t, e, args);
    } catch (Translation_error& err) {
      // FIXME: Improve diagnostics.
      throw Type_error("no matching call to 'e'");
    }
  }
  banjo_unhandled_case(pd);
}


Expr&
make_dependent_function_call(Context& cxt, Reference_expr& e, Expr_list& args)
{
  lingo_unimplemented();
}



// Make a dependent function call expression.
Expr&
make_dependent_call(Context& cxt, Expr& e, Expr_list& args)
{
  Type& t = make_fresh_type(cxt);
  Expr& init = cxt.make_call(t, e, args);

  // Unify with previous requirements.
  if (cxt.in_requirements())
    return make_required_expression(cxt, init);

  // Don't check in unconstrained templates.
  if (cxt.in_unconstrained_template())
    return init;

  // Determine if the constraints explicitly admit this declaration.
  Expr& cons = *cxt.current_template_constraints();
  if (Expr* ret = admit_expression(cxt, cons, init))
    return *ret;

  // Otherwise, e refers to a previous declaration, possibly many.
  //
  // TODO: Use dispatch?
  //
  // TODO: Handle the overload case. For overloaded funtions, we
  // need to find them most general.
  if (Template_ref* r = as<Template_ref>(&e))
    return make_dependent_template_call(cxt, *r, args);
  if (Reference_expr* r = as<Reference_expr>(&e))
    return make_dependent_function_call(cxt, *r, args);

  banjo_unhandled_case(e);
}


// Make a non-dependent call expression.
//
// FIXME: Allow calls to expressions of any function type.
//
// NOTE: The builtin function call operator requries an object-to-value
// conversion on the target. Be sure to apply that.
//
// FIXME: If e has user-defined type then we need to look for an
// overloaded operator.
Expr&
make_regular_call(Context& cxt, Expr& e, Expr_list& args)
{
  if (Reference_expr* ref = as<Reference_expr>(&e)) {
    Decl& d = ref->declaration();
    Type& t = declared_type(d);

    if (Function_type* f = as<Function_type>(&t))
      return cxt.make_call(f->return_type(), e, args);

    throw Translation_error(cxt, "'{}' is not callable", e);
  }

  banjo_unhandled_case(e);
}


Expr&
make_call(Context& cxt, Expr& e, Expr_list& args)
{
  if (is_type_dependent(e) || is_type_dependent(args))
    return make_dependent_call(cxt, e, args);
  else
    return make_regular_call(cxt, e, args);
}


} // namespace banjo
