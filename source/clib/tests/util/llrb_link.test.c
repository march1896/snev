#include <llrb_link.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <test_util.h>

struct data {
	int key;
	int value;

	struct llrb_link link;
};

#define N 500000
struct data *pdata[N];

static void _init_data(int n) {
	int i;
	unsigned int iseed = (unsigned int)time(NULL);

	srand (iseed);

	for (i = 0; i < n; i ++) {
		pdata[i] = (struct data*)malloc(sizeof(struct data));

		pdata[i]->key = rand() % N;
		pdata[i]->value = 0;
	}
}

static void _destroy_data(int n) {
	int i;

	for (i = 0; i < n; i ++) {
		free(pdata[i]);
	}
}

static struct llrb_link *root = NULL;

int comp(const struct llrb_link *l, const struct llrb_link *r) {
	struct data *pl, *pr;
	pl = container_of(l, struct data, link);
	pr = container_of(r, struct data, link);

	if (pl->key < pr->key) return -1;
	else if (pl->key == pr->key) return 0;
	else return 1;
}

void insert_data(int n, bool check) {
	int i;

	for (i = 0; i < n; i ++) {
		root = llrb_insert(root, &(pdata[i]->link), comp);
		if (check)
			llrb_debug_check(root, comp);
	}
}

void remove_data(int n, bool check) {
	int i;

	for (i = 0; i < n; i ++) {
		root = llrb_remove(root, &(pdata[i]->link), comp);
		if (check)
			llrb_debug_check(root, comp);
	}
}

void llrb_link_correctness_test() {
	int length = 1000;
	_init_data(length);
	insert_data(length, true);
	remove_data(length, true);
	_destroy_data(length);
}

void llrb_link_performance_test() {
	clock_t start_c, end_c;
	int length = N;

	_init_data(length);

	start_c = clock();	
	insert_data(length, false);
	remove_data(length, false);
	end_c = clock();
	log_printline("insert/remove %d elements used %f", length, (float)(end_c - start_c)/CLOCKS_PER_SEC);

	_destroy_data(length);
}

void llrb_link_test() {
	test_run_single("llrb link correctness test", llrb_link_correctness_test);
	test_run_single("llrb link performance test", llrb_link_performance_test);
}
