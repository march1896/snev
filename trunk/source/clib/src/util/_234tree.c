#include <_234tree.h>

static pf_compare_function comp = NULL;

inline void _234_node_set_comp(pf_compare_function cpf) {
	comp = cpf;
}

static bool __node_isleaf(struct _234_node *n) {
	if (n->children[0] != NULL) {
		int d = 1;

		for (; d <= n->dim; d ++) 
			dbg_assert(d->children[d] != NULL);

		return true;
	}
	return false;
}

static __node_init(struct _234_node *n, void *key) {
	n->dim = 1;
	n->keys[0] = key;

	{
		// TODO: debug purpose only, remove when stable.
		int i;
		for (i = 0; i < 4; i ++) n->children[i] = NULL;
		n->keys[1] = n->keys[2] = NULL;
	}
}

/* c is NULL, or is the new created sibling RIGHT adjacent to the splitted node */
static struct _234_node* __node_insert(struct _234_node *n, void *key, struct _234_node *c) {
	dbg_assert(n->dim < 3);
	dbg_assert((c == NULL && n->left == NULL) || /* if n is leaf, no child will be added */
			(c != NULL && n->left != NULL)); /* if n is not leaf, exactly one child will be added */

	// node is 2 node or 3 node
	int i = 0;
	for (; i < n->dim; i ++) {
		if (comp(n->keys[i], key) > 0) {
			int j = i;
			for (; j < n->dim; j ++) 
				n->keys[j+1] = n->key[j];
			n->keys[i] = key;

			for (j = i + 1; j <= n->dim; j ++) 
				n->children[j+1] = n->children[j];
			n->children[i+1] = c;

			break;
		}
	}
	// insert back
	if (i == n->dim) {
		n->keys[n->dim] = key;
		n->children[n->dim + 1] = key;
	}

	n->dim ++;
}

static struct _234_node* __add_adjust(struct _234_node *n) {
	if (n->dim < 3) return n;

	{
		struct _234_node *s = (struct _234_node*)halloc(struct _234_node);
		struct _234_node *p = n->parent;
		void *popup = n->second;

		if (p == NULL) {
			/* n is root node, make a new root node */
			p = (struct _234_node*)halloc(struct _234_node);
			__node_init(p, n->second);

			p->left = n;
			p->midleft = s;
		}
		else {
			/* insert the middle element to the parent node */
			__node_insert(p, popup, s);
		}

		__node_init(s, n->third);

		s->parent = p;
		s->left = n->midright;
		if (n->midright) n->midright->parent = s;
		s->midleft = n->right;
		if (n->right) n->right->parent = s;

		n->dim = 1;

		return p;
	}
}

static struct _234_node* __node_sibling(struct _234_node *n, int side) {
	if (n == NULL || n->parent == NULL) return NULL;

	struct _234_node *p = n->parent;
	int i;

	for (i = 0; i <= p->dim; i ++) {
		if (n == p->children[i]) {
			if (side == 0) return i > 0 ? p->children[i-1] : NULL;
			else return i <= p->dim ? p->children[i+1] : NULL;
		}
	}

	dbg_assert(false);
}

static struct _234_node *__node_root(struct _234_node *n) {
	if (n == NULL) return NULL;

	if (n->parent == NULL) return n;

	return __node_root(n->parent);
}

struct _234_node *_234_node_find(struct _234_node *n, void *key, int *index) {
	int k = 0;

	if (n == NULL) return NULL;

	for (k = 0; k < n->dim; k ++) {
		int compr = comp(n->keys[k], key);

		if (compr == 0) { 
			*index = k;
			return n;
		}

		if (compr > 0) 
			return _234_node_find(n->children[k], key, index);
	}
	return _234_node_find(n->children[n->dim], key, index);
}

struct _234_node* _234_node_add(struct _234_node *n, void *key) {
	int i = 0;

	n = __add_adjust(n);

	if (__node_isleaf(n)) {
		__node_insert(n, key, NULL);
		return n;
	}

	for (i = 0; i < n->dim; i ++) {
		int compr = comp(n->keys[i], key);

		if (compr > 0) {
			_234_node_add(n->children[i], key);
			break;
		}
	}

	if (i == n->dim) _234_node_add(n->children[n->dim], key);

	return n;
}


static struct _234_node* __node_steal_sibling(struct _234_node *n);
/* delete the idx'th element of n, return the root of the tree */
struct _234_node* _234_node_delete(struct _234_node *n, int idx) {
	dbg_assert(idx < n->dim);

	if (!__node_isleaf(n)) {
		/* find a subtitute leaf(predecessor) node key */
		struct _234_node *fwd = n;

		/* while fwd is not leaf node, find the predecessor */
		while (fwd->left) {
			int i = 0;
			for (; i < fwd->dim; i ++) {
				int compr = comp(fwd->keys[i], n->keys[idx]);

				if (compr >= 0) {
					fwd = fwd->children[i];
					break;
				}
			}
			if (i == fwd->dim) fwd = fwd->children[fwd->dim];
		}

		dbg_assert(fwd && comp(fwd->keys[fwd->dim-1], n->keys[idx]) <= 0);
		n->keys[idx] = fwd->keys[fwd->dim-1];

		_234_node_delete(fwd, fwd->dim - 1);

		return __node_root(n);
	}
	else {
		dbg_assert(idx < n->dim);
		int i;

		if (n->dim > 1) {
			/* simplest case, leaf node contains more than one key, delete directly */
			for (i = idx + 1; i < n->dim; i ++) 
				n->keys[i-1] = n->keys[i];

			n->dim --;
			return __node_root(n);
		}
		else {
			if (n->parent == NULL) {
				/* also the a simple case, entire tree contains one node, delete key directly */
				for (i = idx + 1; i < n->dim; i ++) 
					n->keys[i-1] = n->keys[i];

				n->dim --;
				if (n->dim == 0) {
					hfree(n);
					return NULL;
				}
				else 
					return __node_root(n);
			}
			else {
				n = __node_steal_sibling(n);

				dbg_assert(n->dim > 1);

				/* try to delete n again */
				return _234_node_delete(n);
			}
		}
	}

