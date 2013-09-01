/*
 * the following macro should be define before include this file.
 *
 typedef struct llrb_link tree_link;
 #define tree_insert llrb_insert
 #define tree_insert_s llrb_insert_s
 #define tree_insert_v llrb_insert_v
 #define tree_insert_sv llrb_insert_sv
 #define tree_remove llrb_remove
 #define tree_remove_v llrb_remove_v
 #define tree_search llrb_search
 #define pf_tree_compare pf_llrb_compare
 #define tree_min llrb_min
 #define tree_max llrb_max
 #define tree_check llrb_debug_check
 #define tree_check_v llrb_debug_check_v
*/


struct data_t {
	int key;

	tree_link link;
};

static struct data_t max_data = {
	2, /* neither min_data nor max_data */
	{
		&max_data.link,
		&max_data.link
	}
};

static struct data_t min_data = {
	3, /* neither min_data nor max_data */
	{
		&min_data.link,
		&min_data.link
	}
};

static int comp(const tree_link* l, const tree_link* r) {
	struct data_t* pl = container_of(l, struct data_t, link);
	struct data_t* pr = container_of(r, struct data_t, link);

	if (pl->key < pr->key) return -1;
	else if (pl->key == pr->key) return 0;
	return 1;
}

static struct data_bound {
	tree_link* min;
	tree_link* max;
} bound = {
	&min_data.link,
	&max_data.link
};

static int comp_v(const tree_link* l, const tree_link* r, void* param) {
	struct data_t* pl = container_of(l, struct data_t, link);
	struct data_t* pr = container_of(r, struct data_t, link);
	struct data_bound* bound = (struct data_bound*)param;

	if (l == r)
		return 0;

	if (l == bound->min) 
		return -1;
	if (l == bound->max)
		return 1;
	if (r == bound->min)
		return 1;
	if (r == bound->max)
		return -1;

	if (pl->key < pr->key) return -1;
	else if (pl->key == pr->key) return 0;
	return 1;
}

static int direct(const tree_link* cur, void* param) {
	int compr = comp((tree_link*)param, cur);

	if (compr != 0) 
		return compr;

	if (param < (void*)cur)
		return -1;
	else if (param == (void*)cur)
		return 0;
	else 
		return 1;

	return 0;
}

static int direct_v(const tree_link* cur, void* param) {
	int compr = comp_v((tree_link*)param, cur, &bound);

	if (compr != 0) 
		return compr;

	if (param < (void*)cur)
		return -1;
	else if (param == (void*)cur)
		return 0;
	else 
		return 1;

	return 0;
}

struct direct_s {
	pf_tree_compare comp;
	const tree_link*      target;
	const tree_link*      candidate;
};

static int direct_first(const tree_link* link, void* param) {
	struct direct_s* dir = (struct direct_s*)param;
	int    compr         = dir->comp(dir->target, link);

	if (compr == 0) {
		dir->candidate = link; /* update the candidate */
		return -1; /* explore the left side */
	}
	else if (compr < 0) /* we should explore left side */
		return -1;
	else {
		return 1;
	}

	/* never been here */
	return 0;
}

static int direct_last(const tree_link* link, void* param) {
	struct direct_s* dir    = (struct direct_s*)param;
	int    compr            = dir->comp(dir->target, link);

	if (compr == 0) {
		dir->candidate = link; /* update the candidate */
		return 1; /* explore the right side */
	}
	else if (compr < 0) /* we should explore left side */
		return -1;
	else {
		return 1;
	}

	/* never been here */
	return 0;
}

static int data_max_diff_type;
static int data_max_dup;

static struct data_t*** pdata;
static int* dup_count;

static void __create_data() {
	int i, j;
	pdata = (struct data_t***)malloc(data_max_diff_type * sizeof(struct data_t**));
	dup_count = (int*)malloc(data_max_diff_type * sizeof(int));

	for (i = 0; i < data_max_diff_type; i ++) {
		pdata[i] = (struct data_t**)malloc(data_max_dup * sizeof(struct data_t*));
		for (j = 0; j < data_max_dup; j++) {
			pdata[i][j] = (struct data_t*)malloc(sizeof(struct data_t));
			pdata[i][j]->key = i;
		}
	
		dup_count[i] = 0;
	}
}

