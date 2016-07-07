// -------------------------------------------------------------
/*
 *     Copyright (c) 2013 Battelle Memorial Institute
 *     Licensed under modified BSD License. A copy of this license can be found
 *     in the LICENSE file in the top level directory of this distribution.
 */
// -------------------------------------------------------------
/**
 * @file   hello.cpp
 * @author William A. Perkins
 * @date   2016-07-07 07:04:56 d3g096
 * 
 * @brief  
 * 
 * 
 */
// -------------------------------------------------------------

#include <iostream>
#include <ga++.h>


// -------------------------------------------------------------
//  Main Program
// -------------------------------------------------------------
int
main(int argc, char **argv)
{
  int me;
  int np;

  GA::Initialize(argc, argv);

  me = GA::SERVICES.nodeid();
  np = GA::SERVICES.nodes();

  std::cout << "Hello from process " << me << " of " << np << std::endl;

  GA::Terminate();
}
