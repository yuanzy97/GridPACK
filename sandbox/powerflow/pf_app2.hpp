/*
 *     Copyright (c) 2013 Battelle Memorial Institute
 *     Licensed under modified BSD License. A copy of this license can be found
 *     in the LICENSE file in the top level directory of this distribution.
 */
// -------------------------------------------------------------
/**
 * @file   pf_app2.hpp
 * @author William A. Perkins
 * @date   2014-01-10 09:48:35 d3g096
 * 
 * @brief  
 * 
 * 
 */
// -------------------------------------------------------------

#ifndef _pf_app_h_
#define _pf_app_h_

#include <boost/smart_ptr/scoped_ptr.hpp>
#include <gridpack/timer/coarse_timer.hpp>
#include "pf_factory.hpp"

namespace gridpack {
namespace powerflow {

// Calling program for powerflow application

class PFApp2
{
public:
  /**
   * Basic constructor
   */
  PFApp2(void);
  
  /**
   * Basic destructor
   */
  ~PFApp2(void);
  
  /**
   * Execute application
   */
  void execute(void);
  
private:
};

} // powerflow
} // gridpack
#endif