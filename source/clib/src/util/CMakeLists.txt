# include_directories(${PROJECT_INCLUDE_DIR}/util)
include_directories(.)

# foreach(ff ${SOURCE_FILES})
# MESSAGE("SINGLE: " ${ff})
# endforeach(ff)

FILE(GLOB SOURCE_FILES "*.c")
add_library(${LIB_NAME} ${SOURCE_FILES})

FILE(GLOB C_INCLUDES "*.h")

install (TARGETS ${HEAP_C_LIB} DESTINATION "${PROJECT_INSTALL_DIR}/lib")
install (FILES ${C_INCLUDES} DESTINATION "${PROJECT_INSTALL_DIR}/include")
