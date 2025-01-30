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

set(CMAKE_CXX_FLAGS "-Wall -w")

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)


file(GLOB
  evolv_IMPL
  CONFIGURE_DEPENDS
  "src/impl/*.h"
)

file(GLOB
  evolv_SRC
  "src/evolv.h"
  ${evolv_IMPL}
)


# tests executable target

find_package(GTest REQUIRED)

if(GTest_FOUND)
  include(ExternalProject)
  
  # ExternalProject_Add(
  #   dbg-cpp
  #   GIT_REPOSITORY https://github.com/makcymal/dbg-cpp.git
  #   GIT_TAG master
  #   SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/dbg
  #   CONFIGURE_COMMAND ""
  #   BUILD_COMMAND ""
  #   INSTALL_COMMAND ""
  #   LOG_DOWNLOAD ON
  # )

  file(GLOB
    instantiate_SRC
    "instantiate.h"
    "instantiate.cc"
  )

  add_executable(tests
    "tests/main.cc"
    ${evolv_SRC}
    ${instantiate_SRC}
  )
  target_include_directories(tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(tests PRIVATE GTest::GTest)
else()
  set_property(TARGET tests PROPERTY EXCLUDE_FROM_ALL TRUE)
endif()
