// Emacs Mode Line: -*- Mode:c++;-*-
// -------------------------------------------------------------
/**
 * @file   epetra_vector_wrap.hpp
 * @author William A. Perkins
 * @date   2015-12-09 10:13:01 d3g096
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

#ifndef _epetra_vector_wrap_hpp_
#define _epetra_vector_wrap_hpp_

#include <memory> // NOTE: This is OK, because Trilinos requires C++-11
#include <Epetra_Vector.h>
#include "parallel/communicator.hpp"
#include "implementation_visitable.hpp"
#include "trilinos/trilinos_types.hpp"

namespace gridpack {
namespace math {

// -------------------------------------------------------------
//  class EpetraVectorWrapper
// -------------------------------------------------------------
/**
 * This provides a wraper around Trilinos' Epetra vector class.
 * Functionality is exposed that does not directly involve the
 * (GridPACK) vector element type.
 * 
 */
class EpetraVectorWrapper 
  : public ImplementationVisitable
{
public:

  /// The index type used by 

  /// Default constructor.
  EpetraVectorWrapper(const parallel::Communicator& comm,
                      const TrilinosIndex& local_length);

  /// Construct with an existing Epetra_Vector instance
  EpetraVectorWrapper(Epetra_Vector& vec, const bool& copyVec);

  /// Copy constructor
  EpetraVectorWrapper(const EpetraVectorWrapper& old);

  /// Destructor
  ~EpetraVectorWrapper(void);

  /// Build a parallel::Communicator from the Vec instance
  parallel::Communicator getCommunicator(void) const;
 
  /// Get (a pointer to) the PETSc implementation
  const Epetra_Vector *getVector(void) const
  {
    return p_vector.get();
  }

  /// Get (a pointer to) the PETSc implementation
  Epetra_Vector *getVector(void)
  {
    return p_vector.get();
  }

  /// Get the global length
  /** 
   * 
   * @return global vector length
   */
  TrilinosIndex size(void) const;

  /// Get the local length
  /** 
   * 
   * @return local vector length
   */
  TrilinosIndex localSize(void) const;

  /// Get the local min/max global indexes
  /** 
   * @e Local.
   * 
   * The minimum index in the first global (0-based) index owned by
   * the local process. The maximum is one more than the last global
   * index owned. An example of usage:
   * 
     \code{.cpp}
     int lo, hi;
     my_vector.local_index_range(lo, hi);
     for (int i = lo; i < hi; ++i) {
       ComplexType x;
       x = ...;
       v.setElement(i, x);
     }
     \endcode
   * 
   * @param lo first (0-based) index of locally owned elements
   * @param hi one more than the last (0-based) index of locally owned elements
   */
  void localIndexRange(TrilinosIndex& lo, TrilinosIndex& hi) const;

  /// Get all elements in the vector
  void getAllElements(TrilinosScalar *x) const;

  /// Make all the elements zero
  void zero(void);

  /// Compute the vector L1 norm (sum of absolute value)
  double norm1(void) const;

  /// Compute the vector L2 norm (root of sum of squares)
  double norm2(void) const;

  /// Compute the infinity (or maximum) norm
  double normInfinity(void) const;

  /// Replace all elements with its absolute value (complex magnitude) 
  void abs(void);

  /// Replace all elements with its reciprocal
  void reciprocal(void);

  /// Print to named file or standard output
  void print(const char* filename = NULL) const;

  /// Save, in MatLAB format, to named file (collective)
  void save(const char *filename) const;

  /// Load from a named file of whatever binary format the math library uses
  void loadBinary(const char *filename);

  /// Save to named file in whatever binary format the math library uses
  void saveBinary(const char *filename) const;

protected:

  /// The (GridPACK) communicator
  parallel::Communicator p_comm;

  /// Minimum global index on this processor
  TrilinosIndex p_minIndex;

  /// Maximum global index on this processor
  TrilinosIndex p_maxIndex;

  /// The PETSc representation
  std::shared_ptr<Epetra_Vector> p_vector;

  /// Was @c p_vector created or just wrapped
  bool p_vectorWrapped;

  /// Allow visits by implemetation visitor
  void p_accept(ImplementationVisitor& visitor);

  /// Allow visits by implemetation visitor
  void p_accept(ConstImplementationVisitor& visitor) const;

};

} // namespace math
} // namespace gridpack

#endif
