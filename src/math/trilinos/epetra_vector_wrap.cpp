// -------------------------------------------------------------
/**
 * @file   epetra_vector_wrap.cpp
 * @author William A. Perkins
 * @date   2015-12-11 08:50:09 d3g096
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

#include <fstream>
#include <Epetra_MpiComm.h>
#include <Epetra_LocalMap.h>
#include <Epetra_Import.h>
#include <EpetraExt_VectorOut.h>
#include <EpetraExt_VectorIn.h>
#include "trilinos/epetra_vector_wrap.hpp"
#include "gridpack/utilities/null_deleter.hpp"
#include "gridpack/utilities/exception.hpp"
#include "implementation_visitor.hpp"

namespace gridpack {
namespace math {

// -------------------------------------------------------------
//  class EpetraVectorWrapper
// -------------------------------------------------------------

// -------------------------------------------------------------
// EpetraVectorWrapper::getCommunicator (static member)
// -------------------------------------------------------------
parallel::Communicator 
EpetraVectorWrapper::getCommunicator(const Epetra_Vector& v)
{
  const Epetra_Comm& comm(v.Comm());
  // FIXME: dynamic_cast? really? Bad programmer. 
  const Epetra_MpiComm& mpicomm(dynamic_cast<const Epetra_MpiComm&>(comm));
  parallel::Communicator result(mpicomm.Comm());
  return result;
}


// -------------------------------------------------------------
// EpetraVectorWrapper:: constructors / destructor
// -------------------------------------------------------------
EpetraVectorWrapper::EpetraVectorWrapper(const parallel::Communicator& comm,
                                         const TrilinosIndex& local_length)
  : ImplementationVisitable(), 
    p_minIndex(), p_maxIndex(),
    p_vector(), 
    p_vectorWrapped(false)
{
  Epetra_MpiComm ecomm(comm);
  Epetra_BlockMap emap(-1, local_length, 1, 0, ecomm);
  p_vector.reset(new Epetra_Vector(emap));
  p_minIndex = emap.MinMyGID();
  p_maxIndex = emap.MaxMyGID() + 1;
}

EpetraVectorWrapper::EpetraVectorWrapper(Epetra_Vector& vec, const bool& copyVec)
  : ImplementationVisitable(),
    p_minIndex(), p_maxIndex(),
    p_vector(), 
    p_vectorWrapped(!copyVec)
{
  if (p_vectorWrapped) {
    p_vector.reset(&vec, utility::null_deleter());
  } else {
    p_vector.reset(new Epetra_Vector(vec));
  }
  p_minIndex = p_vector->Map().MinMyGID();
  p_maxIndex = p_vector->Map().MaxMyGID() + 1;
}

EpetraVectorWrapper::EpetraVectorWrapper(const EpetraVectorWrapper& old)
  : ImplementationVisitable(old), 
    p_minIndex(old.p_minIndex), p_maxIndex(old.p_maxIndex),
    p_vector(), 
    p_vectorWrapped(old.p_vectorWrapped)
{
  if (p_vectorWrapped) {
    p_vector = old.p_vector;
  } else {
    p_vector.reset(new Epetra_Vector(*old.p_vector));
  }
}

EpetraVectorWrapper::~EpetraVectorWrapper(void)
{
}

// -------------------------------------------------------------
// EpetraVectorWrapper::size
// -------------------------------------------------------------
TrilinosIndex 
EpetraVectorWrapper::size(void) const
{
  return p_vector->GlobalLength();
}

// -------------------------------------------------------------
// EpetraVectorWrapper::localSize
// -------------------------------------------------------------
TrilinosIndex
EpetraVectorWrapper::localSize(void) const
{
  return p_vector->MyLength();
}
  
// -------------------------------------------------------------
// EpetraVectorWrapper::localIndexRange
// -------------------------------------------------------------
void
EpetraVectorWrapper::localIndexRange(TrilinosIndex& lo, TrilinosIndex& hi) const
{
  lo = p_minIndex;
  hi = p_maxIndex;
}

// -------------------------------------------------------------
// EpetraVectorWrapper::getAllElements
// -------------------------------------------------------------
void 
EpetraVectorWrapper::getAllElements(TrilinosScalar *x) const
{
  const Epetra_BlockMap& gmap(p_vector->Map());
  const Epetra_Comm& ecomm(gmap.Comm());
  if (ecomm.NumProc() > 1) {
    Epetra_LocalMap lmap(this->size(), 0, ecomm);
    Epetra_Vector lvect(lmap);
    Epetra_Import importer(lmap, gmap); // FIXME: consider caching this
    lvect.Import(*p_vector, importer, Insert);
    lvect.ExtractCopy(x);
  } else {
    p_vector->ExtractCopy(x);
  }
}

// -------------------------------------------------------------
// EpetraVectorWrapper::zero
// -------------------------------------------------------------
void
EpetraVectorWrapper::zero(void)
{
  p_vector->PutScalar(0.0);
}

// -------------------------------------------------------------
// EpetraVectorWrapper::norm1
// -------------------------------------------------------------
double
EpetraVectorWrapper::norm1(void) const
{
  TrilinosScalar thenorm;
  if (!p_vector->Norm1(&thenorm)) {
    throw gridpack::Exception("error computing Norm1 of Epetra_Vector");
  }
  return thenorm;
}

// -------------------------------------------------------------
// EpetraVectorWrapper::norm2
// -------------------------------------------------------------
double
EpetraVectorWrapper::norm2(void) const
{
  TrilinosScalar thenorm;
  if (!p_vector->Norm2(&thenorm)) {
    throw gridpack::Exception("error computing Norm2 of Epetra_Vector");
  }
  return thenorm;
}

// -------------------------------------------------------------
// EpetraVectorWrapper::normInfinity
// -------------------------------------------------------------
double
EpetraVectorWrapper::normInfinity(void) const
{
  TrilinosScalar thenorm;
  if (!p_vector->NormInf(&thenorm)) {
    throw gridpack::Exception("error computing Norminf of Epetra_Vector");
  }
  return thenorm;
}

// -------------------------------------------------------------
// EpetraVectorWrapper::abs
// -------------------------------------------------------------
void
EpetraVectorWrapper::abs(void)
{
  p_vector->Abs(*p_vector);
}

// -------------------------------------------------------------
// EpetraVectorWrapper::reciprocal
// -------------------------------------------------------------
void
EpetraVectorWrapper::reciprocal(void)
{
  p_vector->Reciprocal(*p_vector);
}

// -------------------------------------------------------------
// EpetraVectorWrapper::ready
// -------------------------------------------------------------
void
EpetraVectorWrapper::ready(void)
{
  // do nothing
}

// -------------------------------------------------------------
// EpetraVectorWrapper::print
// -------------------------------------------------------------
void
EpetraVectorWrapper::print(const char* filename) const
{
  if (filename) {
    std::ofstream out(filename);
    p_vector->Print(out);
    out.close();
  } else {
    p_vector->Print(std::cout);
  }
};

// -------------------------------------------------------------
// EpetraVectorWrapper::save
// -------------------------------------------------------------
void
EpetraVectorWrapper::save(const char* filename) const
{
  int ierr(0);
  ierr = EpetraExt::VectorToMatlabFile(filename, *p_vector);
  if (ierr) {
    throw Exception("EpetraExt::VectorToMatlabFile failure");
  }
}

// -------------------------------------------------------------
// EpetraVectorWrapper::loadBinary
// -------------------------------------------------------------
void
EpetraVectorWrapper::loadBinary(const char* filename)
{
  int ierr(0);
  Epetra_Vector *in;
  ierr = EpetraExt::MatrixMarketFileToVector(filename, p_vector->Map(), in);
  *p_vector = *in;
  if (ierr) {
    throw gridpack::Exception("EpetraVectorWrapper::loadBinary failed");
  }
}

// -------------------------------------------------------------
// EpetraVectorWrapper::saveBinary
// -------------------------------------------------------------
void
EpetraVectorWrapper::saveBinary(const char* filename) const
{
  int ierr(0);
  ierr = EpetraExt::VectorToMatrixMarketFile(filename, *p_vector);
  if (ierr) {
    throw gridpack::Exception("EpetraVectorWrapper::saveBinary failed");
  }
}

// -------------------------------------------------------------
// EpetraVectorWrapper::p_accept
// -------------------------------------------------------------
void 
EpetraVectorWrapper::p_accept(ImplementationVisitor& visitor)
{
  visitor.visit(*this);
}

void 
EpetraVectorWrapper::p_accept(ConstImplementationVisitor& visitor) const
{
  visitor.visit(*this);
}

} // namespace math
} // namespace gridpack
