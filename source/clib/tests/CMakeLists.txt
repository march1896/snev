cmake_minimum_required (VERSION 2.6)

# This is Just a C Library
project (TJCLIB_TESTS)

set (TJCLIB_DIR ${PROJECT_SOURCE_DIR}/../bin/lib)
set (TJCLIB_INCLUDE ${PROJECT_SOURCE_DIR}/../bin/include)

include_directories(${TJCLIB_INCLUDE})

add_subdirectory(util)
