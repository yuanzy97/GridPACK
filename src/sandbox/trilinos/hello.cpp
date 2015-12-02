// -------------------------------------------------------------
/*
 *     Copyright (c) 2013 Battelle Memorial Institute
 *     Licensed under modified BSD License. A copy of this license can be found
 *     in the LICENSE file in the top level directory of this distribution.
 */
// -------------------------------------------------------------
// -------------------------------------------------------------
/**
 * @file   hello.cpp
 * @author William A. Perkins
 * @date   2015-12-02 14:49:12 d3g096
 * 
 * @brief  
 * 
 * 
 */
// -------------------------------------------------------------
// -------------------------------------------------------------
// Created December  2, 2015 by William A. Perkins
// Last Change: 2013-05-03 12:23:12 d3g096
// -------------------------------------------------------------


#include <iostream>
#include <Teuchos_GlobalMPISession.hpp>

// -------------------------------------------------------------
//  Main Program
// -------------------------------------------------------------
int
main(int argc, char **argv)
{
  Teuchos::GlobalMPISession session(&argc,&argv,0);
  int nproc(session.getNProc());
  int rank(session.getRank());

  std::cout << "Hello from process " << rank << " of " << nproc << std::endl;
  return 0;
}
