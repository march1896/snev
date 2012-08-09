#include <test_util.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <cntr.h>

int *rawdata, *uniquedata, *uniquesorteddata;

void init_data() {
	unsigned int iseed = (unsigned int)time(NULL);
	srand (iseed);

	rawdata = (int*)malloc(sizeof(int)*plength);
	uniquedata = (int*)malloc(sizeof(int)*plength);
	uniquesorteddata = (int*)malloc(sizeof(int)*plength);
}

void destroy_data() {
	free(rawdata);
	free(uniquedata);
	free(uniquesorteddata);
}

/*
 * x contains the generate data
 * y contains the sorted and unified data.
 * return the sorted and unified data size.
 */
static bool binary_search(int *buf, int size, int value);
int generate_data(int type, int length) {
	int i;
	if (type == 0) {
		// increase
		for (i = 0; i < length; i ++) {
			rawdata[i] = uniquedata[i] = uniquesorteddata[i] = i + 1;
		}
		return length;
	}
	else if (type == 1) {
		for (i = 0; i < length; i ++) {
			rawdata[i] = uniquedata[i] = length - i;
			uniquesorteddata[i] = i + 1;
		}
		return length;
	}
	else if (type == 2) {
		int i, j, k, *helper;
		for (i = 0; i < length; i ++) {
			
			rawdata[i] = uniquedata[i] = uniquesorteddata[i] = (rand() % length + length) % length + 1;
		}
		qsort(uniquesorteddata, length, sizeof(int), qsort_int_compare);

		for (i = 1, j = 0; i < length; i ++) {
			if (uniquesorteddata[i] != uniquesorteddata[j]) {
				uniquesorteddata[++j] = uniquesorteddata[i];
			}
		}

		helper = (int*)malloc(sizeof(int) * (length + 10));
		memset(helper, 0, sizeof(int) * (length + 10));
		for (i = 0, k = 0; i < length; i ++) {
			if (helper[uniquedata[i]] == 0) {
				helper[uniquedata[i]] = 1;
				uniquedata[k++] = uniquedata[i];
			}
		}
		free(helper);

		dbg_assert(k == j + 1);
		return j + 1;
	}
	else {
		dbg_assert(false);
	}
	return -1;
}

static bool binary_search(int *buf, int size, int value) {
	int mid = size / 2;

	if (size == 0) return false;

	if (buf[mid] == value) return true;
	else if (buf[mid] < value) {
		return binary_search(buf + mid + 1, size - mid - 1, value);
	}
	else {
		return binary_search(buf, mid, value);
	}
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

int generate_data(int type, int length);
cntr cntr_create(test_cont_type ct) {
	switch (ct) {
	case ec_list:
		return cntr_create_as_list();
	case ec_array:
		return cntr_create_as_array();
	case ec_bst:
		return cntr_create_as_bst(cntr_int_compare);
	default:
		return NULL;
	}

	return NULL;
}

void generate_test_data(test_data_type dt, test_data_length dl, int* length, int* ulength) {
	*length = dl == el_correctness ? clength : plength;
	*ulength = generate_data(dt, *length);
}

static const char* cntr_names[ec_end] = {"list", "array", "bst"};
const char* cntr_name(test_cont_type ct) {
	return cntr_names[ct];
}

static const char* data_order_names[ed_end] = {"increase", "decrease", "random" };
const char* data_order_name(test_data_type dt) {
	return data_order_names[dt];
}