static void __reset_data() {
	int i;
	for (i = 0; i < data_max_diff_type; i ++) {
		dup_count[i] = 0;
	}
}

static void __destroy_data() {
	int i, j;

	for (i = 0; i < data_max_diff_type; i ++) {
		for (j = 0; j < data_max_dup; j ++) {
			free(pdata[i][j]);
		}
		free(pdata[i]);
	}
	free(pdata);
}


static tree_link* root = NULL;

static void tree_clear() {
	while (root != NULL) {
		root = tree_remove(root, root, comp);
	}
}

#define dlink(i, j) (&pdata[i][j]->link)
#define linkdata(l) ((container_of(l, struct data_t, link))->key)
static void tree_link_basic_test() {
	tree_link* temp;
	dbg_assert(root == NULL);
	data_max_diff_type = 10;
	data_max_dup = 2;
	__create_data();
	
	/* test insert/remove/search */
	{
		root = tree_insert(root, dlink(2, 0), comp);
		root = tree_insert(root, dlink(0, 0), comp);
		root = tree_insert(root, dlink(1, 0), comp);
		root = tree_insert(root, dlink(3, 0), comp);
		/* now the tree contains { 0, 1, 2, 3 } */
		tree_check(root, comp);

		temp = tree_min(root);
		dbg_assert(linkdata(temp) == 0);
		temp = tree_max(root);
		dbg_assert(linkdata(temp) == 3);

		temp = tree_search(root, direct, dlink(0, 0));
		dbg_assert(temp == dlink(0, 0));
		temp = tree_search(root, direct, dlink(1, 0));
		dbg_assert(temp == dlink(1, 0));
		temp = tree_search(root, direct, dlink(2, 0));
		dbg_assert(temp == dlink(2, 0));
		temp = tree_search(root, direct, dlink(3, 0));
		dbg_assert(temp == dlink(3, 0));
		temp = tree_search(root, direct, dlink(4, 0));
		dbg_assert(temp == NULL);

		root = tree_remove(root, dlink(0, 0), comp);
		root = tree_remove(root, dlink(2, 0), comp);
		root = tree_insert(root, dlink(5, 0), comp);
		root = tree_insert(root, dlink(7, 0), comp);
		/* now the tree contains { 1, 3, 5, 7 } */
		tree_check(root, comp);

		temp = tree_min(root);
		dbg_assert(linkdata(temp) == 1);
		temp = tree_max(root);
		dbg_assert(linkdata(temp) == 7);

		temp = tree_search(root, direct, dlink(0, 0));
		dbg_assert(temp == NULL);
		temp = tree_search(root, direct, dlink(1, 0));
		dbg_assert(temp == dlink(1, 0));
		temp = tree_search(root, direct, dlink(2, 0));
		dbg_assert(temp == NULL);
		temp = tree_search(root, direct, dlink(3, 0));
		dbg_assert(temp == dlink(3, 0));
		temp = tree_search(root, direct, dlink(5, 0));
		dbg_assert(temp == dlink(5, 0));
		temp = tree_search(root, direct, dlink(7, 0));
		dbg_assert(temp == dlink(7, 0));

		root = tree_insert(root, dlink(3, 1), comp);
		/* now the tree is { 1, 3, 3, 5, 7 } */
		tree_check(root, comp);

		temp = tree_min(root);
		dbg_assert(linkdata(temp) == 1);
		temp = tree_max(root);
		dbg_assert(linkdata(temp) == 7);

		temp = tree_search(root, direct, dlink(3, 0));
		dbg_assert(temp == dlink(3, 0));
		temp = tree_search(root, direct, dlink(3, 1));
		dbg_assert(temp == dlink(3, 1));

		root = tree_remove(root, dlink(3, 1), comp);
		/* now the tree is { 1, 3, 5, 7 } */
		tree_check(root, comp);

		temp = tree_min(root);
		dbg_assert(linkdata(temp) == 1);
		temp = tree_max(root);
		dbg_assert(linkdata(temp) == 7);

		temp = tree_search(root, direct, dlink(3, 0));
		dbg_assert(temp == dlink(3, 0));

		root = tree_remove(root, dlink(3, 0), comp);
		/* now the tree is { 1, 5, 7 } */
		tree_check(root, comp);

		temp = tree_min(root);
		dbg_assert(linkdata(temp) == 1);
		temp = tree_max(root);
		dbg_assert(linkdata(temp) == 7);

		temp = tree_search(root, direct, dlink(3, 0));
		dbg_assert(temp == NULL);

		tree_clear();
	}
	/* test insert_s */
	{
		bool dup = false;
		dbg_assert(root == NULL);
		root = tree_insert_s(root, dlink(2, 0), comp, &dup);
		dbg_assert(dup == false);
		root = tree_insert_s(root, dlink(0, 0), comp, &dup);
		dbg_assert(dup == false);
		root = tree_insert_s(root, dlink(1, 0), comp, &dup);
		dbg_assert(dup == false);
		root = tree_insert_s(root, dlink(3, 0), comp, &dup);
		dbg_assert(dup == false);
		/* now the tree contains { 0, 1, 2, 3 } */
		tree_check(root, comp);

		temp = tree_min(root);
		dbg_assert(linkdata(temp) == 0);
		temp = tree_max(root);
		dbg_assert(linkdata(temp) == 3);

		temp = tree_search(root, direct, dlink(0, 0));
		dbg_assert(temp == dlink(0, 0));
		temp = tree_search(root, direct, dlink(1, 0));
		dbg_assert(temp == dlink(1, 0));
		temp = tree_search(root, direct, dlink(2, 0));
		dbg_assert(temp == dlink(2, 0));
		temp = tree_search(root, direct, dlink(3, 0));
		dbg_assert(temp == dlink(3, 0));
		temp = tree_search(root, direct, dlink(4, 0));
		dbg_assert(temp == NULL);

		root = tree_remove(root, dlink(0, 0), comp);
		root = tree_remove(root, dlink(2, 0), comp);
		root = tree_insert_s(root, dlink(5, 0), comp, &dup);
		dbg_assert(dup == false);
		root = tree_insert_s(root, dlink(7, 0), comp, &dup);
		dbg_assert(dup == false);
		/* now the tree contains { 1, 3, 5, 7 } */
		tree_check(root, comp);

		temp = tree_min(root);
		dbg_assert(linkdata(temp) == 1);
		temp = tree_max(root);
		dbg_assert(linkdata(temp) == 7);

		temp = tree_search(root, direct, dlink(0, 0));
		dbg_assert(temp == NULL);
		temp = tree_search(root, direct, dlink(1, 0));
		dbg_assert(temp == dlink(1, 0));
		temp = tree_search(root, direct, dlink(2, 0));
		dbg_assert(temp == NULL);
		temp = tree_search(root, direct, dlink(3, 0));
		dbg_assert(temp == dlink(3, 0));
		temp = tree_search(root, direct, dlink(5, 0));
		dbg_assert(temp == dlink(5, 0));
		temp = tree_search(root, direct, dlink(7, 0));
		dbg_assert(temp == dlink(7, 0));

		root = tree_insert_s(root, dlink(3, 1), comp, &dup);
		dbg_assert(dup == true);
		/* now the tree is still { 1, 3, 5, 7 } */
		tree_check(root, comp);

		temp = tree_min(root);
		dbg_assert(linkdata(temp) == 1);
		temp = tree_max(root);
		dbg_assert(linkdata(temp) == 7);

		temp = tree_search(root, direct, dlink(3, 0));
		dbg_assert(temp == dlink(3, 0));

		root = tree_remove(root, dlink(3, 0), comp);
		/* now the tree is { 1, 5, 7 } */
		tree_check(root, comp);

		temp = tree_min(root);
		dbg_assert(linkdata(temp) == 1);
		temp = tree_max(root);
		dbg_assert(linkdata(temp) == 7);

		temp = tree_search(root, direct, dlink(3, 0));
		dbg_assert(temp == NULL);

		tree_clear();
	}
	/* test insert_v/remove_v */
	{
		bool dup = false;
		dbg_assert(root == NULL);
		root = tree_insert_v(root, dlink(2, 0),    comp_v, &bound);
		root = tree_insert_v(root, &max_data.link, comp_v, &bound);
		root = tree_insert_v(root, dlink(0, 0),    comp_v, &bound);
		root = tree_insert_v(root, &min_data.link, comp_v, &bound);
		root = tree_insert_v(root, dlink(1, 0),    comp_v, &bound);
		root = tree_insert_v(root, dlink(3, 0),    comp_v, &bound);
		/* now the tree contains { min, 0, 1, 2, 3, max } */
		tree_check_v(root, comp_v, &bound);

		temp = tree_min(root);
		dbg_assert(linkdata(temp) == min_data.key);
		temp = tree_max(root);
		dbg_assert(linkdata(temp) == max_data.key);

		//temp = tree_search(root, direct, dlink(0, 0));
		temp = tree_search(root, direct_v, dlink(0, 0));
		dbg_assert(temp == dlink(0, 0));
		temp = tree_search(root, direct_v, dlink(1, 0));
		dbg_assert(temp == dlink(1, 0));
		temp = tree_search(root, direct_v, dlink(2, 0));
		dbg_assert(temp == dlink(2, 0));
		temp = tree_search(root, direct_v, dlink(3, 0));
		dbg_assert(temp == dlink(3, 0));
		temp = tree_search(root, direct_v, &min_data.link);
		dbg_assert(temp == &min_data.link);
		temp = tree_search(root, direct_v, &max_data.link);
		dbg_assert(temp == &max_data.link);

		root = tree_remove_v(root, dlink(0, 0), comp_v, &bound);
		root = tree_remove_v(root, dlink(2, 0), comp_v, &bound);
		root = tree_insert_v(root, dlink(5, 0), comp_v, &bound);
		root = tree_insert_v(root, dlink(7, 0), comp_v, &bound);
		/* now the tree contains { min, 1, 3, 5, 7, max } */
		tree_check_v(root, comp_v, &bound);

		temp = tree_min(root);
		dbg_assert(linkdata(temp) == min_data.key);
		temp = tree_max(root);
		dbg_assert(linkdata(temp) == max_data.key);

		temp = tree_search(root, direct_v, dlink(0, 0));
		dbg_assert(temp == NULL);
		temp = tree_search(root, direct_v, dlink(1, 0));
		dbg_assert(temp == dlink(1, 0));
		temp = tree_search(root, direct_v, dlink(2, 0));
		dbg_assert(temp == NULL);
		temp = tree_search(root, direct_v, dlink(3, 0));
		dbg_assert(temp == dlink(3, 0));
		temp = tree_search(root, direct_v, dlink(5, 0));
		dbg_assert(temp == dlink(5, 0));
		temp = tree_search(root, direct_v, dlink(7, 0));
		dbg_assert(temp == dlink(7, 0));

		root = tree_insert_sv(root, dlink(3, 1), comp_v, &bound, &dup);
		dbg_assert(dup == true);
		/* now the tree is still { min, 1, 3, 5, 7, max } */
		tree_check_v(root, comp_v, &bound);
		temp = tree_search(root, direct_v, dlink(3, 0));
		dbg_assert(temp == dlink(3, 0));

		root = tree_remove_v(root, dlink(3, 0), comp_v, &bound);
		/* now the tree is { min, 1, 5, 7, max } */
		tree_check_v(root, comp_v, &bound);
		temp = tree_search(root, direct_v, dlink(3, 0));
		dbg_assert(temp == NULL);

		root = tree_insert_v(root, dlink(5, 1), comp_v, &bound);
		/* now the tree is still { min, 1, 5, 5, 7, max } */
		tree_check_v(root, comp_v, &bound);
		temp = tree_search(root, direct_v, dlink(5, 1));
		dbg_assert(temp == dlink(5, 1));

		root = tree_remove_v(root, dlink(5, 1), comp_v, &bound);
		/* now the tree is { min, 1, 5, 7, max } */
		tree_check_v(root, comp_v, &bound);
		temp = tree_search(root, direct_v, dlink(5, 0));
		dbg_assert(temp == dlink(5, 0));

		root = tree_remove_v(root, dlink(5, 0), comp_v, &bound);
		/* now the tree is { min, 1, 7, max } */
		tree_check_v(root, comp_v, &bound);
		temp = tree_search(root, direct_v, dlink(5, 0));
		dbg_assert(temp == NULL);

		/* clear the tree */
		/* now the tree is { min, 1, 7, max } */
		root = tree_remove_v(root, &min_data.link, comp_v, &bound);
		root = tree_remove_v(root, &max_data.link, comp_v, &bound);
		root = tree_remove(root, dlink(1, 0), comp);
		root = tree_remove(root, dlink(7, 0), comp);

		dbg_assert(root == NULL);
	}

	__destroy_data();
	return;
}

