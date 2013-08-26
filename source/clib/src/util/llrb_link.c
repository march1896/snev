#include <llrb_link.h>

#define BLACK 1
#define RED   0

static struct llrb_link *_rotate_left(struct llrb_link* n) {
	struct llrb_link* r = n->right;
	struct llrb_link* p = n->parent;

	dbg_assert(r != NULL && r->color == RED);

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

	r->color = n->color;
	n->color = RED;

	return r;
}

static struct llrb_link *_rotate_right(struct llrb_link *n) {
	struct llrb_link *l = n->left;
	struct llrb_link *p = n->parent;

	dbg_assert(l != NULL && l->color == RED);

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

	l->color = n->color;
	n->color = RED;

	return l;
}

static void llrb_init(struct llrb_link *n) {
	dbg_assert(n != NULL);

	n->left = NULL;
	n->right = NULL;
	n->parent = NULL;
	n->color = RED;
}

void llrb_swap_link(struct llrb_link **pa, struct llrb_link **pb) {
	struct llrb_link *a = *pa;
	struct llrb_link *b = *pb;
	if (a->parent == b) {
		llrb_swap_link(pb, pa);
		return;
	}
	else if (b->parent == a) {
		struct llrb_link *br = b->right;
		struct llrb_link *bl = b->left;
		struct llrb_link *ap = a->parent;
		struct llrb_link *bs = (b == a->left) ? a->right : a->left;
		int b_color = b->color;

		b->parent = ap;
		if (a->left == b) {
			b->left = a;
			b->right = bs;
		}
		else {
			dbg_assert(a->right == b);
			b->right = a;
			b->left = bs;
		}
		b->color = a->color;

		a->left = bl;
		a->right = br;
		a->parent = b;
		a->color = b_color;

		if (ap) {
			if (ap->left == a) ap->left = b;
			else ap->right = b;
		}
		if (bs) bs->parent = b;

		if (bl) bl->parent = a;
		if (br) br->parent = a;
	}
	else {
		struct llrb_link *ap = a->parent;
		struct llrb_link *al = a->left;
		struct llrb_link *ar = a->right;

		struct llrb_link *bp = b->parent;
		struct llrb_link *bl = b->left;
		struct llrb_link *br = b->right;

		struct llrb_link temp = *a;
		*a = *b;
		*b = temp;

		if (al) al->parent = b;
		if (ar) ar->parent = b;

		if (bl) bl->parent = a;
		if (br) br->parent = a;

		if (ap) {
			if (ap->left == a) ap->left = b;
			else ap->right = b;
		}

		if (bp) {
			if (bp->left == b) bp->left = a;
			else bp->right = a;
		}
	}
	dbg_assert(b != NULL && a != NULL);
	*pa = b;
	*pb = a;
}

/*
 * For llrb 23tree, the successor must be a leaf, that is, it can not have a right child,
 * this is determined by the left-leaning property
 */
static struct llrb_link *_swap_link(struct llrb_link *anc, struct llrb_link *des) {
	if (des->parent != anc) {
		/* swap c and its successor */
		struct llrb_link swp;
		struct llrb_link *par = des->parent;

		swp = *des;
		*des = *anc;
		if (anc->parent) {
			if (anc->parent->left == anc)
				anc->parent->left = des;
			else 
				anc->parent->right = des;
		}

		if (anc->left) 
			anc->left->parent = des;
		if (anc->right) 
			anc->right->parent = des;

		if (par->left == des) par->left = anc;
		else par->right = anc;

		*anc = swp;
		return des;
	}
	else {
		struct llrb_link *gp = anc->parent;
		struct llrb_link *sib = des == anc->left ? anc->right : anc->left;
		int des_color = des->color;

		dbg_assert(des->left == NULL && des->right == NULL);

		if (gp) {
			if (gp->left == anc)
				gp->left = des;
			else 
				gp->right = des;
		}
		if (sib) {
			sib->parent = des;
		}

		if (des == anc->left) {
			des->right = anc;
			des->left = anc;
		}
		else {
			des->left = sib;
			des->right = anc;
		}
		des->parent = gp;
		des->color = anc->color;

		anc->parent = des;
		anc->left = anc->right = NULL;
		anc->color = des_color;

		return des;
	}
	return NULL;
}

