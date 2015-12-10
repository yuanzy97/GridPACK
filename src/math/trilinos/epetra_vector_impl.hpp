// Emacs Mode Line: -*- Mode:c++;-*-
// -------------------------------------------------------------
/**
 * @file   epetra_vector_impl.hpp
 * @author William A. Perkins
 * @date   2015-12-10 12:52:14 d3g096
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


#ifndef _epetra_vector_impl_hpp_
#define _epetra_vector_impl_hpp_

#include "complex_operators.hpp"
#include "value_transfer.hpp"
#include "vector_implementation.hpp"
#include "trilinos/trilinos_types.hpp"
#include "trilinos/epetra_vector_wrap.hpp"

namespace gridpack {
namespace math {

// -------------------------------------------------------------
//  class EpetraVectorImplementation
// -------------------------------------------------------------

template <typename T, typename I = int>
class EpetraVectorImplementation 
  : public VectorImplementation<T, I>
{
public:

  typedef typename VectorImplementation<T, I>::IdxType IdxType;
  typedef typename VectorImplementation<T, I>::TheType TheType;

  /// A flag to denote whether the library can be used directly
  /**
   * Some operations can be passed directly to the underlying library
   * if the TheType is the same as the PETSc type @e or the PETSc type
   * is complex.  This type computes and stores that flag. 
   * 
   */
  static const bool useLibrary = UseTrilinosLibrary<TheType>::value;

  /// The number of library elements used to represent a single vector element
  static const unsigned int elementSize = TrilinosElementSize<TheType>::value;

  /// Default constructor.
  EpetraVectorImplementation(const parallel::Communicator& comm,
                             const IdxType& local_length)
    : VectorImplementation<T>(comm), p_vwrap(comm, local_length*elementSize)
  { }
  
  /// Construct from an existing Epetra vector
  EpetraVectorImplementation(Epetra_Vector& pvec, const bool& copyvec = true)
    : VectorImplementation<T>(EpetraVectorWrapper::getCommunicator(pvec)), 
      p_vwrap(pvec, copyvec)
  { }

  /// Destructor
  /** 
   * @e Collective
   * 
   */
  ~EpetraVectorImplementation(void) {}

