include_directories(${PROJECT_SOURCE_DIR}/container)
include_directories(.)

# foreach(ff ${SOURCE_FILES})
# MESSAGE("SINGLE: " ${ff})
# endforeach(ff)

FILE(GLOB SOURCE_FILES "*.c")
add_executable(clist_test clist.test.c)
target_link_libraries(clist_test libTJCLIB_CNTR.a)
