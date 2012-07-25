include_directories(${PROJECT_INCLUDE_DIR}/memory)
include_directories(${PROJECT_INCLUDE_DIR}/util)

foreach (dir ${SUB_MODULES})
	FILE(GLOB C_CPP_CXX_FILES "${dir}/*.cxx" "${dir}/*.cpp" "${dir}/*.c")

	LIST(APPEND SOURCE_FILES ${C_CPP_CXX_FILES})
endforeach(dir)

# foreach(ff ${SOURCE_FILES})
# MESSAGE("SINGLE: " ${ff})
# endforeach(ff)

add_library(${CORE_LIB} ${SOURCE_FILES})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CORE_LIB_OUTPUT_DIR})
set_target_properties(${CORE_LIB} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CORE_LIB_OUTPUT_DIR})
# set_target_properties(${CORE_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CORE_LIB_OUTPUT_DIR})
