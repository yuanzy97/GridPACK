// Emacs Mode Line: -*- Mode:c++;-*-
// -------------------------------------------------------------
// file: petsc_types.hpp
// -------------------------------------------------------------
// -------------------------------------------------------------
// Battelle Memorial Institute
// Pacific Northwest Laboratory
// -------------------------------------------------------------
// -------------------------------------------------------------
// Created February 18, 2015 by William A. Perkins
// Last Change: 2015-12-07 10:07:35 d3g096
// -------------------------------------------------------------


#ifndef _petsc_types_hpp_
#define _petsc_types_hpp_


#include <petsc.h>
#include "library_types.hpp"

namespace gridpack {
namespace math {

/// A flag (type) to denote whether the library can be used
/**
 * Some operations can be passed directly to the underlying library
 * if the TheType is the same as the PETSc type @e or the PETSc type
 * is complex.  This type computes and stores that flag. 
 * 
 */
template <typename GridPACKScalar>
struct UsePetscLibrary : public UseLibrary<GridPACKScalar, PetscScalar>
{
};

/// The number of PETSc scalars used to represent a single GridPACK vector element
template <typename GridPACKScalar>
struct PetscElementSize
  : public boost::mpl::if_< UseLibrary<GridPACKScalar, PetscScalar>, 
                            boost::mpl::int_<1>, 
                            boost::mpl::int_<2> >::type
{
};



} // namespace math
} // namespace gridpack

#endif
