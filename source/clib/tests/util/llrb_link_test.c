#include <llrb_link.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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

#define container_of(ptr, type, member) \
	(type *)( (char *)ptr - offsetof(type,member) )

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
			llrb_debug_check(root);
	}
}

void remove_data(int n, bool check) {
	int i;

	for (i = 0; i < n; i ++) {
		root = llrb_remove(root, &(pdata[i]->link), comp);
		if (check)
			llrb_debug_check(root);
	}
}

void llrb_link_correctness_test() {
	int length = 1000;
	printf("llrb link correctness test begin\n");
	_init_data(length);
	insert_data(length, true);
	remove_data(length, true);
	_destroy_data(length);
	printf("llrb link correctness test end\n");
}

void llrb_link_performance_test() {
	clock_t start_c, end_c;
	int length = N;

	printf("llrb link performance test begin\n");
	_init_data(length);

	start_c = clock();	
	insert_data(length, false);
	remove_data(length, false);
	end_c = clock();
	printf("insert/remove %d elements used %f\n", length, (float)(end_c - start_c)/CLOCKS_PER_SEC);

	_destroy_data(length);
	
	printf("llrb link performance test end\n");
}

void llrb_link_test() {
	llrb_link_correctness_test();
	llrb_link_performance_test();
}
