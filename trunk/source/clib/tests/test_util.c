#include <test_util.h>
#include <time.h>
void x() {
	clock_t start = clock();
	/*Do something*/
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
}