/*
 * use a <key, address> pair as the real key stored in the tree.
 */
static int __compare_wrap(const struct llrb_link* a, const struct llrb_link* b, pf_llrb_compare raw_comp) {
	int raw_result = raw_comp(a, b);

	if (raw_result != 0) return raw_result;

	if (a < b) 
		return -1;
	else if (a > b) 
		return 1;

	return 0;
}

static inline bool _is_red(const struct llrb_link *n) {
	if (n != NULL && n->color == RED) return true;

	return false;
}

static inline void _color_flip(struct llrb_link *n) {
	n->left->color = !n->left->color;
	n->right->color = !n->right->color;
	n->color = !n->color;
}

struct llrb_link *_fix_up(struct llrb_link *c) {
	if (_is_red(c->right) && !_is_red(c->left))
		c = _rotate_left(c);

	if (_is_red(c->left) && _is_red(c->left->left))
		c = _rotate_right(c);

	if (_is_red(c->left) && _is_red(c->right))
		_color_flip(c);

	return c;
}

static struct llrb_link *__llrb_insert(struct llrb_link *c, struct llrb_link *n, pf_llrb_compare comp) {
	if (c == NULL) return n;

	{
		int compr = __compare_wrap(n, c, comp);
		dbg_assert(compr != 0);

		if (compr < 0) {
			c->left = __llrb_insert(c->left, n, comp);
			c->left->parent = c;
		}
		else {
			c->right = __llrb_insert(c->right, n, comp);
			c->right->parent = c;
		}
	}

	return _fix_up(c);
}

/* insert single instance of the link by using the raw comparison function */
static struct llrb_link *__llrb_insert_s(struct llrb_link *c, struct llrb_link *n, pf_llrb_compare comp, bool* dup) {
	if (c == NULL) return n;

	{
		int compr = comp(n, c);

		if (compr == 0) {
			/* we don't have to do anything here */
			/* the __fix_up function will do nothing */
			*dup = true;
		}
		else if (compr < 0) {
			c->left = __llrb_insert_s(c->left, n, comp, dup);
			c->left->parent = c;
		}
		else {
			c->right = __llrb_insert_s(c->right, n, comp, dup);
			c->right->parent = c;
		}
	}

	return _fix_up(c);
}

static struct llrb_link *_move_red_left(struct llrb_link *c) {
	_color_flip(c);

	if (_is_red(c->right->left)) {
		c->right = _rotate_right(c->right);
		c = _rotate_left(c);
		_color_flip(c);
	}

	return c;
}

static struct llrb_link *_move_red_right(struct llrb_link *c) {
	_color_flip(c);

	if (_is_red(c->left->left)) {
		c = _rotate_right(c);
		_color_flip(c);
	}

	return c;
}

struct llrb_link *_delete_min(struct llrb_link *c) {
	if (c->left == NULL) return NULL;

	if (!_is_red(c->left) && !_is_red(c->left->left))
		c = _move_red_left(c);

	c->left = _delete_min(c->left);

	return _fix_up(c);
}

/*
 * TODO: the code from the paper does not handle the case that n is not 
 * found in the tree.
 */
struct llrb_link *__llrb_delete(struct llrb_link *c, struct llrb_link *n, pf_llrb_compare comp) {
	int compr = __compare_wrap(n, c, comp);

