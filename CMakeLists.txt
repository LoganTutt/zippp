project(zippp)
cmake_minimum_required( VERSION 2.9 )
set(CMAKE_CXX_STANDARD 17)

# Download and unpack googletest at configure time
# Based on example at https://crascit.com/2015/07/25/cmake-gtest/
configure_file(GtestCMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# Download and unpack google benchmark at configure time
configure_file(BenchmarkCMakeLists.txt.in benchmark-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/benchmark-download )
if(result)
  message(FATAL_ERROR "CMake step for benchmark failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/benchmark-download )
if(result)
  message(FATAL_ERROR "Build step for benchmark failed: ${result}")
endif()
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/benchmark-src
                 ${CMAKE_CURRENT_BINARY_DIR}/benchmark-build
                 EXCLUDE_FROM_ALL)


# actual code for zipppp
include_directories(include)

add_executable(zippptests tests/zip_test.cpp)
target_link_libraries(zippptests gtest gtest_main )

add_executable(zipppbench benchmarks/zippp_benchmarks.cpp)
target_link_libraries(zipppbench benchmark::benchmark )