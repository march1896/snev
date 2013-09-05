struct data_t {
	struct skip_link* link;

	int key;
};

static int comp(const struct skip_link* l, const struct skip_link* r) {
	struct data_t* pl = container_of(l, struct data_t, link);
	struct data_t* pr = container_of(r, struct data_t, link);

	if (pl->key < pr->key) return -1;
	else if (pl->key == pr->key) return 0;
	return 1;
}

static int direct(const struct skip_link* cur, void* param) {
	int compr = comp((struct skip_link*)param, cur);

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

static int direct_first(const struct skip_link* link, void* param) {
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

static int direct_last(const struct skip_link* link, void* param) {
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


static struct skip_link* root = NULL;

static void tree_clear() {
	while (root != NULL) {
		root = tree_remove(root, root, comp);
	}
}

#define dlink(i, j) (&pdata[i][j]->link)
#define linkdata(l) ((container_of(l, struct data_t, link))->key)
static void struct skip_link_basic_test() {
	struct skip_link* temp;
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

static void struct skip_link_robust_test() {
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

static void struct skip_link_insert_remove() {
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

static void struct skip_link_insert_remove_s() {
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

static void struct skip_link_insert_remove_v() {
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

static void struct skip_link_insert_remove_sv() {
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

static void struct skip_link_search() {
	int i = 0;

	dbg_assert(root == NULL);
	for (i = 0; i < data_max_diff_type; i ++) {
		root = tree_insert(root, dlink(i, 0), comp);
	}

	for (i = 0; i < num_operation; i ++) {
		int idx = rand() % data_max_diff_type;
		struct skip_link* q = tree_search(root, direct, dlink(idx, 0));
		dbg_assert(q == dlink(idx, 0));
	}
}

static void struct skip_link_patterned_search() {
	int i = 0;

	dbg_assert(root == NULL);
	for (i = 0; i < data_max_diff_type; i ++) {
		root = tree_insert(root, dlink(i, 0), comp);
	}

	for (i = 0; i < num_operation / data_max_diff_type; i ++) {
		int idx;
		for (idx = 0; idx < data_max_diff_type; idx ++) {
			struct skip_link* q = tree_search(root, direct, dlink(idx, 0));
			dbg_assert(q == dlink(idx, 0));
		}
	}
}

#ifdef tree_dynamic_search
static void struct skip_link_dynamic_search() {
	int i = 0;

	dbg_assert(root == NULL);
	for (i = 0; i < data_max_diff_type; i ++) {
		root = tree_insert(root, dlink(i, 0), comp);
	}

	for (i = 0; i < num_operation; i ++) {
		int idx = rand() % data_max_diff_type;
		struct skip_link* q = tree_dynamic_search(&root, direct, dlink(idx, 0));
		dbg_assert(q == dlink(idx, 0));
	}
}

static void struct skip_link_patterned_dynamic_search() {
	int i = 0;

	dbg_assert(root == NULL);
	for (i = 0; i < data_max_diff_type; i ++) {
		root = tree_insert(root, dlink(i, 0), comp);
	}

	for (i = 0; i < num_operation / data_max_diff_type; i ++) {
		int idx;
		for (idx = 0; idx < data_max_diff_type; idx ++) {
			struct skip_link* q = tree_dynamic_search(&root, direct, dlink(idx, 0));
			dbg_assert(q == dlink(idx, 0));
		}
	}
}
#endif

static void struct skip_link_correctness_test() {
	struct skip_link_basic_test();
	//struct skip_link_robust_test();

	num_operation = 100 * 1000;

	data_max_diff_type = 1000;
	data_max_dup = 10;
	__create_data();
	tree_clear();
	test_run_single("insert   /remove correctness", struct skip_link_insert_remove);

	__reset_data();
	tree_clear();
	test_run_single("insert_s /remove correctness", struct skip_link_insert_remove_s);

	__reset_data();
	tree_clear();
	test_run_single("insert_v /remove correctness", struct skip_link_insert_remove_v);

	__reset_data();
	tree_clear();
	test_run_single("insert_sv/remove correctness", struct skip_link_insert_remove_sv);

	__reset_data();
	tree_clear();
	test_run_single("search correctness", struct skip_link_search);
	tree_clear();

	__destroy_data();
}

static void struct skip_link_performance_test() {
	num_operation = 100 * 10000;
	data_max_diff_type = 10000;
	data_max_dup = 10;

	__create_data();
	tree_clear();
	test_run_single("insert   /remove performance", struct skip_link_insert_remove);

	__reset_data();
	tree_clear();
	test_run_single("insert_s /remove performance", struct skip_link_insert_remove_s);

	__reset_data();
	tree_clear();
	test_run_single("insert_v /remove performance", struct skip_link_insert_remove_v);

	__reset_data();
	tree_clear();
	test_run_single("insert_sv/remove performance", struct skip_link_insert_remove_sv);

#ifndef tree_dynamic_search 
	__reset_data();
	tree_clear();
	test_run_single("search           performance", struct skip_link_search);

	__reset_data();
	tree_clear();
	test_run_single("patterned search performance", struct skip_link_patterned_search);

#else /* tree_dynamic_search */
	__reset_data();
	tree_clear();
	test_run_single("dynamic search   performance", struct skip_link_dynamic_search);

	__reset_data();
	tree_clear();
	test_run_single("patterned dynamic search performance", struct skip_link_patterned_dynamic_search);
#endif

	tree_clear();
	__destroy_data();
}

