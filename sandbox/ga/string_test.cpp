// -------------------------------------------------------------
/*
 *     Copyright (c) 2013 Battelle Memorial Institute
 *     Licensed under modified BSD License. A copy of this license can be found
 *     in the LICENSE file in the top level directory of this distribution.
 */
// -------------------------------------------------------------
/**
 * @file   string_test.cpp
 * @author William A. Perkins
 * @date   2016-07-07 07:04:11 d3g096
 * 
 * @brief  Simple test of a global array.
 * 
 * 
 */

#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <ga++.h>


// -------------------------------------------------------------
//  Main Program
// -------------------------------------------------------------
int
main(int argc, char **argv)
{
  int me;
  int np;

  static const int msglen = 1024;
  static const int ndim = 2;

  GA::Initialize(argc, argv);

  me = GA::SERVICES.nodeid();
  np = GA::SERVICES.nodes();

  // I really want to have a one-dimensional array of char[msglen],
  // but this is not supported.  So, I make a 2D array of int sized
  // accordingly.

  int buftype = MT_C_INT;
  int bufsize = MA_sizeof(MT_CHAR, msglen, buftype);

  // Make the distributed array: one message for each processor

  int dims[ndim] = { np, bufsize };
  int chunk[ndim] = { 1, dims[1] };
  int lo[ndim], hi[ndim];

  if (me == 0) {
    std::cerr << "Array size: " << dims[0] << " x " << dims[1] << std::endl;
  }

  GA::GlobalArray *g = 
    GA::SERVICES.createGA(buftype, 2, &dims[0], "Test Array", &chunk[0]);

  g->printDistribution();

  // Initialize the distributed array (to empty strings)

  int junk = 0;
  g->fill(&junk);

  // Make a process specific message

  std::ostringstream sbuf;
  sbuf << "Hello from process " << me << " of " << np << std::ends;

  // Put the message in the distributed array


  g->distribution(me, &lo[0], &hi[0]);
  char buf[msglen];

                                // because sbuf.str().c_str() can't be
                                // used directly

  strncpy(buf, sbuf.str().c_str(), msglen);
  junk = 1;
  g->put(&lo[0], &hi[0], &buf[0], &junk);

  // Wait for all processes to complete their message

  GA::SERVICES.sync();


  // Get all the messages and print them out
  
  if (me == 0) {
    int ld = 1;

    for (int p = 0; p < np; p++) {
      lo[0] = p; lo[1] = 0;
      hi[0] = p; hi[1] = dims[1] - 1;

      char buf[msglen];
      g->get(&lo[0], &hi[0], &buf[0], &ld);
      
      std::cout << p << ": " << buf << std::endl;
    }
  }
    
  g->destroy();
  GA::Terminate();
}

