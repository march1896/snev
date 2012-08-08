// #include <cntr.h>
// #include <test_util.h>
// #include <stdio.h>
// 
// typedef cntr (*pf_cntr_create)();
// static void add_find_remove(
// 	pf_cntr_create create
// 	) {
// 	cntr c = create();
// 	citer_dos(first, NULL);
// 	citer_dos(second, NULL);
// 	int i, j, x;
// 
// 	for (i = 0; i < 100; i ++) {
// 		cntr_add_back(c, (void*)i);
// 	}
// 
// 	j = cntr_find(c, (void*)10, first);
// 	dbg_assert(j != 0);
// 	cntr_citer_end(c, second);
// 
// 	cntr_remove(c, first, second);
// 
// 	x = cntr_size(c);
// 	dbg_assert(x == 10);
// 	x = 0;
// 	cntr_citer_begin(c, first);
// 	cntr_citer_end(c, second);
// 
// 	while (!citer_equal(first, second)){
// 		j = (int)citer_get_ref(first);
// 		dbg_assert(j == x);
// 
// 		x ++;
// 		citer_to_next(first);
// 	}
// 	dbg_assert(x == (int)citer_get_ref(first));
// 
// 	j = cntr_find(c, (void*)10, first);
// 	dbg_assert(j == 0);
// 
// 	cntr_citer_begin(c, first);
// 	cntr_citer_end(c, second);
// 
// 	cntr_remove(c, first, second);
// 
// 	dbg_assert(cntr_size(c) == 0);
// 
// 	cntr_destroy(c);
// }
// 
// static void linear_memory_test() {
// 	
// }
// 
// static void linear_correctness_test() {
// 	printf("add find remove test start\n");
// 	printf("list : \n");
// 	add_find_remove(cntr_create_as_list);
// 	printf("array : \n");
// 	add_find_remove(cntr_create_as_array);
// 	printf("add find remove test end\n\n");
// }
// 
// static void linear_performance_test() {
// 
// }
// 
// void cntr_linear_test() {
// 	do_test("cntr linear correctness", linear_correctness_test);
// 
// 	do_test("cntr linear performance", linear_performance_test);
// }
