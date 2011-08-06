// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/prover/info.h
/// \brief Interface to classes ATerm_Info, AI_Jitty and AI_Inner

#ifndef INFO_H
#define INFO_H

#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/detail/prover/utilities.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

enum Compare_Result
{
  compare_result_smaller,
  compare_result_equal,
  compare_result_bigger
};

/// \brief Base class for classes that provide information about the structure of
/// \brief data expressions in one of the internal formats of the rewriter.
class ATerm_Info
{
  protected:
    /// \brief The rewriter used to translate formulas to the internal format of rewriters.
    boost::shared_ptr<detail::Rewriter> f_rewriter;

    /// \brief ATermAppl representing the constant \c true.
    atermpp::aterm_int f_true;

    /// \brief ATermAppl representing the constant \c false.
    atermpp::aterm_int f_false;

    /// \brief ATermAppl representing the \c if \c then \c else function with type Bool -> Bool -> Bool -> Bool.
    atermpp::aterm_int f_if_then_else_bool;

    /// \brief ATermAppl representing the \c equality function.
    ATerm f_eq;

    /// \brief Flag indicating whether or not the arguments of equality functions are taken into account
    /// \brief when determining the order of expressions.
    bool f_full;

    /// \brief Flag indicating whether or not the result of the comparison between the first two arguments
    /// \brief weighs stronger than the result of the comparison between the second pair of arguments of an
    /// \brief equation, when determining the order of expressions.
    bool f_reverse;

    Compare_Result lexico(Compare_Result a_result1, Compare_Result a_result2)
    {
      return (a_result1 != compare_result_equal) ? a_result1 : a_result2;
    }

    Compare_Result compare_address(atermpp::aterm_appl a_term1, atermpp::aterm_appl a_term2)
    {
      long v_address1 = reinterpret_cast < long >((ATermAppl)a_term1);
      long v_address2 = reinterpret_cast < long >((ATermAppl)a_term2);

      if (v_address1 < v_address2)
      {
        return compare_result_smaller;
      }
      if (v_address1 > v_address2)
      {
        return compare_result_bigger;
      }
      return compare_result_equal;
    }

    bool alpha1(atermpp::aterm_appl a_term1, atermpp::aterm_appl a_term2, size_t a_number)
    {
      if (get_number_of_arguments(a_term1) == a_number)
      {
        return false;
      }
      else
      {
        atermpp::aterm_appl v_term = get_argument(a_term1, a_number);
        return (v_term == a_term2) || lpo1(v_term, a_term2) || alpha1(a_term1, a_term2, ++a_number);
      }
    }

    bool beta1(atermpp::aterm_appl a_term1, atermpp::aterm_appl a_term2)
    {
      const atermpp::aterm v_operator_1 = get_operator(a_term1);
      const atermpp::aterm v_operator_2 = get_operator(a_term2);
      return (compare_address(v_operator_1, v_operator_2) == compare_result_bigger) && majo1(a_term1, a_term2, 0);
    }

    bool gamma1(atermpp::aterm_appl a_term1, atermpp::aterm_appl a_term2)
    {
      const atermpp::aterm v_operator_1 = get_operator(a_term1);
      const atermpp::aterm v_operator_2 = get_operator(a_term2);
      return (v_operator_1 == v_operator_2) && lex1(a_term1, a_term2, 0) && majo1(a_term1, a_term2, 0);
    }

    bool delta1(atermpp::aterm_appl a_term1, atermpp::aterm_appl a_term2)
    {
      return gsOccurs((ATerm)(ATermAppl)a_term2, (ATerm)(ATermAppl)a_term1);
    }

    bool majo1(atermpp::aterm_appl a_term1, atermpp::aterm_appl a_term2, size_t a_number)
    {
      if (get_number_of_arguments(a_term2) == a_number)
      {
        return true;
      }
      else
      {
        atermpp::aterm_appl v_term = get_argument(a_term2, a_number);
        return lpo1(a_term1, v_term) && majo1(a_term1, a_term2, ++a_number);
      }
    }

    bool lex1(atermpp::aterm_appl a_term1, atermpp::aterm_appl a_term2, size_t a_number)
    {
      if (get_number_of_arguments(a_term1) == a_number)
      {
        return false;
      }
      else
      {
        atermpp::aterm_appl v_term1 = get_argument(a_term1, a_number);
        atermpp::aterm_appl v_term2 = get_argument(a_term2, a_number);
        if (v_term1 == v_term2)
        {
          return lex1(a_term1, a_term2, ++a_number);
        }
        else
        {
          return lpo1(v_term1, v_term2);
        }
      }
    }

    /// \brief Returns an integer corresponding to the structure of the guard passed as argument \c a_guard.
    int get_guard_structure(atermpp::aterm_appl a_guard)
    {
      if (is_variable(a_guard))
      {
        return 0;
      }
      if (is_equality(a_guard))
      {
        atermpp::aterm_appl v_term1, v_term2;

        v_term1 = get_argument(a_guard, 0);
        v_term2 = get_argument(a_guard, 1);
        if (is_variable(v_term1) && is_variable(v_term2))
        {
          return 1;
        }
        return 2;
      }
      return 3;
    }

