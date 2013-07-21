cmake_minimum_required (VERSION 2.6)

# This is Just a C Library
project (TJCLIB)

# The version number.
set (NONAME_VERSION_MAJOR 0)
set (NONAME_VERSION_MINOR 1)

# should we use our own math functions
option(SINGLE_LIB "build an single library" ON)

option(BUILD_TEST "build test projects" ON)

# the name of the lib, this is the output of the project
set(LIB_NAME TJCLIB)
set(PROJECT_INSTALL_DIR ${PROJECT_SOURCE_DIR}/../bin)

include_directories(.)

if (SINGLE_LIB)
	set (SUB_DIRS memheap util cntr regex)
	foreach (dir ${SUB_DIRS})
		include_directories(${dir})

		file(GLOB C_SOURCE_FILES "${dir}/*.c")

		list(APPEND TOTAL_SOURCE_FILES ${C_SOURCE_FILES})

		file(GLOB HEADER_FILES "${dir}/*.h")
		file(GLOB LOCAL_HEADER_FILES "${dir}/*.local.h" "${dir}/*.doc.h")

		list(LENGTH LOCAL_HEADER_FILES LOCAL_HEADER_LENGTH)
		if(LOCAL_HEADER_LENGTH)
			list(REMOVE_ITEM HEADER_FILES ${LOCAL_HEADER_FILES})
		endif(LOCAL_HEADER_LENGTH)

		list(APPEND TOTAL_HEADER_FILES ${HEADER_FILES})

	endforeach (dir)

	add_library(${LIB_NAME} ${TOTAL_SOURCE_FILES})

	install (TARGETS ${LIB_NAME} DESTINATION "${PROJECT_INSTALL_DIR}/lib")

	install (FILES ${TOTAL_HEADER_FILES} DESTINATION "${PROJECT_INSTALL_DIR}/include")

 	if (BUILD_TEST) 
		include_directories("../tests/")
		file (GLOB_RECURSE C_TESTS_FILES "../tests/*.c")
		add_executable (TestMain ${C_TESTS_FILES})
		target_link_libraries(TestMain ${LIB_NAME})

		#FOREACH (TESTFILE ${C_TESTS_FILES})
		#	GET_FILENAME_COMPONENT(TEST ${TESTFILE} NAME_WE)
		#	add_executable (${TEST} ${TESTFILE})
		#	target_link_libraries(${TEST} ${LIB_NAME})
		#ENDFOREACH (TESTFILE)
	endif (BUILD_TEST)

elseif (SINGLE_LIB)

	add_subdirectory(util)
	add_subdirectory(memheap)
	add_subdirectory(cntr)


endif (SINGLE_LIB)

