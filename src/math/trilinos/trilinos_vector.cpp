// -------------------------------------------------------------
/**
 * @file   trilinos_vector.cpp
 * @author William A. Perkins
 * @date   2015-12-10 14:26:47 d3g096
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
#include "vector.hpp"
#include "complex_operators.hpp"
#include "trilinos/epetra_vector_impl.hpp"
#include "trilinos/epetra_vector_extractor.hpp"


namespace gridpack {
namespace math {

// -------------------------------------------------------------
// applyBinaryOperator
// -------------------------------------------------------------
template <typename T>
static void
applyBinaryOperator(Epetra_Vector *v1, const Epetra_Vector *v2, 
                    base_binary_function<T>& op)
{
  TrilinosIndex n1, n2;
  n1 = v1->MyLength();
  n2 = v2->MyLength();
  BOOST_ASSERT(n1 == n2);

  TrilinosScalar *x1(v1->Values());
  TrilinosScalar *x2(v2->Values());
  binary_operation<T, TrilinosScalar>(n1, x1, x2, op);
}

// -------------------------------------------------------------
// applyBinaryOperator
// -------------------------------------------------------------
template <typename T>
static void
applyUnaryOperator(Epetra_Vector *v, base_unary_function<T>& op)
{
  TrilinosIndex n;
  n = v->MyLength();
  TrilinosScalar *x(v->Values());
  unary_operation<T, TrilinosScalar>(n, x, op);
}


// -------------------------------------------------------------
//  class VectorT
// -------------------------------------------------------------

// -------------------------------------------------------------
// VectorT:: constructors / destructor
// -------------------------------------------------------------
template <typename T, typename I>
VectorT<T, I>::VectorT(const parallel::Communicator& comm, const int& local_length)
  :  parallel::WrappedDistributed(), utility::Uncopyable()
{
  EpetraVectorImplementation<T, I> *impl = 
    new EpetraVectorImplementation<T, I>(comm, local_length);
  p_vector_impl.reset(impl);
  p_setDistributed(impl);
}

template VectorT<ComplexType>::VectorT(const parallel::Communicator& comm, 
                                       const int& local_length);
template VectorT<RealType>::VectorT(const parallel::Communicator& comm, 
                                    const int& local_length);

// -------------------------------------------------------------
// VectorT::add
// -------------------------------------------------------------
template <typename T, typename I>
void
VectorT<T, I>::add(const VectorT<T, I>& x, const VectorT<T, I>::TheType& scale)
{
  this->p_checkCompatible(x);
  
  const Epetra_Vector *xvec(GetEpetraVector(x));
  Epetra_Vector *yvec(GetEpetraVector(*this));
  if (EpetraVectorImplementation<T, I>::useLibrary) {
    TrilinosScalar alpha =
      gridpack::math::equate<TrilinosScalar, TheType>(scale);
    TrilinosScalar beta(1.0);

    // This call computes y = beta*x + alpha*y. Where y is p_vector.  
    
    int ierr = yvec->Update(alpha, *xvec, beta);
  } else {
    scaleAdd2<T> op(scale);
    applyBinaryOperator<T>(yvec, xvec, op);
  }
}

template void VectorT<ComplexType>::add(const VectorT<ComplexType>& x, 
                                        const VectorT<ComplexType>::TheType& scale);
template void VectorT<RealType>::add(const VectorT<RealType>& x, 
                                     const VectorT<RealType>::TheType& scale);

template <typename T, typename I>
void
VectorT<T, I>::add(const VectorT<T, I>::TheType& x)
{
  gridpack::math::addvalue<TheType> op(x);
  this->p_vector_impl->applyOperation(op);
}

template void VectorT<ComplexType, int>::add(const VectorT<ComplexType>::TheType& x);
template void VectorT<double, int>::add(const VectorT<RealType>::TheType& x);


// -------------------------------------------------------------
// VectorT::equate
// -------------------------------------------------------------
/** 
 * Make this vector equal to another.  This works regardless of the
 * GridPACK element type (reall is added to real, imaginary is added
 * to imaginary).
 * 
 * @param x vector to copy
 * 
 * @return 
 */
template <typename T, typename I>
void
VectorT<T, I>::equate(const VectorT<T, I>& x)
{
  this->p_checkCompatible(x);
  Epetra_Vector *yvec(GetEpetraVector(*this));
  const Epetra_Vector *xvec(GetEpetraVector(x));
  *yvec = *xvec;
}

template void VectorT<ComplexType, int>::equate(const VectorT<ComplexType>& x);
template void VectorT<double, int>::equate(const VectorT<RealType>& x);

// -------------------------------------------------------------
// VectorT::elementMultiply
// -------------------------------------------------------------
template <typename T, typename I>
void
VectorT<T, I>::elementMultiply(const VectorT<T, I>& x)
{
  this->p_checkCompatible(x);
  Epetra_Vector *vec(GetEpetraVector(*this));
  const Epetra_Vector *xvec(GetEpetraVector(x));
  int ierr(0);
  multiplyvalue2<T> op;
  applyBinaryOperator<T>(vec, xvec, op);
}  

template void VectorT<ComplexType, int>::elementMultiply(const VectorT<ComplexType>& x);
template void VectorT<double, int>::elementMultiply(const VectorT<RealType>& x);

// -------------------------------------------------------------
// VectorT::elementDivide
// -------------------------------------------------------------
template <typename T, typename I>
void
VectorT<T, I>::elementDivide(const VectorT<T, I>& x)
{
  this->p_checkCompatible(x);
  Epetra_Vector *vec(GetEpetraVector(*this));
  const Epetra_Vector *xvec(GetEpetraVector(x));
  int ierr(0);
  dividevalue2<T> op;
  applyBinaryOperator<T>(vec, xvec, op);
}  

template void VectorT<ComplexType, int>::elementDivide(const VectorT<ComplexType> &x);
template void VectorT<double, int>::elementDivide(const VectorT<RealType> &x);


} // namespace math
} // namespace gridpack
