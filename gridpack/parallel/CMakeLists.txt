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
# Created May  6, 2013 by William A. Perkins
# Last Change: 2016-11-11 11:25:03 d3g096
# -------------------------------------------------------------

add_library(gridpack_parallel 
  environment.cpp
  communicator.cpp
  distributed.cpp
  index_hash.cpp
  random.cpp
)

target_link_libraries(gridpack_parallel
  ${Boost_LIBRARIES} 
  ${GA_LIBRARIES} 
  ${MPI_CXX_LIBRARIES}
)
  
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})
set(target_libraries
  ${Boost_LIBRARIES} ${MPI_CXX_LIBRARIES}
)

if (GA_FOUND)
  include_directories(BEFORE ${GA_INCLUDE_DIRS})
  list(INSERT target_libraries 0 ${GA_LIBRARIES})
endif()

# -------------------------------------------------------------
# gridpack header installation
# -------------------------------------------------------------
install(FILES
  environment.hpp
  communicator.hpp
  parallel.hpp
  distributed.hpp
  shuffler.hpp
  ga_shuffler.hpp
  printit.hpp
  task_manager.hpp
  random.hpp
  index_hash.hpp
  global_store.hpp
  DESTINATION include/gridpack/parallel
)

# -------------------------------------------------------------
# gridpack parallel library installation
# -------------------------------------------------------------
install(TARGETS 
  gridpack_parallel
  DESTINATION lib
)


# -------------------------------------------------------------
# TEST: greetings
# A simple MPI program (that does not use boost::test)
# -------------------------------------------------------------
add_executable(greetings test/greetings.cpp)
target_link_libraries(greetings gridpack_parallel ${target_libraries})

if (NOT USE_PROGRESS_RANKS)
  add_test(greetings_serial greetings)
  set_tests_properties(greetings_serial
    PROPERTIES 
    PASS_REGULAR_EXPRESSION "I am process 0 of [1-9].*$"
    )
endif()

add_test(NAME greetings_parallel 
  COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 4 ${MPIEXEC_PREFLAGS} $<TARGET_FILE:greetings> ${MPIEXEC_POSTFLAGS}
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
set_tests_properties(greetings_parallel
  PROPERTIES 
  PASS_REGULAR_EXPRESSION "I am process 0 of [1-9].*$"
)

if (GA_FOUND)
  # -------------------------------------------------------------
  # TEST: task_test
  # A simple program to test the task manager
  # -------------------------------------------------------------
  add_executable(task_test 
    test/task_test.cpp 
    ../../timer/local_timer.cpp
  )
  target_link_libraries(task_test 
    gridpack_parallel 
    ${target_libraries}
  )
  gridpack_add_run_test(task_test task_test "")
endif(GA_FOUND)

# -------------------------------------------------------------
# TEST: mpi_test
# A simple MPI test using boost::test
# -------------------------------------------------------------
add_executable(mpi_test test/mpi_test.cpp)
target_link_libraries(mpi_test 
  ${target_libraries}
)
gridpack_add_unit_test(mpi_test mpi_test)

# -------------------------------------------------------------
# TEST: shuffle_test
# -------------------------------------------------------------
add_executable(shuffle_test test/shuffle_test.cpp)
target_link_libraries(shuffle_test 
  gridpack_parallel 
  ${target_libraries}
)
gridpack_add_unit_test(shuffle shuffle_test)

# -------------------------------------------------------------
# TEST: hash_test
# A simple program to test the task manager
# -------------------------------------------------------------
add_executable(hash_test test/hash_test.cpp)
target_link_libraries(hash_test 
  gridpack_parallel 
  gridpack_timer
  ${target_libraries}
)
gridpack_add_unit_test(hash_test hash_test)

# -------------------------------------------------------------
# TEST: random_test
# A simple program to test the random number generator
# -------------------------------------------------------------
add_executable(random_test test/random_test.cpp)
target_link_libraries(random_test 
    gridpack_parallel 
    ${target_libraries}
)
gridpack_add_run_test(random_test random_test "")

# -------------------------------------------------------------
# TEST: reduce_test
# Test the various all_reduce member functions of Communicator
# -------------------------------------------------------------
add_executable(reduce_test test/reduce_test.cpp)
target_link_libraries(reduce_test 
  gridpack_parallel 
  ${target_libraries}
)
gridpack_add_run_test(reduce_test reduce_test "")

if (GA_FOUND)
# -------------------------------------------------------------
# TEST: store_test
# A simple program to test the global store module
# -------------------------------------------------------------
add_executable(store_test test/store_test.cpp)
target_link_libraries(store_test gridpack_parallel 
  ${target_libraries})

gridpack_add_run_test(store_test store_test "")
endif (GA_FOUND)

