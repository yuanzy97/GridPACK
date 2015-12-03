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
# Last Change: 2015-12-02 15:29:33 d3g096
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
  COMPONENTS Teuchos Epetra EpetraExt Tpetra
)

if (Trilinos_FOUND)
  message(STATUS "Found Trilinos: ${Trilinos_DIR}")
endif (Trilinos_FOUND)

include_directories(${Teuchos_INCLUDE_DIRS})

# -------------------------------------------------------------
# hello
# A really stupid program.
# -------------------------------------------------------------
add_executable(hello hello.cpp)
target_link_libraries(hello 
  ${Trilinos_LIBRARIES} 
  ${MPI_CXX_LIBRARIES}
)