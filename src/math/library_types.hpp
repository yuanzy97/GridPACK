// Emacs Mode Line: -*- Mode:c++;-*-
// -------------------------------------------------------------
/**
 * @file   library_types.hpp
 * @author William A. Perkins
 * @date   2015-12-07 10:05:27 d3g096
 * 
 * @brief  Some generic tests for math library scalar types
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

#ifndef _library_types_hpp_
#define _library_types_hpp_

namespace gridpack {
namespace math {

/// A flag (type) to denote whether the library can be used
/**
 * Some operations can be passed directly to the underlying library if
 * the GridPACK scalar type is the same as the underlying library
 * scalar type @e or the library scalar type is complex.  This type
 * computes and stores that flag at compile time
 * 
 */
template <typename GridPACKScalar, typename LibraryScalar>
struct UseLibrary
  : public boost::mpl::bool_<
            boost::is_same<GridPACKScalar, LibraryScalar>::value ||
            boost::is_same<ComplexType, LibraryScalar>::value >::type
{
};

/// The number of library elements used to represent a single GridPACK vector element
template <typename GridPACKScalar, typename LibraryScalar>
struct ElementSize
  : public boost::mpl::if_< UseLibrary<GridPACKScalar, LibraryScalar>, 
                            boost::mpl::int_<1>, 
                            boost::mpl::int_<2> >::type
{
};


} // namespace math
} // namespace gridpack

#endif
