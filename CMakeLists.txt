project( zippp )
cmake_minimum_required( VERSION 3.8 )
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    #disable warning for override of GTEST_ROOT so that the path can be configured if needed
    if (POLICY CMP0074)
        cmake_policy(SET CMP0074 NEW)
    endif()
endif(MSVC)

find_package(GTest REQUIRED)

include_directories(include)
include_directories(${GTEST_INCLUDE_DIRS})


# Find the correct versions of gtest for different platforms
if(MSVC)
    # On windows there is different binaries for debug and release
    # They also require different flags when building
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(GTEST_LIBS "${GTEST_LIBRARY_DEBUG};${GTEST_MAIN_LIBRARY_DEBUG}")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    else()
        set(GTEST_LIBS "${GTEST_LIBRARY};${GTEST_MAIN_LIBRARY}")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    endif()
else()
    set(GTEST_LIBS "${GTEST_BOTH_LIBRARIES}")
endif(MSVC)

add_executable(zippptests tests/zip_test.cpp)
target_link_libraries(zippptests ${GTEST_LIBS} )

if(NOT MSVC)
    #Gtest requires pthread on Unix
    find_package(Threads REQUIRED)
    target_link_libraries(zippptests ${CMAKE_THREAD_LIBS_INIT})
endif()