	if (compr < 0) {
		/* if n is not in the tree, we may crash here */
		if (!_is_red(c->left) && !_is_red(c->left->left))
			c = _move_red_left(c);
		c->left = __llrb_delete(c->left, n, comp);
		if (c->left)
			c->left->parent = c;
	}
	else {
		if (_is_red(c->left))
			c = _rotate_right(c);
		compr = __compare_wrap(n, c, comp);
		if (compr == 0 && c->right == NULL)
			return NULL;
		if (!_is_red(c->right) && !_is_red(c->right->left))
			c = _move_red_right(c);
		compr = __compare_wrap(n, c, comp);
		if (compr == 0) {
			{
				struct llrb_link *suc = c->right;

				while (suc->left != NULL)
					suc = suc->left;

				dbg_assert(suc->right == NULL);

				c = _swap_link(c, suc);
				//llrb_swap_link(&c, &suc);
			}
			c->right = _delete_min(c->right);
		}
		else {
			c->right = __llrb_delete(c->right, n, comp);
			if (c->right)
				c->right->parent = c;
		}
	}

	return _fix_up(c);
}

struct llrb_link *llrb_insert(struct llrb_link *root, struct llrb_link *nlink, pf_llrb_compare comp) {
	llrb_init(nlink);
	root = __llrb_insert(root, nlink, comp);

	root->color = BLACK;

	return root;
}

struct llrb_link* llrb_insert_s (struct llrb_link* root, struct llrb_link* nlink, pf_llrb_compare comp, bool* dup) {
	llrb_init(nlink);
	*dup = false;

	root = __llrb_insert_s(root, nlink, comp, dup);
	root->color = BLACK;

	return root;
}

struct llrb_link *llrb_remove(struct llrb_link *root, struct llrb_link *n, pf_llrb_compare comp) {
	root = __llrb_delete(root, n, comp);

	if (root)
		root->color = BLACK;

	return root;
}

static int __compare_wrap_v(const struct llrb_link* a, const struct llrb_link* b, pf_llrb_compare_v raw_comp, void* param) {
	int raw_result = raw_comp(a, b, param);

	if (raw_result != 0) return raw_result;

	if (a < b) 
		return -1;
	else if (a > b) 
		return 1;

	return 0;
}

struct llrb_link *__llrb_insert_v(struct llrb_link *c, struct llrb_link *n, pf_llrb_compare_v comp, void* param) {
	if (c == NULL) return n;

	{
		int compr = __compare_wrap_v(n, c, comp, param);

		if (compr < 0) {
			c->left = __llrb_insert_v(c->left, n, comp, param);
			c->left->parent = c;
		}
		else {
			c->right = __llrb_insert_v(c->right, n, comp, param);
			c->right->parent = c;
		}
	}

	return _fix_up(c);
}

struct llrb_link* llrb_insert_v (struct llrb_link* root, struct llrb_link* nlink, pf_llrb_compare_v comp, void* param) {
	llrb_init(nlink);
	root = __llrb_insert_v(root, nlink, comp, param);

	root->color = BLACK;

	return root;
}

struct llrb_link *__llrb_insert_sv(struct llrb_link *c, struct llrb_link *n, pf_llrb_compare_v comp, void* param, bool* dup) {
	if (c == NULL) return n;

	{
		int compr = comp(n, c, param);

		if (compr == 0) {
			/* we need to do nothing */
			*dup = true;
		}
		else if (compr < 0) {
			c->left = __llrb_insert_sv(c->left, n, comp, param, dup);
			c->left->parent = c;
		}
		else {
			c->right = __llrb_insert_sv(c->right, n, comp, param, dup);
			c->right->parent = c;
		}
	}

	return _fix_up(c);
}

struct llrb_link* llrb_insert_sv(struct llrb_link* root, struct llrb_link* nlink, pf_llrb_compare_v comp, void* param, bool* dup) {
	llrb_init(nlink);
	*dup = false;

	root = __llrb_insert_sv(root, nlink, comp, param, dup);

	root->color = BLACK;

	return root;
}

/* TODO: the code from the paper does not handle the case that n is not found in the tree. */
struct llrb_link *__llrb_delete_v(struct llrb_link *c, struct llrb_link *n, pf_llrb_compare_v comp, void* param) {
	int compr = __compare_wrap_v(n, c, comp, param);

