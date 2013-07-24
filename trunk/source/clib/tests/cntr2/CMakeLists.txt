set (TJCLIB ${PROJECT_SOURCE_DIR}/../bin/lib/SLIB)
add_executable(CLIST_TEST clist.test.c)
target_link_libraries(CLIST_TEST ${TJCLIB})