static void tree_link_robust_test() {
	dbg_assert(root == NULL);

	data_max_diff_type = 10;
	data_max_dup = 2;
	__create_data();

	root = tree_remove(root, dlink(0, 0), comp);
	dbg_assert(root == NULL);

	root = tree_insert(root, dlink(0, 0), comp);
	root = tree_remove(root, dlink(1, 0), comp);
	dbg_assert(root == dlink(0, 0));

	__destroy_data();
}

static int data_max_diff_type;
static int num_operation;

static void tree_link_insert_remove() {
	int i = 0;

	dbg_assert(root == NULL);
	dbg_assert(data_max_dup >= 1);
	for (i = 0; i < num_operation; i ++) {
		int idx = rand() % data_max_diff_type;

		if (dup_count[idx] == 0) {
			root = tree_insert(root, dlink(idx, dup_count[idx] ++), comp);
		}
		else if (dup_count[idx] == data_max_dup) {
			root = tree_remove(root, dlink(idx, --dup_count[idx]), comp);
		}
		else {
			int add = rand() % 2;
			if (add == 1) {
				root = tree_insert(root, dlink(idx, dup_count[idx] ++), comp);
			}
			else {
				root = tree_remove(root, dlink(idx, -- dup_count[idx]), comp);
			}
		}
	}
}

