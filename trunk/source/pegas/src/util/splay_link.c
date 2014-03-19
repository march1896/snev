#include <util/splay_link.h>

static inline struct splaylink *__splay_rotate_left(struct splaylink* n) {
	struct splaylink* r = n->right;
	struct splaylink* p = n->parent;

	dbg_assert(r != NULL);

	n->parent = r;
	n->right = r->left;

	if (r->left != NULL) 
		r->left->parent = n;

	r->left = n;
	r->parent = p;
	if (p != NULL) {
		if (p->left == n) 
			p->left = r;
		else 
			p->right = r;
	}

	return r;
}

static inline struct splaylink *__splay_rotate_right(struct splaylink *n) {
	struct splaylink *l = n->left;
	struct splaylink *p = n->parent;

	dbg_assert(l != NULL);

	n->parent = l;
	n->left = l->right;

	if (l->right != NULL) 
		l->right->parent = n;

	l->right = n;
	l->parent = p;
	if (p != NULL) {
		if (p->left == n) 
			p->left = l;
		else 
			p->right = l;
	}

	return l;
}

static inline void __splay(struct splaylink* n) {
	while (n->parent) {
		if (!n->parent->parent) {
			/* Zip step */
			if (n->parent->left == n) 
				__splay_rotate_right(n->parent);
			else 
				__splay_rotate_left(n->parent);
		}
		else if (n->parent->left == n && n->parent->parent->left == n->parent) {
			/* Zip zip step */
			__splay_rotate_right(n->parent->parent);
			__splay_rotate_right(n->parent);
		}
		else if (n->parent->right == n && n->parent->parent->right == n->parent) {
			/* Zip zip step */
			__splay_rotate_left(n->parent->parent);
			__splay_rotate_left(n->parent);
		}
		else if (n->parent->left == n && n->parent->parent->right == n->parent) {
			/* Zip zag step */
			__splay_rotate_right(n->parent);
			__splay_rotate_left(n->parent);
		}
		else {
			/* Zip zag step */
			dbg_assert(n->parent->right == n && n->parent->parent->left == n->parent);
			__splay_rotate_left(n->parent);
			__splay_rotate_right(n->parent);
		}
	}
}

static inline void splay_init(struct splaylink *n) {
	dbg_assert(n != NULL);

	n->left = NULL;
	n->right = NULL;
	n->parent = NULL;
}

/*
 * use a <key, address> pair as the real key stored in the tree.
 */
static inline int __compare_wrap(const struct splaylink* a, const struct splaylink* b, pf_splay_compare raw_comp) {
	int raw_result = raw_comp(a, b);

	if (raw_result != 0) return raw_result;

	if (a < b) 
		return -1;
	else if (a > b) 
		return 1;

	return 0;
}

static inline int __compare_wrap_v(const struct splaylink* a, const struct splaylink* b, pf_splay_compare_v raw_comp, void* param) {
	int raw_result = raw_comp(a, b, param);

	if (raw_result != 0) return raw_result;

	if (a < b) 
		return -1;
	else if (a > b) 
		return 1;

	return 0;
}

struct splaylink *splay_remove(struct splaylink *root, struct splaylink *target, pf_splay_compare comp) {
	struct splaylink* fwd = root;

	dbg_assert(root != NULL && target != NULL);
	while (fwd != target) {
		int compr = __compare_wrap(target, fwd, comp);

		if (compr == 0) 
			break;
		if (compr < 0) 
			fwd = fwd->left;
		else 
			fwd = fwd->right;
	}
	dbg_assert(fwd != NULL);
	if (fwd == NULL)
		return root;

	dbg_assert(fwd == target);
	__splay(target);

