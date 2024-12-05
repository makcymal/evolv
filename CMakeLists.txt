cmake_minimum_required(VERSION 3.19.0)

project(evolv
  VERSION 0.1.0
  DESCRIPTION "Markov chain implementation"
  LANGUAGES CXX)

if (NOT CMAKE_BINARY_DIR STREQUAL ${CMAKE_SOURCE_DIR}/.cmake)
  message(FATAL_ERROR "Run \"cmake -S . -B .cmake\"")
endif()

set(CMAKE_CXX_FLAGS "-Wall")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../.build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../.build)

# add_library(main SHARED lab2/main.cc)

# add_executable(background lab2/background.cc)
