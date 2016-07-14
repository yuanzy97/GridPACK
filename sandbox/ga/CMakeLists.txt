#
#     Copyright (c) 2013 Battelle Memorial Institute
#     Licensed under modified BSD License. A copy of this license can be
#     found
#     in the LICENSE file in the top level directory of this distribution.
#
# -*- mode: cmake -*-
# -------------------------------------------------------------
# file: CMakeLists.txt
# -------------------------------------------------------------
# -------------------------------------------------------------
# Created July  7, 2016 by William A. Perkins
# Last Change: 2016-07-07 07:19:32 d3g096
# -------------------------------------------------------------

cmake_minimum_required (VERSION 2.8.8)
project (GATest CXX)

# where to look for special .cmake files
list (APPEND CMAKE_MODULE_PATH 
    "${GATest_SOURCE_DIR}"
)

# -------------------------------------------------------------
# MPI compiler
# -------------------------------------------------------------
message(STATUS "Checking MPI ...")
find_package (MPI REQUIRED)
include_directories(AFTER ${MPI_INCLUDE_PATH})
message(STATUS "MPI_CXX_LIBRARIES: ${MPI_CXX_LIBRARIES}")

# -------------------------------------------------------------
# Boost
#
# Find the Boost library. 
# -------------------------------------------------------------
message(STATUS "Checking Boost ...")
find_package(Boost 1.52 REQUIRED)
include_directories(BEFORE ${Boost_INCLUDE_DIR})

# -------------------------------------------------------------
# GlobalArrays
# 
# Only versions > 5.2 have been used
# -------------------------------------------------------------
option (GA_USE_PROGRESS_RANKS "Enable progress ranks in GridPACK" OFF)
message(STATUS "Checking GA ...")
find_package(GA REQUIRED)

if (GA_USE_PROGRESS_RANKS)
  add_definitions (-DUSE_PROGRESS_RANKS=1)
endif(GA_USE_PROGRESS_RANKS)
include_directories(AFTER ${GA_INCLUDE_DIRS})

# -------------------------------------------------------------
# hello
# A really simple test
# -------------------------------------------------------------
add_executable(hello hello.cpp)
target_link_libraries(hello ${GA_LIBRARIES} ${MPI_CXX_LIBRARIES})

# -------------------------------------------------------------
# string_test
# Another really simple test
# -------------------------------------------------------------
add_executable(string_test string_test.cpp)
target_link_libraries(string_test ${GA_LIBRARIES} ${MPI_CXX_LIBRARIES})

# -------------------------------------------------------------
# global_counter
# Exercises read/increment of global array elements
# -------------------------------------------------------------
add_executable(global_counter global_counter.cpp)
target_link_libraries(global_counter ${GA_LIBRARIES} ${MPI_CXX_LIBRARIES})

# -------------------------------------------------------------
# gather
# Exerecises gather repeatedly
# -------------------------------------------------------------
add_executable(gather gather.cpp)
target_link_libraries(gather ${GA_LIBRARIES} ${MPI_CXX_LIBRARIES})
