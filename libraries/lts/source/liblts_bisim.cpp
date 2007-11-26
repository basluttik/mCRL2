// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_bisim.cpp

#include "mcrl2/print/messaging.h"
#include "mcrl2/struct.h"
#include "mcrl2/lts/liblts.h"
#include "mcrl2/lts/detail/bsim.h"

#ifdef __cplusplus
using namespace mcrl2::utilities;
using namespace std;
#endif

int traceLevel = 0, optimal = 0, classes = 0; 

namespace mcrl2
{
namespace lts
{

bool bisimulation_reduce(lts &l, bool branching, bool add_class_to_state, vector<string> *tau_actions)
{
  if ( add_class_to_state )
  {
    if ( !l.has_state_info() )
    {
      gsErrorMsg("LTS has no state information; cannot add equivalence class information\n");
      return false;
    } else if ( (ATgetType(l.state_value(0)) != AT_APPL) || strcmp(ATgetName(ATgetAFun((ATermAppl) l.state_value(0))),"STATE") )
    {
      gsErrorMsg("unsuitable state information; can only add equivalence class information to mCRL2 states\n");
      return false;
    }
  }

  set_tau_actions(tau_actions);
  int initState = ReadData(l); 
  
  if ( branching )
  {
    SCC();
    ReduceBranching();  
  } else {
    Reduce();
  }

  if ( add_class_to_state )
  {
    WriteDataAddParam(l,branching?ATtrue:ATfalse);
  } else {
    WriteData(l,initState,branching?ATtrue:ATfalse);
  }

  return true;
}

bool bisimulation_compare(lts &l1, lts &l2, bool branching, vector<string> *tau_actions)
{
  int init1, init2;
  set_tau_actions(tau_actions);
  ReadCompareData(l1, &init1, l2, &init2);
  
  bool equal = false;
  if ( branching )
  {
    equal = CompareBranching(init1, init2) == ATtrue;
  } else {
    equal = Compare(init1, init2) == ATtrue;
  }
  return equal;
}

}
}
