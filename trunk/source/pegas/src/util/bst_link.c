#include <util/cominc.h>
#include <util/bst_link.h>

#include <util/math.h>

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

static int bst_compare_wrap_v(const struct bst_link* lhs, const struct bst_link* rhs, pf_bst_compare_v raw_comp, void* raw_param) {
	int raw_result = raw_comp(lhs, rhs, raw_param);
	if (raw_result < 0) {
		return -1;
	}
	else if (raw_result > 0) {
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

static int bst_compare_wrap(struct bst_link* a, struct bst_link* b, pf_bst_compare comp) {
	int raw_result = comp(a, b);
	if (raw_result < 0)
		return -1;
	else if (raw_result > 0)
		return 1;

	if (a < b) 
		return -1;
	else if (a > b)
		return 1;

	return 0;
}

struct bst_link* bst_insert(struct bst_link* root, struct bst_link* n_link, pf_bst_compare comp) {
	struct bst_link* fwd = root;
	struct bst_link* par = NULL;
	int comp_res = 0;
	if (root == NULL)
		return n_link;

	while (fwd != NULL) {
		comp_res = bst_compare_wrap(fwd, n_link, comp);

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

struct bst_link* bst_search(struct bst_link* root, void*  param, pf_bst_direct direct) {
	int dir = direct(root, param);

	if (root == NULL) return NULL;

	while (dir != 0) {
		root = dir > 0 ? root->right : root->left;

		dir = direct(root, param);
	}

	return root;
}

struct bst_link* bst_replace(struct bst_link* cur, struct bst_link* par) {
	struct bst_link* replacement = NULL;

	dbg_assert(par == NULL || 
			  (par != NULL && (par->left == cur || par->right == cur)));

	if (cur == NULL) return NULL;

	if (cur->left == NULL && cur->right == NULL) {
		replacement = NULL;
	}
	else if (cur->left != NULL) {
		replacement = cur->left;
	}
	else if (cur->right != NULL) {
		replacement = cur->right;
	}
	else {
		/* two choices, randomly choose one. */
		int choose_left = mrand(2);
		struct bst_link* leaf_par = cur;
		struct bst_link* leaf = NULL;
		if (choose_left) {
			leaf = leaf_par->left;
			while (leaf->right != NULL) {
				leaf_par = leaf;
				leaf = leaf->right;
			}
			leaf_par->right = NULL;
		}
		else {
			leaf = leaf_par->right;
			while (leaf->left != NULL) {
				leaf_par = leaf;
				leaf = leaf->left;
			}
			leaf_par->left = NULL;
		}
		replacement = leaf;
		replacement->left = cur->left;
		replacement->right= cur->right;
	}

	/* we have to reassign the child pointer of par */
	if (par != NULL) {
		if (par->left == cur)
			par->left = replacement;
		else
			par->right = replacement;
	}
	
	return replacement;
}

struct bst_link* bst_remove(struct bst_link* root, struct bst_link* to_rm, pf_bst_compare comp) {
	/* first we should navigate to the link to be remove */
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
	
	replace = bst_replace(fwd, par);

	if (to_rm == root) {
		/* return the new root */
		return replace;
	}

	return root;
}

struct bst_link* bst_insert_v(struct bst_link* root, struct bst_link* n_link, pf_bst_compare_v comp, void* param) {
	return NULL;
}
struct bst_link* bst_remove_v(struct bst_link* root, struct bst_link* n_link, pf_bst_compare_v comp, void* param) {
	return NULL;
}


inline struct bstp_link* bstp_predecessor(struct bstp_link* cur) {
	return NULL;
}

inline struct bstp_link* bstp_successor(struct bstp_link* root) {
	return NULL;
}

inline struct bstp_link* bstp_rotate_left(struct bstp_link* link) {
	return NULL;
}

inline struct bstp_link* bstp_rotate_right(struct bstp_link* link) {
	return NULL;
}
