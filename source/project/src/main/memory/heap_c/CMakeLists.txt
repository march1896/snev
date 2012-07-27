include_directories(${PROJECT_INCLUDE_DIR}/memory/heap_c)
include_directories(${PROJECT_INCLUDE_DIR}/util)

# foreach(ff ${SOURCE_FILES})
# MESSAGE("SINGLE: " ${ff})
# endforeach(ff)

set (HEAP_C_LIB HeapC)

FILE(GLOB C_FILES "*.c")
FILE(GLOB C_INCLUDES "${PROJECT_INCLUDE_DIR}/memory/heap_c/*.h")

add_library(${HEAP_C_LIB} ${C_FILES})
set_target_properties(${CORE_LIB} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CORE_LIB_OUTPUT_DIR})

install (TARGETS ${HEAP_C_LIB} DESTINATION "${PROJECT_SOURCE_DIR}/../bin/heapc/lib")
install (FILES ${C_INCLUDES} DESTINATION "${PROJECT_SOURCE_DIR}/../bin/heapc/include")
