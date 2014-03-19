#include <time.h>

#include "test_util.h"

extern void util_all_test();
extern void heap_all_test();
extern void cntr2_all_test();
/*extern void cntr_all_test();*/
extern void cls_builtintype_test();

#include <stdio.h>
int main() {
 	unsigned int iseed = (unsigned int)time(NULL);
 	srand (iseed);
	//printf("%d\n", iseed);

#if !defined(_DEBUG) && !defined(DEBUG) 
	g_use_file = true;
#endif

	test_run_single("oop all test", cls_builtintype_test);
	//test_run_single("util all test", util_all_test);
	//test_run_single("heap all test", heap_all_test);

	test_run_single("cntr2 all test", cntr2_all_test);

	/* deprecated */
	//test_run_single("cntr all test", cntr_all_test);
}
