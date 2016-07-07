// -------------------------------------------------------------
// file: global_counter.cpp
// -------------------------------------------------------------
// -------------------------------------------------------------
// Battelle Memorial Institute
// Pacific Northwest Laboratory
// -------------------------------------------------------------
// -------------------------------------------------------------
// Created June 21, 2012 by William A. Perkins
// Last Change: 2016-07-07 07:00:00 d3g096
// -------------------------------------------------------------


#include <iostream>
#include <ga++.h>

// -------------------------------------------------------------
//  Main Program
// -------------------------------------------------------------
int
main(int argc, char **argv)
{
  static int gasize = 1;

  GA::Initialize(argc, argv, 0);
  MA_init(MT_C_DBL, 4000, 4000);

  int me(GA::SERVICES.nodeid());

                                // build a GA w/ just one value

  GA::GlobalArray *myga = 
    GA::SERVICES.createGA(MT_C_INT, 1, &gasize, "1D", NULL);

                                // zero it

  myga->zero();

                                // print it

  myga->print();

                                // each process reads and increments the counter

  static int num(10);
  static int zero(0), one(1);
  for (int i = 0; i < num; ++i) {
    int id(myga->readInc(&zero, one));
    std::cout << me << ": ID( " << i << ") = " << id << std::endl;
  }

  myga->destroy();
  GA::Terminate();

  return 0;
  
}
