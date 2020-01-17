project( zippp )
cmake_minimum_required( VERSION 3.8 )
set(CMAKE_CXX_STANDARD 17)

include_directories(include)
add_executable(zippp_debug tests/zip_test.cpp)