	dbg_assert(target->parent == NULL);
	if (target->left == NULL && target->right == NULL) {
		/* the only element in the tree */
		return NULL;
	}
	else if (target->left == NULL) {
		target->right->parent = NULL;
		return target->right;
	}
	else if (target->right == NULL) {
		target->left->parent = NULL;
		return target->left;
	}
	else {
		/* TODO: randomly ? */
		struct splaylink* swap_node = splay_min(target->right);
		dbg_assert(swap_node != NULL);
		
		if (swap_node->parent == target) {
			/* target is the root */
			swap_node->parent = NULL;
			if (swap_node == target->left) {
				dbg_assert(swap_node->right == NULL);
				swap_node->right = target->right;

				if (target->right)
					target->right->parent = swap_node;
			}
			else {
				dbg_assert(swap_node == target->right);
				dbg_assert(swap_node->left == NULL);
				swap_node->left = target->left;

				if (target->left)
					target->left->parent = swap_node;
			}

			return swap_node;
		}
		else {
			/* remove the swap node */
			dbg_assert(swap_node->parent != NULL);
			dbg_assert(swap_node->left == NULL || swap_node->right == NULL);
			if (swap_node->left == NULL && swap_node->right == NULL) {
				/* leaf node */
				if (swap_node->parent->left == swap_node)
					swap_node->parent->left = NULL;
				else {
					dbg_assert(swap_node->parent->right == swap_node);
					swap_node->parent->right = NULL;
				}
			}
			else if (swap_node->right == NULL) {
				/* replace the maximum element in the left sub tree */
				dbg_assert(swap_node->parent->right == swap_node);
				swap_node->parent->right = swap_node->left;
				if (swap_node->left)
					swap_node->left->parent = swap_node->parent;
			}
			else {
				/* replace the minimum element in the right sub tree */
				dbg_assert(swap_node->parent->left == swap_node);
				swap_node->parent->left = swap_node->right;
				if (swap_node->right)
					swap_node->right->parent = swap_node->parent;
			}

			swap_node->parent = NULL;
			swap_node->left   = target->left;
			swap_node->right  = target->right;

			if (target->left)
				target->left->parent = swap_node;
			if (target->right)
				target->right->parent = swap_node;

			return swap_node;
		}
	}

	dbg_assert(false);
	return NULL;
}

struct splaylink* splay_remove_v(struct splaylink* root, struct splaylink* target, pf_splay_compare_v comp_v, void* param) {
	struct splaylink* fwd = root;

	dbg_assert(root != NULL && target != NULL);
	while (fwd != target) {
		int compr = __compare_wrap_v(target, fwd, comp_v, param);

		if (compr == 0) 
			break;
		if (compr < 0) 
			fwd = fwd->left;
		else 
			fwd = fwd->right;
	}
	dbg_assert(fwd != NULL);
	if (fwd == NULL)
		return root;

	dbg_assert(fwd == target);
	__splay(target);

	dbg_assert(target->parent == NULL);
	if (target->left == NULL && target->right == NULL) {
		/* the only element in the tree */
		return NULL;
	}
	else if (target->left == NULL) {
		target->right->parent = NULL;
		return target->right;
	}
	else if (target->right == NULL) {
		target->left->parent = NULL;
		return target->left;
	}
	else {
		/* TODO: randomly ? */
		struct splaylink* swap_node = splay_min(target->right);
		dbg_assert(swap_node != NULL);
		
		if (swap_node->parent == target) {
			/* target is the root */
			swap_node->parent = NULL;
			if (swap_node == target->left) {
				dbg_assert(swap_node->right == NULL);
				swap_node->right = target->right;

				if (target->right)
					target->right->parent = swap_node;
			}
			else {
				dbg_assert(swap_node == target->right);
				dbg_assert(swap_node->left == NULL);
				swap_node->left = target->left;

				if (target->left)
					target->left->parent = swap_node;
			}

			return swap_node;
		}
		else {
			/* remove the swap node */
			dbg_assert(swap_node->parent != NULL);
			dbg_assert(swap_node->left == NULL || swap_node->right == NULL);
			if (swap_node->left == NULL && swap_node->right == NULL) {
				/* leaf node */
				if (swap_node->parent->left == swap_node)
					swap_node->parent->left = NULL;
				else {
					dbg_assert(swap_node->parent->right == swap_node);
					swap_node->parent->right = NULL;
				}
			}
			else if (swap_node->right == NULL) {
				/* replace the maximum element in the left sub tree */
				dbg_assert(swap_node->parent->right == swap_node);
				swap_node->parent->right = swap_node->left;
				if (swap_node->left)
					swap_node->left->parent = swap_node->parent;
			}
			else {
				/* replace the minimum element in the right sub tree */
				dbg_assert(swap_node->parent->left == swap_node);
				swap_node->parent->left = swap_node->right;
				if (swap_node->right)
					swap_node->right->parent = swap_node->parent;
			}

			swap_node->parent = NULL;
			swap_node->left   = target->left;
			swap_node->right  = target->right;

			if (target->left)
				target->left->parent = swap_node;
			if (target->right)
				target->right->parent = swap_node;

			return swap_node;
		}
	}

