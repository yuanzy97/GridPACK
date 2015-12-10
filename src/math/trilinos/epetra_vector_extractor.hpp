// Emacs Mode Line: -*- Mode:c++;-*-
// -------------------------------------------------------------
/*
 *     Copyright (c) 2013 Battelle Memorial Institute
 *     Licensed under modified BSD License. A copy of this license can be found
 *     in the LICENSE file in the top level directory of this distribution.
 */
// -------------------------------------------------------------
/**
 * @file   epetra_vector_extractor.hpp
 * @author William A. Perkins
 * @date   2015-12-10 13:35:02 d3g096
 * 
 * @brief  
 * 
 * 
 */
// -------------------------------------------------------------

#ifndef _epetra_vector_extractor_hpp_
#define _epetra_vector_extractor_hpp_

#include "gridpack/utilities/uncopyable.hpp"
#include "vector.hpp"
#include "trilinos/epetra_vector_wrap.hpp"
#include "implementation_visitor.hpp"

namespace gridpack {
namespace math {

// -------------------------------------------------------------
//  class EpetraVectorExtractor
// -------------------------------------------------------------
class EpetraVectorExtractor 
  : public ImplementationVisitor
{
public:

  /// Default constructor.
  EpetraVectorExtractor(void)
    : ImplementationVisitor(), p_vector(NULL)
  { }

  /// Destructor
  ~EpetraVectorExtractor(void) {}

  /// Get the vector
  void visit(EpetraVectorWrapper& epetra_wrap)
  {
    p_vector = epetra_wrap.getVector();
  }

  Epetra_Vector *vector(void)
  {
    return p_vector;
  }

protected:

  /// Where the (non-const) vector goes if it's found
  Epetra_Vector *p_vector;

};


// -------------------------------------------------------------
//  class EpetraConstVectorExtractor
// -------------------------------------------------------------
class EpetraConstVectorExtractor 
  : public ConstImplementationVisitor
{
public:

  /// Default constructor.
  EpetraConstVectorExtractor(void)
    : ConstImplementationVisitor(), p_vector(NULL)
  { }

  /// Destructor
  ~EpetraConstVectorExtractor(void) {}

  /// Get the vector
  void visit(const EpetraVectorWrapper& epetra_wrap) 
  {
    p_vector = epetra_wrap.getVector();
  }

  const Epetra_Vector *vector(void)
  {
    return p_vector;
  }

protected:

  /// Where the (const) vector goes if it's found
  const Epetra_Vector *p_vector;

};

// -------------------------------------------------------------
// GetEpetraVector
// -------------------------------------------------------------
/// Get a Epetra vector from a Vector
template <typename T, typename I>
Epetra_Vector *
GetEpetraVector(VectorT<T, I>& A)
{
  Epetra_Vector *result(NULL);
  EpetraVectorExtractor extract;
  A.accept(extract);
  result = extract.vector();

  // a null pointer means the Vector was not implemented in Epetra -- a
  // programming error
  BOOST_ASSERT(result);

  return result;
}

/// Get a (const) Epetra vector from a Vector
template <typename T, typename I>
const Epetra_Vector *
GetEpetraVector(const VectorT<T, I>& A)
{
  const Epetra_Vector *result(NULL);
  EpetraConstVectorExtractor extract;
  A.accept(extract);
  result = extract.vector();

  // a null pointer means the Vector was not implemented in Epetra -- a
  // programming error
  BOOST_ASSERT(result);

  return result;
}


} // namespace math
} // namespace gridpack



#endif
