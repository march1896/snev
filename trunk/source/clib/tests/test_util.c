#include <test_util.h>
#include <time.h>
#include <stdlib.h>
#include <cntr.h>

int *x, *y;

void init_data() {
	unsigned int iseed = (unsigned int)time(NULL);
	srand (iseed);

	x = (int*)malloc(sizeof(int)*plength);
	y = (int*)malloc(sizeof(int)*plength);
}

void destroy_data() {
	free(x);
	free(y);
}

/*
 * x contains the generate data
 * y contains the sorted and unified data.
 * return the sorted and unified data size.
 */
int generate_data(int type, int length) {
	int i;
	if (type == 0) {
		// increase
		for (i = 0; i < length; i ++) {
			x[i] = y[i] = i + 1;
		}
		return length;
	}
	else if (type == 1) {
		for (i = 0; i < length; i ++) {
			x[i] = length - i;
			y[i] = i + 1;
		}
		return length;
	}
	else if (type == 2) {
		int i, j;
		for (i = 0; i < length; i ++) {
			x[i] = y[i] = (rand() % length + length) % length + 1;
		}
		qsort(y, length, sizeof(int), qsort_int_compare);

		for (i = 1, j = 0; i < length; i ++) {
			if (y[i] != y[j]) {
				y[++j] = y[i];
			}
		}
		return j + 1;
	}
	else {
		dbg_assert(false);
	}
	return -1;
}

int cntr_int_compare(const void* x, const void* y) {
	int a = (int)x;
	int b = (int)y;

	if (a < b) return -1;
	else if (a > b) return 1;

	return 0;
}

int qsort_int_compare(const void* x, const void* y) {
	int* a = (int*)x;
	int* b = (int*)y;

	if (*a < *b) return -1;
	else if (*a > *b) return 1;

	return 0;
}

