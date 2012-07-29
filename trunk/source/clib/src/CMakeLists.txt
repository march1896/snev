cmake_minimum_required (VERSION 2.6)

# This is Just a C Library
project (TJCLIB)

# The version number.
set (NONAME_VERSION_MAJOR 1)
set (NONAME_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
# configure_file (
# "${PROJECT_SOURCE_DIR}/cmake_config.h.in"
#"${PROJECT_BINARY_DIR}/TutorialConfig.h"
#)

# the name of the lib, this is the output of the project
set(LIB_NAME TJCLIB)
set(PROJECT_INSTALL_DIR ${PROJECT_SOURCE_DIR}/../bin)

include_directories(.)

add_subdirectory(memheap)
add_subdirectory(container)
add_subdirectory(tests)
