// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tools.h
/// \brief Add your file description here.

#ifndef PBES_DETAIL_TOOLS
#define PBES_DETAIL_TOOLS

#include <stdexcept>
#include <sstream>
#include <climits>
#include <iostream>
#include <sstream>
#include <fstream>
#include "aterm2.h"
#include "mcrl2/print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/struct.h"
#include "mcrl2/print.h"
#include "mcrl2/parse.h"
#include "mcrl2/parse/typecheck.h"
#include "mcrl2/parse/regfrmtrans.h"
#include "mcrl2/alpha.h"
#include "mcrl2/dataimpl.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_translate.h"
#include "mcrl2/lps/detail/algorithms.h"
#include "mcrl2/lps/mcrl22lps.h"

namespace lps {

namespace detail {

  inline
  pbes<> lps2pbes(const specification& spec, const state_formula& formula, bool timed)
  {
    return lps::pbes_translate(formula, spec, timed);
  }

  inline
  state_formula mcf2statefrm(const std::string& formula_text, const specification& spec)
  {
    std::stringstream formula_stream;
    formula_stream << formula_text;
    ATermAppl f = parse_state_formula(formula_stream);
    f = type_check_state_formula(f, spec);
    f = implement_data_state_formula(f, spec);
    f = translate_regular_formula(f);
    return f;
  }

  // 
  inline
  pbes<> lps2pbes(const std::string& spec_text, const std::string& formula_text, bool timed)
  {
    pbes<> result;
    specification spec = mcrl22lps(spec_text);
    state_formula f = mcf2statefrm(formula_text, spec);
    return lps2pbes(spec, f, timed);
  }

} // namespace detail

} // namespace lps

#endif // PBES_DETAIL_TOOLS
