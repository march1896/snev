#include <bst_link.h>

inline struct bst_link* bst_min(struct bst_link* root) {
	if (root == NULL) {
		return NULL;
	}

	while (root->left != NULL) {
		root = root->left;
	}

	return root;
}

inline struct bst_link* bst_max(struct bst_link* root) {
	if (root == NULL) {
		return NULL;
	}

	while (root->right != NULL) {
		root = root->right;
	}

	return root;
}

inline struct bst_link* bst_sub_predecessor(struct bst_link* link) {
	return bst_max(link->left);
}

inline struct bst_link* bst_sub_successor(struct bst_link* link) {
	return bst_min(link->right);
}

inline struct bst_link* bst_rotate_left(struct bst_link* link) {
	struct bst_link* right_c = link->right;

	/* debug_assert(link != NULL); */
	dbg_assert(link->right != NULL);

	link->right   = right_c->left;
	right_c->left = link;

	return right_c;
}

inline struct bst_link* bst_rotate_right(struct bst_link* link) {
	struct bst_link* left_c = link->left;

	dbg_assert(link->left != NULL);

	link->left   = left_c->right;
	left_c->right = link;

	return left_c;
}

static int bst_compare_warp(const struct bst_link* lhs, const struct bst_link* rhs, pf_bst_compare raw_comp) {
	if (raw_comp(lhs, rhs) < 0) {
		return -1;
	}
	else if (raw_comp(lhs, rhs) > 0) {
		return 1;
	}

	if (lhs < rhs) {
		return -1;
	}
	else if (lhs > rhs) {
		return 1;
	}

	return 0;
}

static int bst_compare_warp_v(const struct bst_link* lhs, const struct bst_link* rhs, pf_bst_compare_v raw_comp, void* raw_param) {
	if (raw_comp(lhs, rhs, raw_param) < 0) {
		return -1;
	}
	else if (raw_comp(lhs, rhs, raw_param) > 0) {
		return 1;
	}

	if (lhs < rhs) {
		return -1;
	}
	else if (lhs > rhs) {
		return 1;
	}

	return 0;
}

struct bst_link* bst_insert(struct bst_link* root, struct bst_link* n_link, pf_bst_compare comp) {
	struct bst_link* fwd = root;
	struct bst_link* par = NULL;
	if (root == NULL)
		return n_link;

	while (fwd != NULL) {
		int comp_res = bst_compare_wrap(fwd, n_link, comp);

		if (comp_res == 0) {
			/* the n_link is already in the tree, just return the root */
			return root;
		}

		par = fwd;
		fwd = comp_res > 0 ? fwd->right : fwd->left;
	}

	if (comp_res < 0) 
		par->left = n_link;
	else 
		par->right = n_link;

	return root;
}

struct bst_link* bst_search(struct bst_link* root, void*  param,            pf_bst_direct direct) {
	int dir = direct(root, param);

	if (root == NULL) return NULL;

	while (dir != 0) {
		root = dir > 0 ? root->right : root->left;

		dir = direct(root, param);
	}

	return root;
}

struct bst_link* bst_remove(struct bst_link* root, struct bst_link* to_rm, pf_bst_compare comp) {
	/* first we should navigate to the link tobe remove */
	struct bst_link* par = NULL;
	struct bst_link* fwd = root;
	struct bst_link* replace = NULL;
	int comp_res;

	while (fwd != to_rm) {
		comp_res = bst_compare_wrap(fwd, to_rm, comp);

		par = fwd;
		fwd = comp_res > 0 ? fwd->right : fwd->left;
	}

	dbg_assert(fwd == to_rm);

	if (to_rm->left == NULL && to_rm->right == NULL) {
		/* leaf node */
		replace = NULL;
	}
	else if (to_rm->left == NULL) {
		replace = to_rm->right;
	}
	else if (to_rm->right == NULL) {
		replace = to_rm->left;
	}
	else {
		int choice = math_rand(2);
		struct bst_link* leaf_par = NULL;

		
	}

	if (par != NULL) {
		if (par->left == to_rm)
			par->left = replace;
		if (par->right == to_rm)
			par->right = replace;

		return root;
	}

	return replace;
}

struct bst_link* bst_insert_v(struct bst_link* root, struct bst_link* n_link, pf_bst_compare comp, void* param);
struct bst_link* bst_remove_v(struct bst_link* root, struct bst_link* n_link, pf_bst_compare comp, void* param);
