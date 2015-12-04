# -------------------------------------------------------------
# file: CMakeLists.txt
# -------------------------------------------------------------
# -------------------------------------------------------------
# Copyright (c) 2013 Battelle Memorial Institute
# Licensed under modified BSD License. A copy of this license can be found
# in the LICENSE file in the top level directory of this distribution.
# -------------------------------------------------------------
# -------------------------------------------------------------
# Created December  2, 2015 by William A. Perkins
# Last Change: 2015-12-03 15:11:38 d3g096
# -------------------------------------------------------------

cmake_minimum_required (VERSION 2.8.8)
project (TrilinosSand)

# This is a C++ project exclusively
enable_language(CXX)

# -------------------------------------------------------------
# MPI compiler
# -------------------------------------------------------------
message(STATUS "Checking MPI ...")
find_package (MPI REQUIRED)
include_directories(AFTER ${MPI_INCLUDE_PATH})

message(STATUS "MPI_CXX_LIBRARIES: ${MPI_CXX_LIBRARIES}")

# -------------------------------------------------------------
# Trilinos
# -------------------------------------------------------------
find_package(Trilinos 12.0 REQUIRED
  HINTS ${Trilinos_DIR}
  COMPONENTS Teuchos Epetra EpetraExt Tpetra Thyra NOX AztecOO Rythmos
)

if (Trilinos_FOUND)
  message(STATUS "Found Trilinos ${Trilinos_VERSION}")
endif (Trilinos_FOUND)

include_directories(${Trilinos_INCLUDE_DIRS})

# need to gather whatever compiler flags were used for Trilinos and
# use them here
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Trilinos_CXX_COMPILER_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${Trilinos_C_COMPILER_FLAGS}")


# -------------------------------------------------------------
# hello
# A really stupid program.
# -------------------------------------------------------------
add_executable(hello hello.cpp)
target_link_libraries(hello 
  ${Trilinos_LIBRARIES} 
  ${MPI_CXX_LIBRARIES}
)

# -------------------------------------------------------------
# epetra_simple 
# From the Trilinos Epetra examples
# -------------------------------------------------------------
add_executable(epetra_simple epetra_simple.cpp)
target_link_libraries(epetra_simple
  ${Trilinos_LIBRARIES} 
  ${MPI_CXX_LIBRARIES}
)
