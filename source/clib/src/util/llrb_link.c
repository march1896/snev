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

static pf_llrb_compare _key_comp;

static void llrb_set_compare(pf_llrb_compare pc) {
	_key_comp = pc;
}

static void llrb_init(struct llrb_link *n) {
	dbg_assert(n != NULL);

	n->left = NULL;
	n->right = NULL;
	n->parent = NULL;
	n->color = RED;
}


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
static int _compare(const struct llrb_link *l, const struct llrb_link *r) {
	int cmpr = _key_comp(l, r);
	if (cmpr != 0) return cmpr;

	if (l < r) 
		return -1;
	else if (l == r) 
		return 0;
	else 
		return 1;
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

struct llrb_link *_insert(struct llrb_link *c, struct llrb_link *n) {
	if (c == NULL) return n;

	{
		int compr = _compare(n, c);

		if (compr < 0) {
			c->left = _insert(c->left, n);
			c->left->parent = c;
		}
		else {
			c->right = _insert(c->right, n);
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
struct llrb_link *_delete(struct llrb_link *c, struct llrb_link *n) {
	int compr = _compare(n, c);

	if (compr < 0) {
		/* if n is not in the tree, we may crash here */
		if (!_is_red(c->left) && !_is_red(c->left->left))
			c = _move_red_left(c);
		c->left = _delete(c->left, n);
		if (c->left)
			c->left->parent = c;
	}
	else {
		if (_is_red(c->left))
			c = _rotate_right(c);
		compr = _compare(n, c);
		if (compr == 0 && c->right == NULL)
			return NULL;
		if (!_is_red(c->right) && !_is_red(c->right->left))
			c = _move_red_right(c);
		compr = _compare(n, c);
		if (compr == 0) {
			{
				struct llrb_link *suc = c->right;

				while (suc->left != NULL)
					suc = suc->left;

				dbg_assert(suc->right == NULL);

				c = _swap_link(c, suc);
			}
			c->right = _delete_min(c->right);
		}
		else {
			c->right = _delete(c->right, n);
			if (c->right)
				c->right->parent = c;
		}
	}

	return _fix_up(c);
}

struct llrb_link *llrb_insert(struct llrb_link *root, struct llrb_link *nlink, pf_llrb_compare pc) {
	llrb_set_compare(pc);

	llrb_init(nlink);
	root = _insert(root, nlink);

	root->color = BLACK;

	return root;
}

struct llrb_link *llrb_remove(struct llrb_link *root, struct llrb_link *n, pf_llrb_compare pc) {
	llrb_set_compare(pc);

	root = _delete(root, n);

	if (root)
		root->color = BLACK;

	return root;
}

static int black_depth;
static void _debug_check(struct llrb_link *c, int depth) {
	if (c == NULL) {
		if (black_depth == -1) black_depth = depth;
		else dbg_assert(depth == black_depth);
	}
	else {
		struct llrb_link *lc = c->left;
		struct llrb_link *rc = c->right;
		struct llrb_link *par = c->parent;

		if (lc) dbg_assert(lc->parent == c);
		if (rc) dbg_assert(rc->parent == c);
		if (par) dbg_assert(par->left == c || par->right == c);

		dbg_assert(!_is_red(rc));
		if (_is_red(par)) dbg_assert(!_is_red(c));

		if (!_is_red(c)) depth ++;

		_debug_check(c->left, depth);
		_debug_check(c->right, depth);
	}
}

void llrb_debug_check(struct llrb_link *root) {
	black_depth = -1;

	_debug_check(root, 0);
}