    /// \brief Compares the structure of two guards.
    Compare_Result compare_guard_structure(ATerm a_guard1, ATerm a_guard2)
    {
      if (get_guard_structure(a_guard1) < get_guard_structure(a_guard2))
      {
        return compare_result_smaller;
      }
      if (get_guard_structure(a_guard1) > get_guard_structure(a_guard2))
      {
        return compare_result_bigger;
      }
      return compare_result_equal;
    }

    /// \brief Compares two guards by their arguments.
    Compare_Result compare_guard_equality(ATerm a_guard1, ATerm a_guard2)
    {
      if (f_full && is_equality(a_guard1) && is_equality(a_guard2))
      {
        atermpp::aterm_appl v_g1a0, v_g1a1, v_g2a0, v_g2a1;

        v_g1a0 = get_argument(a_guard1, 0);
        v_g1a1 = get_argument(a_guard1, 1);
        v_g2a0 = get_argument(a_guard2, 0);
        v_g2a1 = get_argument(a_guard2, 1);
        if (f_reverse)
        {
          return lexico(compare_term(v_g1a1, v_g2a1), compare_term(v_g1a0, v_g2a0));
        }
        else
        {
          return lexico(compare_term(v_g1a0, v_g2a0), compare_term(v_g1a1, v_g2a1));
        }
      }
      return compare_result_equal;
    }

    /// \brief Compares terms by their type.
    Compare_Result compare_term_type(atermpp::aterm_appl a_term1, atermpp::aterm_appl a_term2)
    {
      if (is_variable(a_term1) && !is_variable(a_term2))
      {
        return compare_result_bigger;
      }
      if (!is_variable(a_term1) && is_variable(a_term2))
      {
        return compare_result_smaller;
      }
      return compare_result_equal;
    }

    /// \brief Compares terms by checking whether one is a part of the other.
    Compare_Result compare_term_occurs(atermpp::aterm_appl a_term1, atermpp::aterm_appl a_term2)
    {
      if (gsOccurs((ATerm)(ATermAppl)a_term1, (ATerm)(ATermAppl)a_term2))
      {
        return compare_result_smaller;
      }
      if (gsOccurs((ATerm)(ATermAppl)a_term2, (ATerm)(ATermAppl)a_term1))
      {
        return compare_result_bigger;
      }
      return compare_result_equal;
    }

  public:
    /// \brief Constructor that initializes the rewriter.
    ATerm_Info(boost::shared_ptr<detail::Rewriter> a_rewriter)
    {
      // pre: true
      // post: f_rewriter == a_rewriter
      f_rewriter = a_rewriter;
    }

    /// \brief Destructor with no particular functionality.
    virtual ~ATerm_Info()
    {}

    /// \brief Sets the flag ATerm_Info::f_reverse.
    void set_reverse(bool a_reverse)
    {
      f_reverse = a_reverse;
    }

    /// \brief Sets the flag ATerm_Info::f_full.
    void set_full(bool a_bool)
    {
      f_full = a_bool;
    }

    /// \brief Compares two guards.
    Compare_Result compare_guard(ATerm a_guard1, ATerm a_guard2)
    {
      return lexico(
               lexico(
                 compare_guard_structure(a_guard1, a_guard2),
                 compare_guard_equality(a_guard1, a_guard2)
               ),
               compare_address(a_guard1, a_guard2)
             );
    }

    /// \brief Compares two terms.
    Compare_Result compare_term(atermpp::aterm_appl a_term1, atermpp::aterm_appl a_term2)
    {
      return lexico(
               lexico(
                 compare_term_occurs(a_term1, a_term2),
                 compare_term_type(a_term1, a_term2)
               ),
               compare_address(a_term1, a_term2)
             );
    }

    /// \brief Compares two terms using lpo.
    bool lpo1(atermpp::aterm_appl a_term1, atermpp::aterm_appl a_term2)
    {
      if (is_variable(a_term1) && is_variable(a_term2))
      {
        return compare_address(a_term1, a_term2) == compare_result_bigger;
      }
      else if (is_variable(a_term1))
      {
        return false;
      }
      else if (is_variable(a_term2))
      {
        return delta1(a_term1, a_term2);
      }
      else
      {
        return alpha1(a_term1, a_term2, 0) || beta1(a_term1, a_term2) || gamma1(a_term1, a_term2);
      }
    }

    /// \brief Indicates whether or not a term has type bool.
    virtual bool has_type_bool(const data_expression a_term) = 0;
    virtual bool has_type_bool(const atermpp::aterm_appl a_term) = 0;

    /// \brief Returns the number of arguments of the main operator of a term.
    virtual size_t get_number_of_arguments(const atermpp::aterm_appl a_term) = 0;

    /// \brief Returns the main operator of the term \c a_term;
    virtual atermpp::aterm get_operator(const atermpp::aterm_appl a_term) = 0;

