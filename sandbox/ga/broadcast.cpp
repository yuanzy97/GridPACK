// -------------------------------------------------------------
/*
 *     Copyright (c) 2013 Battelle Memorial Institute
 *     Licensed under modified BSD License. A copy of this license can be found
 *     in the LICENSE file in the top level directory of this distribution.
 */
// -------------------------------------------------------------
/**
 * @file   broadcast.cpp
 * @author William A. Perkins
 * @date   2016-07-07 07:03:11 d3g096
 * 
 * @brief  Small test of GA broadcast of serialized string
 * 
 * 
 */
// -------------------------------------------------------------
// Created May  1, 2007 by William A. Perkins
// Last Change: 2016-07-07 07:01:56 d3g096
// -------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <cstring>
#include <ga++.h>

// -------------------------------------------------------------
//  Main Program
// -------------------------------------------------------------
int
main(int argc, char **argv)
{
  
  static const int msglen = 1024;
  int me;
  int np;

  GA::Initialize(argc, argv);
  me = GA::SERVICES.nodeid();
  np = GA::SERVICES.nodes();

  char buf[msglen];

  if (me == 0) {
    std::ostringstream sbuf;
    sbuf << "Hello from process " << me << " of " << np << std::ends;
    strncpy(buf, sbuf.str().c_str(), msglen);
  }

  GA::SERVICES.brdcst(buf, 256, 0);

  std::cout << "From process " << me << ": \"" << buf << "\"" << std::endl;
  
  GA::Terminate();

}


