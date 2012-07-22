cmake_minimum_required (VERSION 2.6)
project (NoName)

# The version number.
set (NONAME_VERSION_MAJOR 1)
set (NONAME_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
# configure_file (
# "${PROJECT_SOURCE_DIR}/cmake_config.h.in"
#"${PROJECT_BINARY_DIR}/TutorialConfig.h"
#)

# *.h files are stored in project_include_dir
set (PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
# *.cxx files are stored in project_main_dir
set (PROJECT_MAIN_DIR "${PROJECT_SOURCE_DIR}/main")
# *.cxx test files are stored in project_test_dir
set (PROJECT_TEST_DIR "${PROJECT_SOURCE_DIR}/test")

# the name of the lib, this is the output of the project
set(CORE_LIB Core)

set(CORE_LIB_OUTPUT_DIR "${PROJECT_BINARY_DIR}/lib")

# the core lib is divided into small logical modules, called sub_modules
SET (SUB_MODULES memory util)

# TEST_INCLUDE_DIR is a set of all include files of the lib, for simplicity 
# test files that are built into runnable can this dir to the include directory
FOREACH (SUB_DIR ${SUB_MODULES})
	LIST(APPEND TEST_INCLUDE_DIR "${PROJECT_INCLUDE_DIR}/${SUB_DIR}")
ENDFOREACH(SUB_DIR)

add_subdirectory(${PROJECT_MAIN_DIR})
add_subdirectory(${PROJECT_TEST_DIR})
