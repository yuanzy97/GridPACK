// Emacs Mode Line: -*- Mode:c++;-*-
// -------------------------------------------------------------
/**
 * @file   trilinos_types.hpp
 * @author William A. Perkins
 * @date   2015-12-09 10:15:04 d3g096
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

#include "library_types.hpp"

#ifndef _trilinos_types_hpp_
#define _trilinos_types_hpp_

namespace gridpack {
namespace math {


/// The Trilinos scalar type
/**
 * The GridPACK/Trilinos math interface based on the Epetra package.
 * The Matrix/Vector scalar type is fixed as double.
 * 
 */
typedef double TrilinosScalar;

/// The Trilinos Matrix/Vector element index type
/**
 * The GridPACK/Trilinos math interface based on the Epetra
 * package. Index type can be either int or long long.  The int type
 * is used (consistent with PETSc).
 * 
 */
typedef int TrilinosIndex;

/// A flag (type) to denote whether the library can be used
/**
 * Some operations can be passed directly to the underlying library if
 * the GridPACK scalar type is the same as the Trilinos type.  This
 * type computes and stores that flag.
 * 
 */
template <typename GridPACKScalar>
struct UseTrilinosLibrary : public UseLibrary<GridPACKScalar, TrilinosScalar>
{};

/// The number of library elements used to represent a single vector element
template <typename GridPACKScalar>
struct TrilinosElementSize: public ElementSize<GridPACKScalar, TrilinosScalar>
{
};

} // namespace math
} // namespace gridpack

#endif
