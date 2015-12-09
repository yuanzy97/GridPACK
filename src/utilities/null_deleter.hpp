// Emacs Mode Line: -*- Mode:c++;-*-
// -------------------------------------------------------------
/**
 * @file   null_deleter.hpp
 * @author William A. Perkins
 * @date   2015-12-09 09:20:11 d3g096
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

#ifndef _null_deleter_hpp_
#define _null_deleter_hpp_

namespace gridpack {
namespace utility {

/// A thing to provide a null delete operation for shared pointers
/**
 * This is used with @c boost::shared_ptr<> or @c std::shared_ptr<> when
 * they contain a raw pointer that should not be deleted when the
 * shared_ptr instance goes away.
 * 
 */
struct null_deleter {
  void operator()(void const *p) const {}
};


} // namespace utility
} // namespace gridpack
#endif
