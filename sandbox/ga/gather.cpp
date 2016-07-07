// -------------------------------------------------------------
/*
 *     Copyright (c) 2013 Battelle Memorial Institute
 *     Licensed under modified BSD License. A copy of this license can be found
 *     in the LICENSE file in the top level directory of this distribution.
 */
// -------------------------------------------------------------
// -------------------------------------------------------------
/**
 * @file   gather.cpp
 * @author William A. Perkins
 * @date   2016-07-07 07:21:19 d3g096
 * 
 * @brief  
 * 
 * 
 */

// -------------------------------------------------------------


#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <boost/timer.hpp>
#include <ga++.h>


// -------------------------------------------------------------
//  Main Program
// -------------------------------------------------------------
int
main(int argc, char **argv)
{
  static int masize(1024*1024);
  static int iterations(1024);
  static int gasize_per_proc = 10*1024;

  boost::timer mytime;

  int me;
  int np;
  int gasize;

  GA::Initialize(argc, argv, 0);
  bool status = MA_init(MT_C_DBL, masize, masize);
  if (!status) {
    std::cerr << "MA_init failed!" << std::endl;
    GA::Terminate();
    exit(3);
  }
    

  me = GA::SERVICES.nodeid();
  np = GA::SERVICES.nodes();

                                // build a 1D array with default distribution
  gasize = gasize_per_proc*np;
  GA::GlobalArray *myga = 
    GA::SERVICES.createGA(MT_C_INT, 1, &gasize, "1D", NULL);

                                // find out what this process owns
  int lo, hi;
  myga->distribution(me, &lo, &hi);

                                // print out the distribution
  myga->printDistribution();

                                // select some random array indexes to query

  int nsamp = gasize_per_proc;
  std::vector<int> idx(nsamp);
  std::vector<int> v(nsamp);
  std::vector<int*> junk(nsamp);

  // make sure each process has a different seed, otherwise they'll
  // all get the same sequence

  srand(static_cast<unsigned long>(clock())*
        static_cast<unsigned long>(me));
  for (int i = 0; i < nsamp; i++) {
    idx[i] = std::min<int>(static_cast<int>(gasize*static_cast<double>(rand())/
                                            static_cast<double>(RAND_MAX)),
                           gasize - 1);
  }

  for (int i = 0; i < nsamp; i++) {
    junk[i] = &idx[i];
  }

  std::vector<int> vupload(gasize_per_proc);
  std::fill(vupload.begin(), vupload.end(), me);


  for (int iter = 0; iter < iterations; iter++) {

    boost::timer itime;

    myga->put(&lo, &hi, &vupload[0], NULL);

    GA::SERVICES.sync();


                                // try to extract values for those indexes

    myga->gather(&v[0], &junk[0], nsamp);

    std::cout << me << ": iteration " << iter 
              << ": time = " << itime.elapsed() << std::endl;
    
  }

  std::cout << me << ": total total time: " << mytime.elapsed() << std::endl;

  myga->destroy();
  GA::Terminate();

  return 0;
}

