// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file lts_fsm.h
 *
 * \brief This file contains a class that contains labelled transition systems in fsm format.
 * \details A labelled transition system in fsm format is a transition system
 * with as state labels vectors of strings, and as transition labels strings.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_LTS_LTS_FSM_H
#define MCRL2_LTS_LTS_FSM_H

#include <string>
#include <vector>
#include <sstream>
#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/action_label_string.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{
/** \brief This class contains state labels for the fsm format.
    \details An fsm state label is just a vector of integers. There is an array m_state_element
             values that contains strings corresponding to the integers in the vector.
             Each integer i at position j in the vector corresponds with the string
             m_state_element_values[j][i].
*/
class state_label_fsm:  public std::vector < size_t >
{
  public:
    /** \brief Default constructor. The label becomes an empty vector.
    */
    state_label_fsm()
    {}

    /** \brief Default constructor. The label is set to the vector v.
    */
    state_label_fsm(const std::vector < size_t >& v):
      std::vector < size_t >(v)
    {}
};
/** \brief Pretty print a state value of this FSM.
    \details The label l is printed as (t1,...,tn).
    \param[in] l  The state value to pretty print.
    \return           The pretty-printed representation of value. */

inline std::string pp(const state_label_fsm l)
{
  std::string s;
  s = "(";
  for (size_t i=0; i<l.size(); ++i)
  {
    std::stringstream sNr;
    sNr << l[i];
    s += sNr.str();
    if (i+1<l.size())
    {
      s += ",";
    }
  }
  s += ")";
  return s;
}
} // namespace detail


/** \brief The class lts_fsm_t contains labelled transition systems in .fsm format.
    \detail The .fsm format consists of an labelled transition system where the
             action labels are strings, and the state labels are vectors of integers.
             The integers at position i corresponds to a string, which are maintained
             in a separate vector for memory efficiency.
*/
class lts_fsm_t : public lts< detail::state_label_fsm, detail::action_label_string >
{
  protected:
    /** \brief state_element_values contain the values that can occur at the i-th
       position of a state label */
    std::vector < std::vector < std::string > > m_state_element_values;

    /** \brief m_parameters contain the parameters corresponding to the
               consecutive elements of a state vector. A parameter consists
               of two strings: a variable name and a string indicating its sort. */
    std::vector < std::pair < std::string, std::string > > m_parameters;

  public:

    /** \brief The lts_type of this labelled transition system. In this case lts_fsm.
        \return Returns lts_fsm as type of this transition system.
    */
    lts_type type() const
    {
      return lts_fsm;
    }

    /** \brief Provides the vector of strings that correspond to the values
               of the number at position idx in a vector.
        \param[in] idx The index of the parameter.
        \return A vector containing strings representing the possible values
                that this parameter can have.
    */
    std::vector < std::string > state_element_values(size_t idx) const
    {
      assert(idx<m_state_element_values.size());
      return m_state_element_values[idx];
    }

    /** \brief Adds a string to the state element values for the idx-th
               position in a state vector. Returns the number given to this string.
        \param[in] idx The parameter index.
        \param[in] s   String to be added as value for the indicate parameter.
        \return The index for the added parameter.
    */
    size_t add_state_element_value(size_t idx, const std::string& s)
    {
      assert(idx<m_state_element_values.size());
      m_state_element_values[idx].push_back(s);
      return m_state_element_values[idx].size();
    }

    /** \brief Returns the element_index'th element for the parameter with index
               parameter_index.
        \details If there are no values for the parameter_index, the element_index
                 is transformed to a string, and this string is returned.
        \param[in] parameter_index The index for this parameter.
        \param[in] element_index The index to the value string corresponding to this parameter.
        \return The string corresponding to the two given indices.
    */
    std::string state_element_value(size_t parameter_index, size_t element_index) const
    {
      assert(parameter_index<m_state_element_values.size());
      if (m_state_element_values[parameter_index].size()==0)
      {
        // The domain for this parameter has no string; return the string "i"
        std::stringstream number_stream;
        number_stream << element_index;
        return number_stream.str();
      }
      assert(element_index<m_state_element_values[parameter_index].size());
      return m_state_element_values[parameter_index][element_index];
    }

    /** \brief Return the parameters of the state vectors stored in this LTS.
    *   \return The state parameters stored in this LTS.
    */
    std::vector < std::pair < std::string, std::string > > process_parameters() const
    {
      return m_parameters;
    }

    /** \brief Returns the i-th parameter of the state vectors stored in this LTS.
        \param[in] i The index of the parameter.
        \return The variable/sort of the state parameter at index i.
    */
    std::pair < std::string, std::string > process_parameter(size_t i) const
    {
      assert(i<m_parameters.size());
      return m_parameters[i];
    }

    /** \brief Clear the state parameters for this LTS.
    */
    void clear_process_parameters()
    {
      m_parameters.clear();
      m_state_element_values.clear();
    }

    /** \brief Set the state parameters for this LTS.
       \param[in] name  The variable name of the added parameter.
       \param[in] sort  The sort of the added parameter.
    */
    void add_process_parameter(const std::string& name, const std::string& sort)
    {
      assert(m_parameters.size()==m_state_element_values.size());
      m_parameters.push_back(std::pair<std::string,std::string>(name,sort));
      m_state_element_values.push_back(std::vector < std::string >());
    }

    /** \brief Save the labelled transition system to file.
     *  \details If the filename is empty, the result is read from stdout.
     *  \param[in] filename Name of the file from which this lts is read.
     */
    void load(const std::string& filename);
    void loadnew(const std::string& filename);

    /** \brief Save the labelled transition system to file.
     *  \details If the filename is empty, the result is written to stdout.
     *  \param[in] filename Name of the file to which this lts is written.
     */
    void save(const std::string& filename) const;
};

} // namespace lts
} // namespace mcrl2

#endif
