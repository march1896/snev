#include <tlink.h>

#define BLACK 1
#define RED   0

static struct tlink *_grandparent(const struct tlink* n) {
	if (n->parent) 
		return n->parent->parent;

	return NULL;
}

static struct tlink *_uncle(const struct tlink* n) {
	struct tlink* g = _grandparent(n);

	if (g != NULL) {
		if (n->parent == g->left) 
			return g->right;
		else 
			return g->left;
	}

	return NULL;
}

static struct tlink* _sibling(const struct tlink* n) {
	dbg_assert(n->parent);

	if (n == n->parent->left)
		return n->parent->right;
	else 
		return n->parent->left;
}

static struct tlink *_rotate_left(struct tlink* n) {
	struct tlink* r = n->right;
	struct tlink* p = n->parent;

	/* r is RED since we only rotate for 3-node, rotate a non 3-node
	 * is not legal */
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

static struct tlink *_rotate_right(struct tlink *n) {
	struct tlink *l = n->left;
	struct tlink *p = n->parent;

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

static inline bool _is_leaf(const struct tlink *n) {
	if (n->left == NULL && n->right == NULL)
		return true;

	return false;
}

static inline bool _is_red(const struct tlink *n) {
	if (n != NULL && n->color == RED) return true;

	return false;
}

static inline bool _is_black(const struct tlink *n) {
	if (n != NULL && n->color == BLACK) return true;

	return false;
}

static inline void _color_flip(struct tlink *n) {
	n->left->color = !n->left->color;
	n->right->color = !n->right->color;
	n->color = !n->color;
}

static pf_tlink_compare _compare;

void tlink_set_compare(pf_tlink_compare pc) {
	_compare = pc;
}

struct tlink *tlink_insert(struct tlink *c, struct tlink *n) {
	if (c == NULL) {
		/* make sure the link is inited */
		dbg_assert(n != NULL && 
				n->left == NULL &&
				n->right == NULL && 
				n->parent == NULL &&
				n->color == RED);

		return n;
	}

	/* 
	 * if both children of c are red links, this means c is the 
	 * middle element of a 4-node in 234tree terminology, we deal 
	 * it as the same of 234 tree (pop the middle element up to the 
	 * parent node), in llrb, simply flip color is enough 
	 */
	if (_is_red(c->left) && _is_red(c->right)) {
		_color_flip(c);
	}

	{
		/*
		 * typical binary search tree insertion.
		 * when we arrive here, we could make sure that not both parent and
		 * uncle are red, (in 234 node terminology, parent is not a 4-node)
		 */
		int compr = _compare(c, n);

		if (compr < 0)
			c->left = tlink_insert(c->left, n);
		else 
			c->right = tlink_insert(c->right, n);
	}

	/*
	 * before the insertion make, the right child of c is black, the left 
	 * child is red or black.
	 */

	/* 
	 * after the insertion, only of children's color may changed,
	 * 1, if change happens in the right child, which mean the right child changed
	 *    from black to red, at this time, if the left child is black, we should 
	 *    do a left rotation to make the left-lean property,(which means the 3-node
	 *    in 234 tree is always lean to left in llrb) if the left is red, we make 
	 *    no change, since c is now the middle element of a 4-node.
	 * 2, if change happens in the left child.
	 *    i, if the left child changes from red to black, we simple leave it there,
	 *       since now the right child must be black.
	 *    ii,if the left child changes from black to red, because the left sub tree
	 *       satisfies llrb property, so after insertion the left child has only 
	 *       a left red child(right is black), or has two same color children.
	 *       and we could also excude the possibility that 
	 *       n->left == n->left->left == n->left->right == RED.
	 *       if n->left is RED before insertion, n->left->left and n->left->right
	 *       must be BLACK before insertion, they can not become RED simultaneously.
	 *       if n->left is BLACK before insertion, n->left's color could only be 
	 *       changed by color flip(which means n->left's children both become black
	 *       and could not be ref simultaneously), rotation never change the color 
	 *       of root.
	 */
	if (_is_black(c->left) && _is_red(c->right))
		c = _rotate_left(c);

	if (_is_red(c->left) && _is_red(c->left->left))
		c = _rotate_right(c);

	return c;
}

struct tlink *tlink_remove(struct tlink *r, struct tlink *n) {
	return NULL;
}

struct tlink *tlink_search(struct tlink *r, struct tlink *n) {
	return NULL;
}

void tlink_init(struct tlink *n) {
	dbg_assert(n != NULL);

	n->left = NULL;
	n->right = NULL;
	n->parent = NULL;
	n->color = RED;
}
