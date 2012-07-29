cmake_minimum_required (VERSION 2.6)

# This is Just a C Library
project (TJCLIB)

# The version number.
set (NONAME_VERSION_MAJOR 1)
set (NONAME_VERSION_MINOR 0)

# should we use our own math functions
option(SINGLE_LIB 
  "build an single library" ON)

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

if (SINGLE_LIB)
	set (SUB_DIRS memheap util)
	foreach (dir ${SUB_DIRS})
		include_directories(${dir})

		FILE(GLOB C_SOURCE_FILES "${dir}/*.c")

		LIST(APPEND TOTAL_SOURCE_FILES ${C_SOURCE_FILES})

		FILE(GLOB HEADER_FILES "${dir}/*.h")

		LIST(APPEND TOTAL_HEADER_FILES ${HEADER_FILES})
	endforeach (dir)

	add_library(SLIB ${TOTAL_SOURCE_FILES})

	install (TARGETS SLIB DESTINATION "${PROJECT_INSTALL_DIR}/lib")
	install (FILES ${TOTAL_HEADER_FILES} DESTINATION "${PROJECT_INSTALL_DIR}/include")

elseif (SINGLE_LIB)

	add_subdirectory(memheap)
	add_subdirectory(util)
endif (SINGLE_LIB)

