#include <test_util.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <cntr.h>

int *rawdata, *uniquedata, *uniquesorteddata;

void init_data() {
	unsigned int iseed = (unsigned int)time(NULL);
	srand (iseed);

	rawdata = (int*)malloc(sizeof(int)*PERFORMANCE_LENGTH);
	uniquedata = (int*)malloc(sizeof(int)*PERFORMANCE_LENGTH);
	uniquesorteddata = (int*)malloc(sizeof(int)*PERFORMANCE_LENGTH);
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
int generate_data(TD_TYPE type, int length) {
	int i;
	if (type == TD_INCREASE) {
		// increase
		for (i = 0; i < length; i ++) {
			rawdata[i] = uniquedata[i] = uniquesorteddata[i] = i + 1;
		}
		return length;
	}
	else if (type == TD_DECREASE) {
		for (i = 0; i < length; i ++) {
			rawdata[i] = uniquedata[i] = length - i;
			uniquesorteddata[i] = i + 1;
		}
		return length;
	}
	else if (type == TD_UNIQUE) {
		for (i = 0; i < length; i ++) {
			rawdata[i] = uniquedata[i] = 1;
			uniquesorteddata[i] = 1;
		}
		return length;
	}
	else if (type == TD_RANDOM) {
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

int generate_data(TD_TYPE type, int length);
cntr cntr_create(TC_TYPE ct) {
	switch (ct) {
	case TC_LIST:
		return cntr_create_as_list();
	case TC_ARRAY:
		return cntr_create_as_array();
	case TC_BST:
		return cntr_create_as_bst_v(cntr_int_compare);
	case TC_RBT:
		return cntr_create_as_rbt_v(cntr_int_compare);
	case TC_234T:
		return cntr_create_as_fbt_v(cntr_int_compare);
	case TC_LLRBTREE:
		return cntr_create_as_llrb_v(cntr_int_compare);
	default:
		return NULL;
	}

	return NULL;
}

void generate_test_data(TD_TYPE dt, TD_LENGTH dl, int* length, int* ulength) {
	*length = dl == TL_CORRECTNESS ? CORRECTNESS_LENGTH : PERFORMANCE_LENGTH;
	*ulength = generate_data(dt, *length);
}

static const char* cntr_names[TC_END] = {"list", "array", "bst", "rbt", "234t", "llrb_tree" };
const char* cntr_name(TC_TYPE ct) {
	return cntr_names[ct];
}

static const char* data_order_names[TD_END] = {"increase", "decrease", "random", "unique" };
const char* data_order_name(TD_TYPE dt) {
	return data_order_names[dt];
}