protected:

  /// Where the vector is actually stored
  EpetraVectorWrapper p_vwrap;

  /// Apply a specific unary operation to the vector
  void p_applyOperation(base_unary_function<TheType>& op)
  {
    Epetra_Vector *v = p_vwrap.getVector();
    TrilinosIndex n(v->MyLength());
    TrilinosScalar *p;
    v->ExtractView(&p);
    unary_operation<TheType, TrilinosScalar>(static_cast<unsigned int>(n), 
                                             p, op);
    v->ResetView(&p);
  }

  /// Apply a specificy accumulator operation to the vector
  RealType p_applyAccumulator(base_accumulator_function<TheType, RealType>& op) const
  {
    RealType result(0.0);
    return result;
  }

  /// Get the global vector length
  IdxType p_size(void) const
  {
    return p_vwrap.size()/elementSize;
  }

  /// Get the size of the vector local part
  IdxType p_localSize(void) const
  {
    return p_vwrap.localSize()/elementSize;
  }

  /// Get the local min/max global indexes (specialized)
  void p_localIndexRange(IdxType& lo, IdxType& hi) const
  {
    TrilinosIndex plo, phi;
    p_vwrap.localIndexRange(plo, phi);
    lo = plo/elementSize;
    hi = phi/elementSize;
  }

  void p_setOrAddElement(const IdxType& i, const TheType& x, bool addem)
  {
    int ierr(0);
    if (useLibrary) {
      Epetra_Vector *v = p_vwrap.getVector();
      TrilinosScalar pv = equate<TrilinosScalar, TheType>(x);
      TrilinosIndex tidx(i);
      if (addem) {
        ierr = v->SumIntoGlobalValues(1, &pv, &tidx);
      } else {
        ierr = v->ReplaceGlobalValues(1, &pv, &tidx);
      }
      if (ierr) {
        throw Exception("Eptra_Vector::ReplaceGlobalValues failure");
      }
    } else {
      p_setOrAddElements(1, &i, &x, addem);
    }
  }

  /// Set an several elements (specialized)
  void p_setOrAddElements(const IdxType& n, const IdxType *i, const TheType *x, 
                          bool addem)
  {
    int ierr;
    Epetra_Vector *v = p_vwrap.getVector();
    const IdxType *theindexes;
    TrilinosIndex *idx = NULL;

    // try to avoid allocating an array for indexes: assumes that TrilinosIndex == IdxType (usually int)
    if (elementSize == 1) {
      theindexes = i;         
    } else {
      idx = new TrilinosIndex[n*elementSize];
      for (int j = 0; j < n; ++j) {
        idx[j*elementSize] = i[j]*elementSize;
        if (elementSize > 1) 
          idx[j*elementSize+1] = idx[j*elementSize]+1;
      }
      theindexes = idx;
    }
    ValueTransferToLibrary<TheType, TrilinosScalar> trans(n, const_cast<TheType *>(x));
    trans.go();
    if (addem) {
      ierr = v->SumIntoGlobalValues(n, trans.to(), theindexes);
    } else {
      ierr = v->ReplaceGlobalValues(n, trans.to(), theindexes);
    }
    if (ierr) {
      throw Exception("Eptra_Vector::Replace/SumIntoGlobalValues failure");
    }
    if (idx != NULL) {
      delete [] idx;
    }
  }


  /// Set an individual element (specialized)
  void p_setElement(const IdxType& i, const TheType& x)
  {
    p_setOrAddElement(i, x, false);
  }

  /// Set an several elements (specialized)
  void p_setElements(const IdxType& n, const IdxType *i, const TheType *x)
  {
    p_setOrAddElements(n, i, x, false);
  }

  /// Add to an individual element (specialized)
  void p_addElement(const IdxType& i, const TheType& x)
  {
    p_setOrAddElement(i, x, true);
  }

  /// Add to an several elements (specialized)
  void p_addElements(const IdxType& n, const IdxType *i, const TheType *x)
  {
    p_setOrAddElements(n, i, x, true);
  }

  /// Get an individual (local) element (specialized)
  void p_getElement(const IdxType& i, TheType& x) const
  {
    if (useLibrary) {
      const Epetra_Vector *v = p_vwrap.getVector();
      TrilinosScalar pv;
      TrilinosIndex tidx(i);
      pv = (*v)[tidx];
      x = equate<TheType, TrilinosScalar>(pv);
    } else {
      p_getElements(1, &i, &x);
    }
  }

  /// Get an several (local) elements (specialized)
  void p_getElements(const IdxType& n, const IdxType *i, TheType *x) const
  {
    const Epetra_Vector *v = p_vwrap.getVector();
    std::vector<TrilinosScalar> px(n*elementSize);
    TrilinosIndex idx;
    for (int j = 0; j < n; ++j) {
      idx = i[j]*elementSize;
      px[j*elementSize] = (*v)[idx];
      if (elementSize > 1) 
        px[j*elementSize+1] = (*v)[idx+1];
    }
    ValueTransferFromLibrary<TrilinosScalar, TheType> trans(n*elementSize, &px[0], x);
    trans.go();
  }

  /// Get all of vector elements (on all processes)
  void p_getAllElements(TheType *x) const
  {
    unsigned int n(p_vwrap.size());
    std::vector<TrilinosScalar> px(n);
    p_vwrap.getAllElements(&px[0]);
    ValueTransferFromLibrary<TrilinosScalar, TheType> trans(n, &px[0], x);
    trans.go();
  }

  /// Make all the elements zero (specialized)
  void p_zero(void)
  {
    p_vwrap.zero();
  }

  /// Make all the elements the specified value (specialized)
  void p_fill(const TheType& value)
  {
    int ierr(0);
    if (useLibrary) {
      Epetra_Vector *v = p_vwrap.getVector();
      TrilinosScalar pv = equate<TrilinosScalar, TheType>(value);
      ierr = v->PutScalar(pv);
      if (ierr) {
        throw Exception("Eptra_Vector::PutScalar failure");
      }
    } else {
      setvalue<TheType> op(value);
      p_applyOperation(op);
    }
  }  

  /// Scale all elements by a single value
  void p_scale(const TheType& x)
  {
    if (useLibrary) {
      Epetra_Vector *vec = p_vwrap.getVector();
      int ierr(0);
      TrilinosScalar px =
        gridpack::math::equate<TrilinosScalar, TheType>(x);
      ierr = vec->Scale(px);
      if (ierr) {
        throw Exception("Eptra_Vector::Scale failure");
      }
    } else {
      gridpack::math::multiplyvalue<TheType> op(x);
      p_applyOperation(op);
    } 
  }

  /// Compute the vector L1 norm (sum of absolute value) (specialized)
  double p_norm1(void) const
  {
    double result;
    if (useLibrary) {
      result = p_vwrap.norm1();
    } else {
      l1_norm<TheType> op;
      double lresult(p_applyAccumulator(op));
      boost::mpi::all_reduce(this->communicator(), lresult, result, std::plus<double>());
    }
    return result;
  }

  /// Compute the vector L2 norm (root of sum of squares) (specialized)
  double p_norm2(void) const
  {
    double result;
    if (useLibrary) {
      result = p_vwrap.norm2();
    } else {
      l2_norm<TheType> op;
      double lresult(p_applyAccumulator(op));
      boost::mpi::all_reduce(this->communicator(), lresult, result, std::plus<double>());
      result = sqrt(result);
    }
    return result;
  }

  /// Compute the vector infinity (or maximum) norm (specialized)
  double p_normInfinity(void) const
  {
    double result;
    if (useLibrary) {
      result = p_vwrap.normInfinity();
    } else {
      infinity_norm<TheType> op;
      double lresult(p_applyAccumulator(op));
      boost::mpi::all_reduce(this->communicator(), lresult, result, boost::mpi::maximum<double>());
    }
    return result;
  }

  /// Replace all elements with its absolute value (specialized) 
  void p_abs(void)
  {
    if (useLibrary) {
      p_vwrap.abs();
    } else {
      absvalue<TheType> op;
      p_applyOperation(op);
    }
  }

  /// Replace all elements with their complex conjugate
  void p_conjugate(void)
  {
    conjugate_value<TheType> op;
    p_applyOperation(op);
  }

  /// Replace all elements with its exponential (specialized)
  void p_exp(void)
  {
    exponential<TheType> op;
    p_applyOperation(op);
  }

  /// Replace all elements with its reciprocal (specialized)
  void p_reciprocal(void)
  {
    if (useLibrary) {
      p_vwrap.reciprocal();
    } else {
      reciprocal<TheType> op;
      p_applyOperation(op);
    }
  }

  /// Make this instance ready to use
  void p_ready(void)
  {
    p_vwrap.ready();
  }

  /// Allow visits by implemetation visitor
  void p_accept(ImplementationVisitor& visitor)
  {
    p_vwrap.accept(visitor);
  }

  /// Print to named file or standard output (specialized)
  void p_print(const char* filename = NULL) const 
  { 
    p_vwrap.print(filename); 
  }

  /// Save, in MatLAB format, to named file (collective) (specialized)
  void p_save(const char *filename) const 
  { 
    p_vwrap.save(filename); 
  }

  /// Load from a named file of whatever binary format the math library uses (specialized)
  void p_loadBinary(const char *filename) 
  { 
    p_vwrap.loadBinary(filename); 
  }

  /// Save to named file in whatever binary format the math library uses (specialized)
  void p_saveBinary(const char *filename) const 
  { 
    p_vwrap.saveBinary(filename); 
  }

  /// Allow visits by implemetation visitor
  void p_accept(ConstImplementationVisitor& visitor) const
  {
    p_vwrap.accept(visitor);
  }

  /// Make an exact replica of this instance (specialized)
  VectorImplementation<T> *p_clone(void) const
  {
    std::unique_ptr<Epetra_Vector> 
      v(new Epetra_Vector(*(p_vwrap.getVector())));

    EpetraVectorImplementation<T, I> *result = 
      new EpetraVectorImplementation<T>(*v, true);

    return result;
  }

};



} // namespace math
} // namespace gridpack

#endif
