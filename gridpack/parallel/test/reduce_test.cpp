/*
 *     Copyright (c) 2013 Battelle Memorial Institute
 *     Licensed under modified BSD License. A copy of this license can be found
 *     in the LICENSE file in the top level directory of this distribution.
 */
/**
 * @file   reduce_test.cpp
 * @author William A. Perkins
 * @date   2016-07-01 08:56:43 d3g096
 * 
 * @brief  gridpack::parallel::Communicator tests
 * 
 * 
 */
#include <iostream>
#include <vector>

#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>

#include "environment.hpp"
#include "communicator.hpp"

// -------------------------------------------------------------
// reduce_tester
// -------------------------------------------------------------
template <typename T>
void 
reduce_tester(const gridpack::parallel::Communicator& comm)
{
  const int vsize(5);
  int nproc(comm.size());
  int me(comm.rank());
  std::vector<T> 
    v(vsize, static_cast<T>(me)),
    vout;

  T thesum(0);
  for (int p = 0; p < nproc; ++p) {
    thesum += static_cast<T>(p);
  }

  vout = v;
  comm.sum(&vout[0], v.size());
  for (int i = 0; i < vsize; ++i) {
    BOOST_CHECK_EQUAL(vout[i], thesum);
  }

  vout = v;
  comm.max(&vout[0], v.size());
  for (int i = 0; i < vsize; ++i) {
    BOOST_CHECK_EQUAL(vout[i], nproc - 1);
  }

  vout = v;
  comm.min(&vout[0], v.size());
  for (int i = 0; i < vsize; ++i) {
    BOOST_CHECK_EQUAL(vout[i], 0);
  }
}


BOOST_AUTO_TEST_SUITE ( reduce ) 

BOOST_AUTO_TEST_CASE( stringShuffle )
{
  gridpack::parallel::Communicator world;
  reduce_tester<float>(world);
  reduce_tester<double>(world);
  reduce_tester<int>(world);
  reduce_tester<long>(world);
}

BOOST_AUTO_TEST_SUITE_END( )

bool init_function()
{
  return true;
}

// -------------------------------------------------------------
//  Main Program
// -------------------------------------------------------------
int
main(int argc, char **argv)
{
  gridpack::parallel::Environment env(argc, argv);
  return ::boost::unit_test::unit_test_main( &init_function, argc, argv );
}

