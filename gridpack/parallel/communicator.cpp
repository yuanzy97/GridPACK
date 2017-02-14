// -------------------------------------------------------------
/*
 *     Copyright (c) 2013 Battelle Memorial Institute
 *     Licensed under modified BSD License. A copy of this license can be found
 *     in the LICENSE file in the top level directory of this distribution.
 */
// -------------------------------------------------------------
// -------------------------------------------------------------
/**
 * @file   communicator.cpp
 * @author William A. Perkins
 * @date   2017-02-10 12:20:47 d3g096
 * 
 * @brief  
 * 
 * 
 */
// -------------------------------------------------------------
#if GRIDPACK_HAVE_GA
#if USE_PROGRESS_RANKS
#include <ga-mpi.h>
#endif
#include <ga++.h>
#endif

#include "gridpack/utilities/uncopyable.hpp"
#include "communicator.hpp"

namespace gridpack {
namespace parallel {

// -------------------------------------------------------------
//  class CommunicatorPrivate
// -------------------------------------------------------------
class CommunicatorPrivate 
  : private utility::Uncopyable
{
public:

  /// Default constructor.
  CommunicatorPrivate(void)
  { }

  /// Destructor
  virtual ~CommunicatorPrivate(void)
  { 
  }

  /// Get GA process group handle
  virtual int handle(void) const = 0;

  /// Mimic GA sync
  virtual void sync(void) const = 0;

  /// Get the world rank
  virtual int worldRank(void) const = 0;

};

// -------------------------------------------------------------
// class CommunicatorPrivateMPI
// -------------------------------------------------------------
class CommunicatorPrivateMPI
  : public CommunicatorPrivate
{
public:

  /// Default constructor.
  CommunicatorPrivateMPI(void)
    : CommunicatorPrivate(), p_comm()
  {}

  CommunicatorPrivateMPI(const boost::mpi::communicator& comm)
    : CommunicatorPrivate(), p_comm(comm)
  { }

  /// Destructor
  ~CommunicatorPrivateMPI(void)
  { 
  }

  /// Get GA process group handle
  int handle(void) const
  {
    // This should not get called, so just return a bogus number
    return -9999;
  }

  /// Mimic GA sync
  void sync(void) const
  {
    p_comm.barrier();
  }

  /// Get the world rank
  int worldRank(void) const
  {
    // This should not get called w/o GA, so just return a bogus number
    return -9999;
  }

protected:

  boost::mpi::communicator p_comm;
};




#if GRIDPACK_HAVE_GA

// -------------------------------------------------------------
//  class CommunicatorPrivateGA
// -------------------------------------------------------------
class CommunicatorPrivateGA 
  : public CommunicatorPrivate
{
public:

  /// Default constructor.
  CommunicatorPrivateGA(void)
    : CommunicatorPrivate(),
      p_handle(GA_Pgroup_get_world())
  { }

  /// Construct on the processes in the specified communicators
  CommunicatorPrivateGA(const boost::mpi::communicator& comm)
    : CommunicatorPrivate(), p_handle()
  {
    int me(comm.rank()), nprocs(comm.size());
    int gaWrld = GA_Pgroup_get_world();
    int defGrp = GA_Pgroup_get_default();
    GA_Pgroup_set_default(gaWrld);
    std::vector<int> gaSrc(nprocs, 0), gaDest(nprocs, 0);
    gaSrc[me] = GA_Nodeid();
    boost::mpi::all_reduce(comm, &gaSrc[0], nprocs, &gaDest[0], std::plus<int>());
    p_handle = GA_Pgroup_create(&gaDest[0],nprocs);
    // GA_Pgroup_sync(p_handle);
    GA_Pgroup_set_default(defGrp);
  }

  /// Destructor
  ~CommunicatorPrivateGA(void)
  { 
    if (p_handle != GA_Pgroup_get_world()){
      GA_Pgroup_destroy(p_handle);
    }
  }

  /// Get GA process group handle
  int handle(void) const
  {
    return p_handle;
  }

  /// Sync GA process group
  void sync(void) const
  {
    GA_Pgroup_sync(p_handle);
  }    

  int worldRank(void) const
  {
    return GA::SERVICES.nodeid();
  }

protected:
  
  /// The GA process group handle
  int p_handle;

};

typedef CommunicatorPrivateGA TheCommunicatorPrivate;

#else

typedef CommunicatorPrivateMPI TheCommunicatorPrivate;

#endif

// -------------------------------------------------------------
//  class Communicator
// -------------------------------------------------------------

// -------------------------------------------------------------
// Communicator:: constructors / destructor
// -------------------------------------------------------------

Communicator::Communicator(void)
#if USE_PROGRESS_RANKS
  : p_comm(GA_MPI_Comm(),boost::mpi::comm_duplicate),
    p_private(new CommunicatorPrivateGA())
#else
  : p_comm(), p_private(new TheCommunicatorPrivate())
#endif
{}

Communicator::Communicator(const boost::mpi::communicator& comm)
  : p_comm(comm), p_private(new TheCommunicatorPrivate(p_comm))
{}


Communicator::Communicator(const MPI_Comm& comm)
  : p_comm(comm, boost::mpi::comm_duplicate),
    p_private(new TheCommunicatorPrivate(p_comm))
{}


Communicator::Communicator(const Communicator& old)
  : p_comm(old.p_comm), p_private(old.p_private)
{}

Communicator::~Communicator(void)
{
}

// -------------------------------------------------------------
// Communicator::swap
// -------------------------------------------------------------
void
Communicator::swap(Communicator& other)
{
  std::swap(p_comm, other.p_comm);
  std::swap(p_private, other.p_private);
}

// -------------------------------------------------------------
// Communicator::operator=
// -------------------------------------------------------------
Communicator & 
Communicator::operator= (const Communicator & rhs) 
{
  Communicator temp(rhs);
  this->swap(temp);
  return (*this);
}

// -------------------------------------------------------------
// Communicator::worldRank
// -------------------------------------------------------------
int 
Communicator::worldRank(void) const
{
  return p_private->worldRank();
}

// -------------------------------------------------------------
// Communicator::getGroup
// -------------------------------------------------------------
int
Communicator::getGroup(void) const
{
  int result(-9999);
  if (p_private) {
    result = p_private->handle();
  }
  return result;
}

// -------------------------------------------------------------
// Communicator::divide
// -------------------------------------------------------------
Communicator 
Communicator::divide(int nsize) const
{
  int nprocs(size());
  int me(rank());
  // find out how many communicators need to be created
  int ngrp = nprocs/nsize;
  if (ngrp*nsize < nprocs) ngrp++;
  // evaluate how many communicators are of size nsize
  int nlarge = nprocs;
  if (ngrp*nsize > nprocs) {
    int nsmall = ngrp*nsize - nprocs;
    nlarge = nprocs - nsmall;
  }
  nlarge = nlarge/nsize;
  if (nprocs - nlarge*nsize  > nsize) nlarge++;
  // figure out what color this process is and use split functionality to
  // create new communicators
  int i;
  int color = 0;
  for (i=0; i<=me; i++) {
    if (i<nlarge*nsize) {
      if (i%nsize == 0) color++;
    } else {
      if ((i-nlarge*nsize)%(nsize-1) == 0) color++;
    }
  }
  return this->split(color);
}

// -------------------------------------------------------------
// Communicator::sync
// -------------------------------------------------------------
void 
Communicator::sync(void) const
{
  if (p_private) {
    p_private->sync();
  } else {
    this->barrier();
  }
}    



} // namespace parallel
} // namespace gridpack