static void tree_link_insert_remove_s() {
	int i = 0;

	dbg_assert(root == NULL);
	for (i = 0; i < num_operation; i ++) {
		int idx = rand() % data_max_diff_type;

		if (dup_count[idx] == 0) {
			bool dup = false;
			root = tree_insert_s(root, dlink(idx, 0), comp, &dup);
			dbg_assert(dup == false);
			dup_count[idx] ++;
		}
		else {
			root = tree_remove(root, dlink(idx, 0), comp);
			dup_count[idx] --;
		}
	}
}

static void tree_link_insert_remove_v() {
	int i = 0;

	dbg_assert(root == NULL);
	root = tree_insert_v(root, &min_data.link, comp_v, &bound);
	root = tree_insert_v(root, &max_data.link, comp_v, &bound);

	for (i = 0; i < num_operation; i ++) {
		int idx = rand() % data_max_diff_type;

		if (dup_count[idx] == 0) {
			root = tree_insert_v(root, dlink(idx, dup_count[idx] ++), comp_v, &bound);
		}
		else if (dup_count[idx] == data_max_dup) {
			root = tree_remove_v(root, dlink(idx, --dup_count[idx]), comp_v, &bound);
		}
		else {
			int add = rand() % 2;
			if (add == 1) {
				root = tree_insert_v(root, dlink(idx, dup_count[idx] ++), comp_v, &bound);
			}
			else {
				root = tree_remove_v(root, dlink(idx, --dup_count[idx]), comp_v, &bound);
			}
		}
	}
}

