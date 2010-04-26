// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/renaming.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_RENAMING_H
#define MCRL2_FDR_RENAMING_H

#include "mcrl2/fdr/term_include_files.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A map list
class maps: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    maps()
      : atermpp::aterm_appl(fdr::detail::constructMaps())
    {}

    /// \brief Constructor.
    /// \param term A term
    maps(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Maps(m_term));
    }

    /// \brief Constructor.
    maps(const map_list& renamings)
      : atermpp::aterm_appl(fdr::detail::gsMakeMaps(renamings))
    {}

    map_list renamings() const
    {
      return atermpp::list_arg1(*this);
    }
};

/// \brief list of mapss
typedef atermpp::term_list<maps> maps_list;

/// \brief vector of mapss
typedef atermpp::vector<maps>    maps_vector;


/// \brief A map/generator list
class mapsgens: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    mapsgens()
      : atermpp::aterm_appl(fdr::detail::constructMapsGens())
    {}

    /// \brief Constructor.
    /// \param term A term
    mapsgens(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_MapsGens(m_term));
    }

    /// \brief Constructor.
    mapsgens(const map_list& renamings, const comprehension_expression_list& comprs)
      : atermpp::aterm_appl(fdr::detail::gsMakeMapsGens(renamings, comprs))
    {}

    map_list renamings() const
    {
      return atermpp::list_arg1(*this);
    }

    comprehension_expression_list comprs() const
    {
      return atermpp::list_arg2(*this);
    }
};

/// \brief list of mapsgenss
typedef atermpp::term_list<mapsgens> mapsgens_list;

/// \brief vector of mapsgenss
typedef atermpp::vector<mapsgens>    mapsgens_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_RENAMING_H
