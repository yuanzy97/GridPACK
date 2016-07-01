// -------------------------------------------------------------
/*
 *     Copyright (c) 2013 Battelle Memorial Institute
 *     Licensed under modified BSD License. A copy of this license can be found
 *     in the LICENSE file in the top level directory of this distribution.
 */
// -------------------------------------------------------------
// -------------------------------------------------------------
/**
 * @file   environment.cpp
 * @author William A. Perkins
 * @date   2016-07-01 10:25:21 d3g096
 * 
 * @brief  
 * 
 * 
 */
// -------------------------------------------------------------

#if GRIDPACK_HAVE_GA
#include <ga++.h>
#endif
#include "environment.hpp"

namespace gridpack {
namespace parallel {

// -------------------------------------------------------------
//  class Environment
// -------------------------------------------------------------

// -------------------------------------------------------------
// Environment:: constructors / destructor
// -------------------------------------------------------------
Environment::Environment(int& argc, char **argv,
                         const long int& ma_stack, 
                         const long int& ma_heap)
  : p_boostEnv(argc, argv)
{

#if GRIDPACK_HAVE_GA
  GA_Initialize();
  MA_init(C_DBL, ma_stack, ma_heap);
#endif
}

Environment::~Environment(void)
{
#if GRIDPACK_HAVE_GA
  GA_Terminate();
#endif
}


} // namespace parallel
} // namespace gridpack

