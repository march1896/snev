#include <llrb.h>

#define BLACK 1
#define RED   0

static struct llrb_link *_rotate_left(struct llrb_link* n) {
	struct llrb_link* r = n->right;
	struct llrb_link* p = n->parent;

	/* r is RED since we only rotate for 3-node, rotate a non 3-node
	 * is not legal in LLRB */
	dbg_assert(r != NULL && r->color == RED);

	n->parent = r;
	r->left = n;

	n->right = r->left;
	if (r->left != NULL) 
		r->left->parent = n;

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

	dbg_assert(l != NULL);

	n->parent = l;
	l->right = n;

	n->left = l->right;
	if (l->right != NULL) l->right->parent = n;

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

static pf_llrb_compare _compare;

void llrb_set_compare(pf_llrb_compare pc) {
	_compare = pc;
}

static inline bool _is_red(const struct llrb_link *n) {
	if (n != NULL && n->color == RED) return true;

	return false;
}

static inline void _color_flip(struct llrb_link *n) {
	if (n == NULL || n->left == NULL || n->right == NULL) return;

	n->left->color = !n->left->color;
	n->right->color = !n->right->color;
	n->color = !n->color;
}

struct llrb_link *_fix_up(struct llrb_link *c) {
	if (_is_red(c->right) && !_is_red(c->left))
		c = _rotate_left(c);

	if (c->left && (_is_red(c->left) && _is_red(c->left->left)))
		c = _rotate_right(c);

	if (_is_red(c->left) && _is_red(c->right))
		_color_flip(c);

	return c;
}

struct llrb_link *_insert(struct llrb_link *c, struct llrb_link *n) {
	if (c == NULL) return n;

	{
		int compr = _compare(c, n);

		if (compr < 0)
			c->left = _insert(c->left, n);
		else 
			c->right = _insert(c->right, n);
	}

	return _fix_up(c);
}

struct llrb_link *llrb_insert(struct llrb_link *root, struct llrb_link *nlink) {
	root = _insert(root, nlink);

	root->color = BLACK;

	return root;
}

static struct llrb_link *_move_red_left(struct llrb_link *c) {
	_color_flip(c);

	if (c->right && _is_red(c->right->left)) {
		c->right = _rotate_right(c->right);
		c = _rotate_left(c->left);
		_color_flip(c);
	}

	return c;
}

static struct llrb_link *_move_red_right(struct llrb_link *c) {
	_color_flip(c);

	if (c->left && _is_red(c->left->left)) {
		c = _rotate_right(c);
		_color_flip(c);
	}

	return c;
}

struct llrb_link *_delete_min(struct llrb_link *c) {
	if (c->left == NULL) return NULL;

	if (c->left && (!_is_red(c->left) && !_is_red(c->left->left))) 
		c = _move_red_left(c);

	c->left = _delete_min(c->left);

	return _fix_up(c);
}

struct llrb_link *_delete(struct llrb_link *c, struct llrb_link *n) {
	int compr = _compare(c, n);

	if (compr < 0) {
		if (c->left && !_is_red(c->left) && !_is_red(c->left->left))
			c = _move_red_left(c);
	}
	else {
		c->right = llrb_remove(c->right, n);
	}

	return NULL;
}

struct llrb_link *llrb_remove(struct llrb_link *c, struct llrb_link *n) {
}

struct llrb_link *llrb_search(struct llrb_link *r, struct llrb_link *n) {
	return NULL;
}

void llrb_init(struct llrb_link *n) {
	dbg_assert(n != NULL);

	n->left = NULL;
	n->right = NULL;
	n->parent = NULL;
	n->color = RED;
}

