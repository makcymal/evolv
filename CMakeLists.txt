cmake_minimum_required(VERSION 3.30)


# CMake runtime

if(NOT CMAKE_BINARY_DIR STREQUAL ${CMAKE_SOURCE_DIR}/.cmake)
  message(FATAL_ERROR "Run \"cmake -B .cmake\"")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../.build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../.build)


# Setting up project

project(evolv
  VERSION 0.1.0
  DESCRIPTION "Markov chains in C++ and Python"
  LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


# Setting up C++

set(CMAKE_CXX_FLAGS "-Wall -Werror")

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)


# test executable target

file(GLOB
  evolv_IMPL
  CONFIGURE_DEPENDS
  "src/impl/*.h"
)

add_executable(test
  "test/main.cc"
  "src/evolv.h"
  ${evolv_IMPL}
)

target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(test PRIVATE GTest::GTest)
