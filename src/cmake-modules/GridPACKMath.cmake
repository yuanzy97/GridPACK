# -*- mode: cmake -*-
# -------------------------------------------------------------
# Copyright (c) 2013 Battelle Memorial Institute
# Licensed under modified BSD License. A copy of this license can be found
# in the LICENSE file in the top level directory of this distribution.
# -------------------------------------------------------------
# -------------------------------------------------------------
# file: GridPACKMath.cmake
# macros to find the parallel math library used by GridPACK
# -------------------------------------------------------------
# -------------------------------------------------------------
# Created December  4, 2015 by William A. Perkins
# Last Change: 2015-12-04 11:46:42 d3g096
# -------------------------------------------------------------

# -------------------------------------------------------------
# gridpack_check_petsc
#
# Select PETSc as the underlying math library for GridPACK and make
# sure it has the necessary components. We need a very recent version
# of PETSc. It needs to parallel and compiled with C++ complex
# support.
# -------------------------------------------------------------
function(gridpack_check_petsc)

  message(STATUS "Checking PETSc ...")
  find_package (PETSc COMPONENTS CXX REQUIRED)

  if (EXISTS "${PETSC_DIR}/${PETSC_ARCH}/conf/PETScConfig.cmake")
    include("${PETSC_DIR}/${PETSC_ARCH}/conf/PETScConfig.cmake")
  elseif (EXISTS "${PETSC_DIR}/${PETSC_ARCH}/lib/petsc/conf/PETScConfig.cmake")
    include("${PETSC_DIR}/${PETSC_ARCH}/lib/petsc/conf/PETScConfig.cmake")
  else()
    message(FATAL_ERROR "PETSc found, but CMake configuration for PETSc installation not found?")
  endif()

  # checks 

  if (NOT PETSC_HAVE_MPI)
    message(FATAL_ERROR "PETSc installation is not parallel (--with-mpi=1)")
  endif()

  # PETSc can be compiled for double precsion (--with-precision=double)
  # complex (--with-scalar-type=complex) or real
  # (--with-scalar-type=real).  This is to determine what that
  # underlying type is.

  if (PETSC_USE_REAL_DOUBLE) 
    message(STATUS "PETSc installation is double precision (--with-precision=double) -- good")
  else()
    message(FATAL_ERROR "PETSc installation is not double precision (--with-precision=double)")
  endif()

  if (PETSC_USE_COMPLEX) 
    message (STATUS "PETSc installation uses complex type (--with-scalar-type=complex)")
  else()
    message (STATUS "PETSc installation uses real type (--with-scalar-type=real)")
  endif()

  # This is probably redundant
  if (NOT PETSC_CLANGUAGE_Cxx)
    message (FATAL_ERROR "PETSc installation does not use C++ (--with-clanguage=c++)")
  endif()

endfunction(gridpack_check_petsc)

# -------------------------------------------------------------
# gridpack_check_math
# -------------------------------------------------------------
function(gridpack_check_math)
  if (NOT GridPACK_MATH_LIBRARY) 
    set(GridPACK_MATH_LIBRARY "PETSC")
  else()
    string(TOUPPER ${GridPACK_MATH_LIBRARY} GridPACK_MATH_LIBRARY)
  endif()
  string(COMPARE EQUAL "${GridPACK_MATH_LIBRARY}" "PETSC" ispetsc)
  if (ispetsc)
    gridpack_check_petsc()
  else()
    message(FATAL_ERROR "Unknown math library name: ${GridPACK_MATH_LIBRARY}")
  endif()
endfunction(gridpack_check_math)