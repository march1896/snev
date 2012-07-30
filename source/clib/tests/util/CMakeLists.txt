set (TJCLIB ${PROJECT_SOURCE_DIR}/../bin/lib/libSLIB.a)
add_executable(CLIST_TEST clist.test.c)
target_link_libraries(CLIST_TEST ${TJCLIB})