static void tree_link_insert_remove_sv() {
	int i = 0;

	dbg_assert(root == NULL);
	root = tree_insert_v(root, &min_data.link, comp_v, &bound);
	root = tree_insert_v(root, &max_data.link, comp_v, &bound);

	for (i = 0; i < num_operation; i ++) {
		int idx = rand() % data_max_diff_type;

		if (dup_count[idx] == 0) {
			bool dup = false;
			root = tree_insert_sv(root, dlink(idx, 0), comp_v, &bound, &dup);
			dbg_assert(dup == false);
			dup_count[idx] ++;
		}
		else {
			root = tree_remove_v(root, dlink(idx, 0), comp_v, &bound);
			dup_count[idx] --;
		}
	}
}

static void tree_link_search() {
	int i = 0;

	dbg_assert(root == NULL);
	for (i = 0; i < data_max_diff_type; i ++) {
		root = tree_insert(root, dlink(i, 0), comp);
	}

	for (i = 0; i < num_operation; i ++) {
		int idx = rand() % data_max_diff_type;
		tree_link* q = tree_search(root, direct, dlink(idx, 0));
		dbg_assert(q == dlink(idx, 0));
	}
}

static void tree_link_patterned_search() {
	int i = 0;

	dbg_assert(root == NULL);
	for (i = 0; i < data_max_diff_type; i ++) {
		root = tree_insert(root, dlink(i, 0), comp);
	}

	for (i = 0; i < num_operation / data_max_diff_type; i ++) {
		int idx;
		for (idx = 0; idx < data_max_diff_type; idx ++) {
			tree_link* q = tree_search(root, direct, dlink(idx, 0));
			dbg_assert(q == dlink(idx, 0));
		}
	}
}

