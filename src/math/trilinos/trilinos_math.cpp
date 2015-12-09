// -------------------------------------------------------------
/**
 * @file   trilinos.cpp
 * @author William A. Perkins
 * @date   2015-12-09 08:22:50 d3g096
 * 
 * @brief  
 * 
 * 
 */
// -------------------------------------------------------------
// -------------------------------------------------------------
/*
 *     Copyright (c) 2013 Battelle Memorial Institute
 *     Licensed under modified BSD License. A copy of this license can be found
 *     in the LICENSE file in the top level directory of this distribution.
 */
// -------------------------------------------------------------

#include <iostream>
#include <Epetra_MpiComm.h>

namespace gridpack {
namespace math {

// -------------------------------------------------------------
// Initialize
// -------------------------------------------------------------
/// Does whatever is necessary to start up the Trilinos library
void
Initialize(void)
{
  
  Epetra_MpiComm comm(MPI_COMM_WORLD);
  int me(comm.MyPID());
  int nproc(comm.NumProc());

  if (me == 0) {
    printf("\nGridPACK math module configured on %d processors\n",nproc);
  }
}

// -------------------------------------------------------------
// Initialized
// -------------------------------------------------------------
bool
Initialized(void)
{
  return true;
}

// -------------------------------------------------------------
// Finalize
// -------------------------------------------------------------
/// Does whatever is necessary to shut down the PETSc library
void
Finalize(void)
{
  // do nothing
}


} // namespace math
} // namespace gridpack