	dbg_assert(false);
	return NULL;
}

struct splaylink *splay_insert(struct splaylink *root, struct splaylink *nlink, pf_splay_compare comp) {
	struct splaylink* fwd = root;
	struct splaylink* par = NULL;
	int compr = 0;

	splay_init(nlink);

	if (root == NULL) {
		return nlink;
	}

	while (fwd != NULL) {
		compr = __compare_wrap(nlink, fwd, comp);

		par = fwd;
		if (compr == 0) 
			return root;
		if (compr < 0) 
			fwd = fwd->left;
		else 
			fwd = fwd->right;
	}

	if (compr < 0)
		par->left = nlink;
	else 
		par->right = nlink;
	nlink->parent = par;

	__splay(nlink);

	dbg_assert(nlink->parent == NULL);

	return nlink;
}

struct splaylink* splay_insert_v(struct splaylink* root, struct splaylink* nlink, pf_splay_compare_v comp, void* param) {
	struct splaylink* fwd = root;
	struct splaylink* par = NULL;
	int compr = 0;

	splay_init(nlink);
	if (root == NULL) {
		return nlink;
	}

	while (fwd != NULL) {
		compr = __compare_wrap_v(nlink, fwd, comp, param);

		par = fwd;
		if (compr == 0)
			return root;
		if (compr < 0) 
			fwd = fwd->left;
		else 
			fwd = fwd->right;
	}

	if (compr < 0)
		par->left = nlink;
	else 
		par->right = nlink;
	nlink->parent = par;

	__splay(nlink);

	dbg_assert(nlink->parent == NULL);

	return nlink;
}

struct splaylink* splay_insert_s(struct splaylink* root, struct splaylink* nlink, pf_splay_compare comp, struct splaylink** dup) {
	struct splaylink* fwd = root;
	struct splaylink* par = NULL;
	int compr = 0;
	*dup = NULL;

	splay_init(nlink);
	if (root == NULL) {
		return nlink;
	}

	while (fwd != NULL) {
		compr = comp(nlink, fwd);

		par = fwd;
		if (compr == 0) {
			*dup = fwd;
			return root;
		}
		if (compr < 0) 
			fwd = fwd->left;
		else 
			fwd = fwd->right;
	}

	if (compr < 0)
		par->left = nlink;
	else 
		par->right = nlink;
	nlink->parent = par;

	__splay(nlink);

	dbg_assert(nlink->parent == NULL);

	return nlink;
}

struct splaylink* splay_insert_sv(struct splaylink* root, struct splaylink* nlink, pf_splay_compare_v comp, void* param, struct splaylink** dup) {
	struct splaylink* fwd = root;
	struct splaylink* par = NULL;
	int compr = 0;
	*dup = NULL;

	splay_init(nlink);
	if (root == NULL) {
		return nlink;
	}

	while (fwd != NULL) {
		compr = comp(nlink, fwd, param);

		par = fwd;
		if (compr == 0) {
			*dup = fwd;
			return root;
		}
		if (compr < 0) 
			fwd = fwd->left;
		else 
			fwd = fwd->right;
	}

	if (compr < 0)
		par->left = nlink;
	else 
		par->right = nlink;
	nlink->parent = par;

	__splay(nlink);

	dbg_assert(nlink->parent == NULL);

	return nlink;
}

struct splaylink* splay_search(struct splaylink* root, pf_splay_direct direct, void* param) {
	struct splaylink* fwd = root;

	while (fwd != NULL) {
		int comp_res = direct(fwd, param);

		if (comp_res == 0) {
			/* found */
			return fwd;
		}
		else if (comp_res < 0) {
			fwd = fwd->left;
		}
		else {
			fwd = fwd->right;
		}
	}