#ifdef tree_dynamic_search
static void tree_link_dynamic_search() {
	int i = 0;

	dbg_assert(root == NULL);
	for (i = 0; i < data_max_diff_type; i ++) {
		root = tree_insert(root, dlink(i, 0), comp);
	}

	for (i = 0; i < num_operation; i ++) {
		int idx = rand() % data_max_diff_type;
		tree_link* q = tree_dynamic_search(&root, direct, dlink(idx, 0));
		dbg_assert(q == dlink(idx, 0));
	}
}

static void tree_link_patterned_dynamic_search() {
	int i = 0;

	dbg_assert(root == NULL);
	for (i = 0; i < data_max_diff_type; i ++) {
		root = tree_insert(root, dlink(i, 0), comp);
	}

	for (i = 0; i < num_operation / data_max_diff_type; i ++) {
		int idx;
		for (idx = 0; idx < data_max_diff_type; idx ++) {
			tree_link* q = tree_dynamic_search(&root, direct, dlink(idx, 0));
			dbg_assert(q == dlink(idx, 0));
		}
	}
}
#endif

static void tree_link_correctness_test() {
	tree_link_basic_test();
	//tree_link_robust_test();

	num_operation = 100 * 1000;

	data_max_diff_type = 1000;
	data_max_dup = 10;
	__create_data();
	tree_clear();
	test_run_single("insert   /remove correctness", tree_link_insert_remove);

	__reset_data();
	tree_clear();
	test_run_single("insert_s /remove correctness", tree_link_insert_remove_s);

	__reset_data();
	tree_clear();
	test_run_single("insert_v /remove correctness", tree_link_insert_remove_v);

	__reset_data();
	tree_clear();
	test_run_single("insert_sv/remove correctness", tree_link_insert_remove_sv);

	__reset_data();
	tree_clear();
	test_run_single("search correctness", tree_link_search);
	tree_clear();

	__destroy_data();
}

static void tree_link_performance_test() {
	num_operation = 100 * 10000;
	data_max_diff_type = 10000;
	data_max_dup = 10;

	__create_data();
	tree_clear();
	test_run_single("insert   /remove performance", tree_link_insert_remove);

	__reset_data();
	tree_clear();
	test_run_single("insert_s /remove performance", tree_link_insert_remove_s);

	__reset_data();
	tree_clear();
	test_run_single("insert_v /remove performance", tree_link_insert_remove_v);

	__reset_data();
	tree_clear();
	test_run_single("insert_sv/remove performance", tree_link_insert_remove_sv);

#ifndef tree_dynamic_search 
	__reset_data();
	tree_clear();
	test_run_single("search           performance", tree_link_search);

	__reset_data();
	tree_clear();
	test_run_single("patterned search performance", tree_link_patterned_search);

#else /* tree_dynamic_search */
	__reset_data();
	tree_clear();
	test_run_single("dynamic search   performance", tree_link_dynamic_search);

	__reset_data();
	tree_clear();
	test_run_single("patterned dynamic search performance", tree_link_patterned_dynamic_search);
#endif

	tree_clear();
	__destroy_data();
}

