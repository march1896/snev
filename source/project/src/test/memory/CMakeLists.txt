INCLUDE_DIRECTORIES(${TEST_INCLUDE_DIR})

LINK_DIRECTORIES(${CORE_LIB_OUTPUT_DIR})

add_executable(HeapOP_Usage HeapOP.Usage.cxx) 
target_link_libraries(HeapOP_Usage ${CORE_LIB})
set_target_properties(HeapOP_Usage PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CORE_LIB_OUTPUT_DIR})