    /// \brief Returns the argument with number \c a_number of the main operator of term \c a_term.
    virtual atermpp::aterm_appl get_argument(const atermpp::aterm_appl a_term, const size_t a_number) = 0;

    /// \brief Indicates whether or not a term is equal to \c true.
    virtual bool is_true(atermpp::aterm_appl a_term) = 0;

    /// \brief Indicates whether or not a term is equal to \c false.
    virtual bool is_false(atermpp::aterm_appl a_term) = 0;

    /// \brief Indicates whether or not a term is equal to the \c if \c then \c else function
    /// \brief with type Bool -> Bool -> Bool -> Bool.
    virtual bool is_if_then_else_bool(atermpp::aterm_appl a_term) = 0;

    /// \brief Indicates whether or not a term is a single variable.
    virtual bool is_variable(atermpp::aterm_appl a_term) = 0;

    /// \brief Indicates whether or not a term is an equality.
    virtual bool is_equality(atermpp::aterm_appl a_term) = 0;
};

/// \brief Class that provides information about the structure of
/// \brief data expressions in the internal format of the rewriter
/// \brief with the jitty strategy.
class AI_Jitty: public ATerm_Info
{
  public:
    /// \brief Constructor that initializes all fields.
    AI_Jitty(boost::shared_ptr<detail::Rewriter> a_rewriter)
      : ATerm_Info(a_rewriter)
    {
      f_true = (f_rewriter->toRewriteFormat(sort_bool::true_()))(0);
      f_false = (f_rewriter->toRewriteFormat(sort_bool::false_()))(0);
      f_if_then_else_bool = (f_rewriter->toRewriteFormat(if_(sort_bool::bool_())))(0);
      f_eq = (ATerm) static_cast<ATermAppl>(detail::equal_symbol());
    }

    /// \brief Destructor with no particular functionality.
    virtual ~AI_Jitty()
    {}

    /// \brief Indicates whether or not a term has type bool.
    virtual bool has_type_bool(const data_expression a_term)
    {
      return a_term.sort()==sort_bool::bool_();
    }

    virtual bool has_type_bool(const atermpp::aterm_appl a_term)
    {
      return f_rewriter->fromRewriteFormat(a_term).sort()==sort_bool::bool_();
    }

    /// \brief Returns the number of arguments of the main operator of a term.
    /// \param a_term An expression in the internal format of the rewriter with the jitty strategy.
    /// \return 0, if \c aterm is a constant or a variable.
    ///         The number of arguments of the main operator, otherwise.
    virtual size_t get_number_of_arguments(const atermpp::aterm_appl a_term)
    {
      if (!is_true(a_term) && !is_false(a_term) && !is_variable(a_term))
      {
        return ATgetArity(ATgetAFun(a_term)) - 1;
      }
      else
      {
        return 0;
      }
    }

    /// \brief Returns the main operator of the term \c a_term;
    virtual atermpp::aterm get_operator(const atermpp::aterm_appl a_term) 
    {
      return a_term(0);
    }

    /// \brief Returns the argument with number \c a_number of the main operator of term \c a_term.
    virtual atermpp::aterm_appl get_argument(const atermpp::aterm_appl a_term, const size_t a_number)
    {
      return a_term(a_number + 1);
    }

    /// \brief Indicates whether or not a term is equal to \c true.
    virtual bool is_true(const atermpp::aterm_appl a_term)
    {
      const atermpp::aterm v_term = a_term(0);
      return (v_term == f_true);
    }

    /// \brief Indicates whether or not a term is equal to \c false.
    virtual bool is_false(const atermpp::aterm_appl a_term)
    {
      const atermpp::aterm v_term = a_term(0);
      return (v_term == f_false);
    }

    /// \brief Indicates whether or not a term is equal to the \c if \c then \c else function
    /// \brief with type Bool -> Bool -> Bool -> Bool.
    virtual bool is_if_then_else_bool(const atermpp::aterm_appl a_term)
    {
      atermpp::aterm v_function = a_term(0);
      return (v_function == f_if_then_else_bool && get_number_of_arguments(a_term) == 3);
    }

    /// \brief Indicates whether or not a term is a single variable.
    virtual bool is_variable(const atermpp::aterm_appl a_term)
    {
      return core::detail::gsIsDataVarId((ATermAppl) a_term);
    }

    /// \brief Indicates whether or not a term is an equality.
    virtual bool is_equality(atermpp::aterm_appl a_term)
    {
      if (get_number_of_arguments(a_term) == 2)
      {
        ATerm v_term;

        v_term = ATgetArgument(a_term, 0);
        v_term = (ATerm) ATmakeAppl1(ATmakeAFun("wrap", 1, false), v_term);
        v_term = (ATerm)(ATermAppl) f_rewriter->fromRewriteFormat(v_term);
        v_term = ATgetArgument(v_term, 0);
        return (v_term == f_eq);
      }
      else
      {
        return false;
      }
    }
};

}
}
}

#endif