	return NULL;
}

struct splaylink* splay_dynamic_search(struct splaylink** p_root, pf_splay_direct direct, void* param) {
	struct splaylink* fwd = *p_root;

	while (fwd != NULL) {
		int comp_res = direct(fwd, param);

		if (comp_res == 0) {
			/* found */
			__splay(fwd);
			return *p_root = fwd;
		}
		else if (comp_res < 0) {
			fwd = fwd->left;
		}
		else {
			fwd = fwd->right;
		}
	}

	return NULL;
}

static void __splay_check(struct splaylink *c, pf_splay_compare comp) {
	struct splaylink *lc = c->left;
	struct splaylink *rc = c->right;
	struct splaylink *par = c->parent;
	int cmpr = 0;

	if (c == NULL)
		return;

	if (lc) {
		dbg_assert(lc->parent == c);

		cmpr = __compare_wrap(lc, c, comp);
		dbg_assert(cmpr == -1);
	}
	if (rc) {
		dbg_assert(rc->parent == c);

		cmpr = __compare_wrap(rc, c, comp);
		dbg_assert(cmpr == 1);
	}
	if (par) {
		dbg_assert(par->left == c || par->right == c);
	}

	if (c->left != NULL)
		__splay_check(c->left, comp);
	if (c->right != NULL)
		__splay_check(c->right, comp);
}

void splay_debug_check(struct splaylink *root, pf_splay_compare comp) {
	if (root == NULL)
		return;

	dbg_assert(root->parent == NULL);
	__splay_check(root, comp);
}

static void __splay_check_v(struct splaylink *c, pf_splay_compare_v comp_v, void* param) {
	struct splaylink *lc = c->left;
	struct splaylink *rc = c->right;
	struct splaylink *par = c->parent;
	int cmpr = 0;

	if (c == NULL)
		return;

	if (lc) {
		dbg_assert(lc->parent == c);

		cmpr = __compare_wrap_v(lc, c, comp_v, param);
		dbg_assert(cmpr == -1);
	}
	if (rc) {
		dbg_assert(rc->parent == c);

		cmpr = __compare_wrap_v(rc, c, comp_v, param);
		dbg_assert(cmpr == 1);
	}
	if (par) {
		dbg_assert(par->left == c || par->right == c);
	}

	if (c->left != NULL)
		__splay_check_v(c->left, comp_v, param);

	if (c->right != NULL)
		__splay_check_v(c->right, comp_v, param);
}

void splay_debug_check_v(struct splaylink* root, pf_splay_compare_v comp_v, void* param) {
	if (root == NULL)
		return;

	dbg_assert(root->parent == NULL);
	__splay_check_v(root, comp_v, param);
}

struct splaylink* splay_min(struct splaylink* root) {
	if (root == NULL) return NULL;

	while (root->left != NULL) {
		root = root->left;
	}

	return root;
}

struct splaylink* splay_max(struct splaylink* root) {
	if (root == NULL) return NULL;

	while (root->right != NULL) {
		root = root->right;
	}

	return root;
}

struct splaylink* splay_predesessor(const struct splaylink* link, bool only_sub) {
	if (link->left != NULL) {
		/* find the max element in the left sub tree */
		struct splaylink* fwd = link->left;

		while (fwd->right != NULL) 
			fwd = fwd->right;
		return fwd;
	}
	else if (!only_sub && link->parent) {
		const struct splaylink* fwd = link;

		while (fwd->parent != NULL) {
			if (fwd->parent->right == fwd) break;
			fwd = fwd->parent;
		}

		return fwd->parent;
	}

	return NULL;
}

struct splaylink* splay_successor(const struct splaylink* link, bool only_sub) {
	if (link->right != NULL) {
		/* find the minimum element in the right sub tree */
		struct splaylink* fwd = link->right;

		while (fwd->left != NULL)
			fwd = fwd->left;

		return fwd;
	}
	else if (!only_sub && link->parent) {
		const struct splaylink* fwd = link;

		while (fwd->parent != NULL) {
			if (fwd->parent->left == fwd) break;
			fwd = fwd->parent;
		}

		return fwd->parent;
	}

	return NULL;
}
