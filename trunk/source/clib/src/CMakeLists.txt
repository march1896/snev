cmake_minimum_required (VERSION 2.6)

# This is Just a C Library
project (TJCLIB)

# The version number.
set (NONAME_VERSION_MAJOR 1)
set (NONAME_VERSION_MINOR 0)

# should we use our own math functions
option(SINGLE_LIB 
	"build an single library" ON)

option(BUILD_TEST
	"build test projects" ON)

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

		FILE(GLOB LOCAL_HEADER_FILES "${dir}/*.local.h" "${dir}/*.doc.h")
		LIST(LENGTH LOCAL_HEADER_FILES LOCAL_HEADER_LENGTH)
		IF(LOCAL_HEADER_LENGTH)
			LIST(REMOVE_ITEM HEADER_FILES ${LOCAL_HEADER_FILES})
		ENDIF(LOCAL_HEADER_LENGTH)

		LIST(APPEND TOTAL_HEADER_FILES ${HEADER_FILES})

	endforeach (dir)

	add_library(${LIB_NAME} ${TOTAL_SOURCE_FILES})

	install (TARGETS ${LIB_NAME} DESTINATION "${PROJECT_INSTALL_DIR}/lib")

	install (FILES ${TOTAL_HEADER_FILES} DESTINATION "${PROJECT_INSTALL_DIR}/include")

 	if (BUILD_TEST) 
		INCLUDE_DIRECTORIES("../tests/")
		FILE (GLOB_RECURSE C_TESTS_FILES "../tests/*.test.c")
		ADD_EXECUTABLE (TestMain ${C_TESTS_FILES})
		TARGET_LINK_LIBRARIES(TestMain ${LIB_NAME})

		#FOREACH (TESTFILE ${C_TESTS_FILES})
		#	GET_FILENAME_COMPONENT(TEST ${TESTFILE} NAME_WE)
		#	add_executable (${TEST} ${TESTFILE})
		#	target_link_libraries(${TEST} ${LIB_NAME})
		#ENDFOREACH (TESTFILE)
	endif (BUILD_TEST)

elseif (SINGLE_LIB)

	add_subdirectory(memheap)
	add_subdirectory(util)

endif (SINGLE_LIB)

