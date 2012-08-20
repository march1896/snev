
typedef struct __node {
	unsigned dim;
	struct __node *parent;
	union {
		struct {
			void *first;
			void *second;
			void *third;
		};
		void *keys[3];
	};
	union {
		struct {
			struct __node* left;
			struct __node* midleft;
			struct __node* midright;
			struct __node* right;
		};
		struct __node* children[4];
	};
} node;

typedef struct __tree {
	node         *root;
	int          size;
	unsigned     flags;
} tree;

static bool node_contains_key(node *n, void *key) {
	int k = 0;
	for (; k < n->dim; k ++)
		if (n->keys[k] == key) return true;

	return false;
}

static bool __node_isleaf(node *n) {
	if (n->children[0] != NULL) {
		int d = 1;

		for (; d <= n->dim; d ++) 
			dbg_assert(d->children[d] != NULL);

		return true;
	}
	return false;
}

static node *tree_find(node *n, void *key) {
	int k = 0;

	if (n == NULL) return NULL;

	for (k = 0; k < n->dim; k ++) {
		int compr = comp(n->keys[k], key);

		if (compr == 0) 
			return n;

		if (compr > 0) 
			return tree_find(n->children[k], key);
	}
	return tree_find(n->children[n->dim], key);
}

static __node_init(node *n, void *key) {
	n->dim = 1;
	n->keys[0] = key;

	{
		// TODO: debug purpose only, remove when stable.
		int i;
		for (i = 0; i < 4; i ++) n->children[i] = NULL;
		n->keys[1] = n->keys[2] = NULL;
	}
}

/* c is NULL, the new created sibling RIGHT adjacent to the splitted node */
static node* __node_insert(node *n, void *key, node *c) {
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

static node* __add_adjust(node *n) {
	if (n->dim < 3) return n;

	{
		node *s = (node*)halloc(node);
		node *p = n->parent;
		void *popup = n->second;

		if (p == NULL) {
			/* n is root node, make a new root node */
			p = (node*)halloc(node);
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
		s->midleft = n->right;

		n->dim = 1;

		return p;
	}
}

static node* tree_add(node *n, void *key) {
	int i = 0;

	n = __add_adjust(n);

	if (__node_isleaf(n)) {
		__node_insert(n, key, NULL);
		return n;
	}

	for (i = 0; i < n->dim; i ++) {
		int compr = comp(n->keys[i], key);

		if (compr > 0) {
			tree_add(n->children[i], key);
			break;
		}
	}

	if (i == n->dim) tree_add(n->children[n->dim], key);

	return n;
}

static node* __node_sibling(node *n, int side) {
	if (n == NULL || n->parent == NULL) return NULL;

	node *p = n->parent;
	int i;

	for (i = 0; i <= p->dim; i ++) {
		if (n == p->children[i]) {
			if (side == 0) return i > 0 ? p->children[i-1] : NULL;
			else return i <= p->dim ? p->children[i+1] : NULL;
		}
	}

	dbg_assert(false);
}

/* delete the idx'th element of n, return an valid node of the tree */
static node* tree_delete(node *n, int idx) {
	dbg_assert(idx < n->dim);

	if (!__node_isleaf(n)) {
		/* find a subtitute leaf(predecessor) node key */
		node *fwd = n;

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

		tree_delete(fwd, fwd->dim - 1);

		return n;
	}
	else {
		dbg_assert(idx < n->dim);
		int i;

		if (n->dim > 1) {
			/* simplest case, leaf node contains more than one key, delete directly */
			for (i = idx + 1; i < n->dim; i ++) 
				n->keys[i-1] = n->keys[i];
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
					return n;
			}
			else {
				node *p = n->parent;
				int i = 0;

				for (; i <= p->dim; i ++)
					if (p->children[i] == n) break;
				dbg_assert(n <= p->dim);

				/* one of siblings has more than one element, borrow one element */
				if (i > 0 && p->children[i-1]->dim > 1) {
					/* left sibling has more than one element */
					node *s = p->children[i-1];
					dbg_assert(__node_isleaf(s));

					n->key = p->keys[i-1];
					p->keys[i-1] = s->keys[s->dim-1];
					s->dim --;

					return n;
				}
				else if (i < p->dim && p->children[i+1]->dim > 1) {
					node *s = p->children[i+1];
					int j;

					dbg_assert(__node_isleaf(s));

					n->keys = p->keys[i];
					p->keys[i] = s->keys[0];

					for (j = 1; j < s->dim; j ++)
						s->keys[j-1] = s->keys[j];
					s->dim --;

					return n;
				}

				/* siblings have only one element, and parent have more than one element, 
				 * move one element from parent to one of siblings */
				if (p->dim > 1) {
					if (i > 0) {
						/* move one element to left sibling */
						node *s = p->children[i-1];
						int j;

						dbg_assert(s->dim == 1);

						s->keys[s->dim++] = p->keys[i-1];
						for (j = i; j < p->dim; j ++) {
							p->keys[j-1] = p->keys[j];
							p->children[j-1] = p->children[j];
						}
						p->children[p->dim-1] = p->children[p->dim];
						p->dim --;

						hfree(n);

						return p;
					}
					else {
						node *s = p->children[i+1];
						int j;
						dbg_assert(__node_isleaf(s));

						for (j = 0; j < s->dim; j ++) 
							s->keys[j+1] = s->keys[j];
						s->keys[0] = p->keys[i];

						for (j = i + 1; j < p->dim; j ++) {
							p->keys[j-1] = p->keys[j];
							p->children[j-1] = p->children[j];
						}
						p->children[p->dim-1] = p->children[p->dim];
						p->dim --;

						hfree(n);

						return p;
					}
				}

				/* both n and p has only one element */
			}
		}
		n->dim --;
	}
}

static node* __node_fuse(node *n) {
	if (n->parent == NULL) {
		/* n is root node */

	}
}