	dbg_assert(false);
	return NULL;
}

static struct _234_node* __node_steal_sibling(struct _234_node *n) {
	struct _234_node *p = n->parent;
	int i = 0;

	if (n->dim > 1) return n;

	if (p == NULL) {
		/* n is the root node, we can not steal any element any more */
		/* this is the only case that the steal operation will FAIL, we should 
		 * not try to use this method when we know a node is root, so assert(false)*/
		dbg_assert(false);
		return n;
	}

	/* n is not root node, if n's sibling has more than one element, we could steal one of them, 
	 * if n's sibling has only one element but p has more than one element, we could fuse n and its sibling.
	 * if both n's sibling and p has only one element, recursively visit its parent */
	for (i = 0 ; i <= p->dim; i ++) if (p->children[i] == n) break;

	dbg_assert(n <= p->dim);

	/* one of siblings has more than one elements, steal one of them */
	if (i > 0 && p->children[i-1]->dim > 1) {
		/* left sibling has more than one element */
		struct _234_node *s = p->children[i-1];
		int j;

		/* insert the stealed node at the left most side of n */
		/* in fact, since n->dim == 1, we could avoid of using loop */
		for (j = 0; j < n->dim; j ++) n->keys[j+1] = n->keys[j];
		for (j = 0; j <= n->dim; j ++) n->children[j+1] = n->keys[j];

		n->key[0] = p->keys[i-1];
		p->keys[i-1] = s->keys[s->dim-1];

		n->left = s->children[s->dim];
		if (s->children[s->dim]) 
			s->children[s->dim]->parent = n;

		s->dim --;
		n->dim ++;

		return n;
	}
	else if (i < p->dim && p->children[i+1]->dim > 1) {
		struct _234_node *s = p->children[i+1];
		int j;

		dbg_assert(n->dim == 1);

		/* steal the left most element from sibling, and insert it at the right most pos if n */
		n->keys[n->dim] = p->keys[i];
		p->keys[i] = s->keys[0];

		n->midleft = s->left;
		if (s->left)
			s->left->parent = n;

		for (j = 1; j < s->dim; j ++) s->keys[j-1] = s->keys[j];
		for (j = 1; j <= s->dim; j ++) s->children[j-1] = s->children[j];

		s->dim --;
		n->dim ++;

		return n;
	}

	/* siblings have only one element(2-node), and parent have more than one element(3-node or 4-node), 
	 * steal one element from parent and fuse n and its sibling into a 4 node */
	if (p->dim > 1) {
		if (i > 0) {
			/* fuse n and n's left sibling */
			struct _234_node *s = p->children[i-1];
			int j;

			dbg_assert(s->dim == 1);

			s->second = p->keys[i-1];
			s->third = n->first;

			s->midright = n->first;
			if (n->first) n->first->parent = s;
			s->right = n->midleft;
			if (n->midleft) n->midleft->parent = s;

			for (j = i; j < p->dim; j ++) {
				p->keys[j-1] = p->keys[j];
				p->children[j-1] = p->children[j];
			}
			p->children[p->dim-1] = p->children[p->dim];
			p->dim --;

			s->dim = 3;

			hfree(n);

			return s;
		}
		else {
			struct _234_node *s = p->children[i+1];
			int j;

			dbg_assert(s->dim == 1);

			n->second = p->keys[i];
			n->third = s->first;

			n->midright = s->first;
			if (s->first) s->first->parent = n;
			n->right = s->midleft;
			if (s->midleft) s->midleft->parent = n;

			for (j = i + 1; j < p->dim; j ++) {
				p->keys[j-1] = p->keys[j];
				p->children[j-1] = p->children[j];
			}
			p->children[p->dim-1] = p->children[p->dim];
			p->dim --;

			n->dim = 3;

			hfree(s);

			return n;
		}
	}

	/* last possible case, both n, s and p are 2 nodes */
	{
		if (p->parent == NULL) {
			/* if p is root, fuse its two children into one */

			/* fuse a 2-node parent with two 2-node children into a 4-node */
			dbg_assert(p->midleft && p->left->dim == 1 && p->midleft->dim == 1);

			p->second = p->first;
			p->first = p->left->first;
			p->third = p->midleft->first;

			{
				struct _234_node *l = p->left, *r = p->midleft;
				dbg_assert(l == n || r == n);

				p->left = l->left;
				p->midleft = l->midleft;
				p->midright = r->left;
				p->right = r->midleft;

				l->left->parent = p;
				l->midleft->parent = p;
				r->left->parent = p;
				r->midleft->parent = p;
			}

			hfree(l);
			hfree(r);

			return p;
		}
		else {
			struct _234_node *np = __node_steal_sibling(p);
			int j;
			dbg_assert(np && np->dim > 1);

			return __node_steal_sibling(n);
		}

	}

	dbg_assert(false);
	return NULL;
}