	if (compr < 0) {
		/* if n is not in the tree, we may crash here */
		if (!_is_red(c->left) && !_is_red(c->left->left))
			c = _move_red_left(c);
		c->left = __llrb_delete_v(c->left, n, comp, param);
		if (c->left)
			c->left->parent = c;
	}
	else {
		if (_is_red(c->left))
			c = _rotate_right(c);
		compr = __compare_wrap_v(n, c, comp, param);
		if (compr == 0 && c->right == NULL)
			return NULL;
		if (!_is_red(c->right) && !_is_red(c->right->left))
			c = _move_red_right(c);
		compr = __compare_wrap_v(n, c, comp, param);
		if (compr == 0) {
			{
				struct llrb_link *suc = c->right;

				while (suc->left != NULL)
					suc = suc->left;

				dbg_assert(suc->right == NULL);

				c = _swap_link(c, suc);
				//llrb_swap_link(&c, &suc);
			}
			c->right = _delete_min(c->right);
		}
		else {
			c->right = __llrb_delete_v(c->right, n, comp, param);
			if (c->right)
				c->right->parent = c;
		}
	}

	return _fix_up(c);
}

struct llrb_link* llrb_remove_v (struct llrb_link* root, struct llrb_link* target, pf_llrb_compare_v comp, void* param) {
	root = __llrb_delete_v(root, target, comp, param);

	if (root)
		root->color = BLACK;

	return root;
}

struct llrb_link* llrb_search(struct llrb_link* root, pf_llrb_direct direct, void* param) {
	struct llrb_link* fwd = root;

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

static int black_depth;
static void __llrb_check(struct llrb_link *c, int depth, pf_llrb_compare comp) {
	if (c == NULL) {
		if (black_depth == -1) black_depth = depth;
		else dbg_assert(depth == black_depth);
	}
	else {
		struct llrb_link *lc = c->left;
		struct llrb_link *rc = c->right;
		struct llrb_link *par = c->parent;
		int cmpr = 0;

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

		if (par) dbg_assert(par->left == c || par->right == c);

		dbg_assert(!_is_red(rc));
		if (_is_red(par)) dbg_assert(!_is_red(c));

		if (!_is_red(c)) depth ++;

		__llrb_check(c->left, depth, comp);
		__llrb_check(c->right, depth, comp);
	}
}

void llrb_debug_check(struct llrb_link *root, pf_llrb_compare comp) {
	black_depth = -1;
	__llrb_check(root, 0, comp);
}

struct llrb_link* llrb_min(struct llrb_link* root) {
	if (root == NULL) return NULL;

	while (root->left != NULL) {
		root = root->left;
	}

	return root;
}

struct llrb_link* llrb_max(struct llrb_link* root) {
	if (root == NULL) return NULL;

	while (root->right != NULL) {
		root = root->right;
	}

	return root;
}

struct llrb_link* llrb_predesessor(const struct llrb_link* link, bool only_sub) {
	if (link->left != NULL) {
		/* find the max element in the left sub tree */
		struct llrb_link* fwd = link->left;

		while (fwd->right != NULL) fwd = fwd->right;
		return fwd;
	}
	else if (!only_sub && link->parent) {
		const struct llrb_link* fwd = link;

		while (fwd->parent != NULL) {
			if (fwd->parent->right == fwd) break;
			fwd = fwd->parent;
		}

		return fwd->parent;
	}

	return NULL;
}

struct llrb_link* llrb_successor(const struct llrb_link* link, bool only_sub) {
	if (link->right != NULL) {
		/* find the minimum element in the right sub tree */
		struct llrb_link* fwd = link->right;

		while (fwd->left != NULL) fwd = fwd->left;

		return fwd;
	}
	else if (!only_sub && link->parent) {
		const struct llrb_link* fwd = link;

		while (fwd->parent != NULL) {
			if (fwd->parent->left == fwd) break;
			fwd = fwd->parent;
		}

		return fwd->parent;
	}

	return NULL;
}
