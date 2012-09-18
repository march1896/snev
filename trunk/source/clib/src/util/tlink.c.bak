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

/* 
 * If take n as the root node of a sub tree, a rotation will not change 
 * the root node color, in fact, the only way to change a subtree's root 
 * color is color flip, which is corresponding to spliting 4-node in 234
 * tree.
 */
static struct tlink *_rotate_left(struct tlink* n) {
	struct tlink* r = n->right;
	struct tlink* p = n->parent;

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

struct tlink *_fix_up(struct tlink *n) {
	if (_is_red(c->right) && _is_black(c->left))
		c = _rotate_left(c);

	if (_is_red(c->left)) {
		if (c->left && _is_red(c->left->left))
			c = _rotate_right(c);
	}

	if (_is_red(c->left) && _is_red(c->right))
		_color_flip(c);
}

/*
 * When we deal with a 4-node in the search path, there is a top-down method, 
 * which will process any 4-node when travering down to leaf. And a down-top
 * method, which will process 4-node when needing. 
 *
 * The advantage of the first method is one-pass and simpiler code.
 * The advantage of the second is it will not split 4-node when not necessary.
 *
 * Here we use the first method.
 *
 * For a recursive call(without a parent pointer, and no operations ops on a 
 * node's parent), there are also top-down operations as well as down-top operation,
 * top-down ops are posed before the recursive call, while down-top ops are posed 
 * after the recursive call.
 *
 */
struct tlink *tlink_insert_rec(struct tlink *c, struct tlink *n) {
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
	 *
	 * This operation should in front of visiting a child node, since 
	 * we are using a top-down way node split. And when we are processing
	 * the current node, it is sure the c->parent is slibing(c->parent) 
	 * are not both red(in 234 tree, parent is not a 4 node), since its 
	 * already processed when traversing down the tree. And this is the only
	 * operation to affect c's parent, according to 234 tree insertion.
	 *
	 * After this operation, two possiblities of c's children.
	 * 1, both of them are black.(it's a 2-node or pushed up, c's ancestor will handle)
	 * 2, left child is red, right is black.(llrb property)
	 */
	if (_is_red(c->left) && _is_red(c->right))
		_color_flip(c);

	{
		/* typical binary search tree insertion. */
		int compr = _compare(c, n);

		if (compr < 0)
			c->left = tlink_insert_rec(c->left, n);
		else 
			c->right = tlink_insert_rec(c->right, n);
	}

	/*
	 * before the insertion make, the right child of c is black, the left 
	 * child is red or black.
	 */
	if (_is_red(c->right)) {
		/* 
		 * Before insertion, we have make sure c is not the middle of a 4-node, that is 
		 * not both c's children are red, at most c->left is red 
		 *
		 * c->right is red implies the change happened in the right sub tree.
		 * Formally, c->right is a middle of 4-node, and it's pushed up(color flip). Now 
		 * if c->left is red, it could construct a legal 4-node which take c as the middle 
		 * node. But if c->left is black, which means c is a 2-node before insertation, 
		 * we should do a rotation to maintain the left-leaning property.
		 */
		if (_is_black(c->left))
			c = _rotate_left(c);
	}

	else {
		/* 
		 * Color change happened in the left subtree.
		 * This is most complicated part of insertation.
		 *
		 * This part deals with the situation that c and c->left construct a 2-node, and 
		 * there is a an element pushed from the first or the middle child of the 2-node.
		 */
		if (_is_red(c->left)) {
			/* 
			 * There is a possibility that c is a 2-node before insertation, and c->left's 
			 * color is changed by a color flip, now c and c->left construct a legal 2-node, 
			 * and both c->left's children are black, in this case we do nothing.
			 */

			/*
			 * Now the left case is that c and c->left is a 2-node before insertion, and 
			 * c->left's chilren's color changed after insertion.
			 * Luckly, if the color changed happened in c->left->right(which means that the 
			 * middle children of a 3-node has pushed an element up), it's already handled 
			 * by the subtree since, the previous operation ensures the left leaning property.
			 *
			 * So the last case is when an element is pushed up from the left children of a 
			 * 3-node, which means c->left->left's color is changed, use a right rotation to 
			 * handle this case.
			 */
			if (c->left && _is_red(c->left->left))
				c = _rotate_right(c);
		}
	}

	return c;
}

struct tlink *tlink_insert(struct tlink *root, struct tlink *nlink) {
	root = tlink_insert_rec(root, nlink);

	root->color = BLACK;
	return root;
}

/*
 * Removing a node inherits of the idea of removing node in binary search tree, when removing 
 * a node in a binary search tree, first we substitute a node with the predecessor or successor 
 * of the to be remove node, then remove the substituted leaf node.
 *
 * When we apply this process on a LLRB, using successor will be simplier, since the the successor
 * will be a leaf node(successor will not have a left child since if it has, the left child will 
 * be the successor, and the successor will not have a black right child according to red-black property,
 * not have a red right child according to left-leanning property.
 *
 * If the succesor is a red node, which means it's a part of a 3-node or 4-node, simply remove it.
 * When the successor is black, which means we should steal an element from its parent, then 
 * for the parent, it could steal an element from the successor's sibling or steal an element from 
 * its parent(the successor's grandparent), then recursive process up.
 *
 * Like insertion, deletion have two methods.
 * First is to maintain that there is no 2-node when traversing down to find the successor.
 * Second is first find the successor, then try to fix the tree recursivly.
 * The advantages and disadvantages are just the same as insertion.
 *
 * Here we choose the first method.
 * When applying the first method, we should only maintain that there is not 2-node from the target 
 * deleting node to its successor, and for path from the root to target, there is no need to maintain 
 * this property, but for the conciseness of coding, the LLRB paper processing all elements.
 */
struct tlink *tlink_remove(struct tlink *c, struct tlink *n) {

	int compr = _compare(c, n);

	if (compr < 0) {
		/* 
		 * the target is in the left sub tree, here we should make sure that c->left is not a 2-node,
		 *
		 * It is also possible to handle c itself but not c->left, since we have store parent pointer.
		 */
	}
	else {
		c->right = tlink_remove(c->right, n);
	}

	return NULL;
}

struct tlink *_tlink_remove_min(struct tlink *c) {
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
