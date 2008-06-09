// Author(s): Simona Orzan. Distributed under the Boost
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./sort_instantiator.cpp


#include "mcrl2/pbes/sort_instantiator.h"


//C++ 
#include <cstdio> 
#include <exception> 
#include <iostream> 
#include <fstream> 
#include <string> 
#include <utility> 

#include "mcrl2/pbes/utility.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/sort_utility.h"
#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/core/print.h"


#include "mcrl2/atermpp/algorithm.h"     // replace
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/core/messaging.h"



using namespace mcrl2::data;

// CLASS SORT_INSTANTIATOR

//======================================================================
// enumerates all possible data expressions constructed by sort s
data_expression_list instantiate_sort(data_operation_list fl, sort_expression s)
{
  data_expression_list result;
  data_operation_list constructors = get_constructors(fl,s);
    
  // enumerate all s-expressions generated by each constructor
  for (data_operation_list::iterator c = constructors.begin(); c != constructors.end(); c++)
    {
      //get the domains of this constructor (=function)
      sort_expression_list domains = domain_sorts(c->sort());
      
      data_expression_list domain_instance;
      data_expression dec = data_expression((aterm_appl)(*c));    
      // instantiate each domain, then apply the constructor c
      // to obtain something of sort s
      for (sort_expression_list::iterator d = domains.begin(); d != domains.end(); d++)
	{
	  domain_instance = instantiate_sort(fl,*d);
	  mcrl2::core::gsVerboseMsg(".....instantiate_sort %s: constructor %s, domain %s, domain_instance %s\n", 
		       mcrl2::core::pp(s).c_str(), mcrl2::core::pp(*c).c_str(), mcrl2::core::pp(domains).c_str(), mcrl2::core::pp(domain_instance).c_str());
	  // apply c on domain_instance
	  for (data_expression_list::iterator i = domain_instance.begin(); 
	       i != domain_instance.end(); i++)
	    {	     
	      data_expression_list args;
	      args = push_front(args, *i);
	      result = 
		push_front(result,data_expression(data_application(dec,args) )); 
	    }
	}
      if (domains.empty())
	result = push_front(result,dec);
    }
  return result;
  
}


void sort_instantiator::set_data_operation_list (data_operation_list flist)
{
  fl = flist;
}

void sort_instantiator::instantiate_sorts (mcrl2::data::sort_expression_list sl)
{
  for (sort_expression_list::iterator ss = sl.begin(); ss != sl.end(); ss++){
    t_sdel new_isort;
    new_isort.s = *ss; 
    new_isort.del = instantiate_sort(fl,*ss);
    instantiated_sorts.push_back(new_isort);
  }
}

data_expression_list sort_instantiator::get_enumeration (sort_expression ss)
{
  data_expression_list leeg;
  for (unsigned short i = 0 ; i < instantiated_sorts.size(); i++)
    if (instantiated_sorts[i].s == ss) 
      return instantiated_sorts[i].del;
  return leeg;
}

bool sort_instantiator::is_finite(sort_expression s) 
{
  return mcrl2::data::is_finite(fl,s);
};

// END CLASS   SORT_INSTANTIATOR


