#include <citer_base.h>
#include <citer_base.local.h>
#include <cntr_base.h>
#include <cntr_base.local.h>
#include <cntr_tree.h>
#include <cntr_algorithm.h>
#include <cntr_factory.h>

typedef struct __cntr_rbt_vtable {
	/* from cntr_linear_vtable */
	pf_cntr_attribute           __attrib;

	pf_cntr_base_destroy        __destroy;
	pf_cntr_base_clear          __clear;
	pf_cntr_base_size           __size;
	pf_cntr_base_add            __add;
	pf_cntr_base_remove         __remove;
	pf_cntr_base_find           __find;
	pf_cntr_base_citer_begin    __citer_begin;
	pf_cntr_base_citer_end      __citer_end;
} cntr_rbt_vtable;

static cattr cntr_rbt_attribute  (cntr c);
static void  cntr_rbt_destroy    (cntr c);
static void  cntr_rbt_clear      (cntr c);
static int   cntr_rbt_size       (cntr c);
static void  cntr_rbt_add        (cntr c, void* object);
static void  cntr_rbt_remove     (cntr c, citer begin, citer end);
static bool  cntr_rbt_find       (cntr c, void* object, citer itr);
static void  cntr_rbt_citer_begin(cntr c, citer itr);
static void  cntr_rbt_citer_end  (cntr c, citer itr);

static cntr_rbt_vtable cntr_rbt_ops = {
	cntr_rbt_attribute, /* __attrib */
	cntr_rbt_destroy, /* destroy */
	cntr_rbt_clear, /* clean */
	cntr_rbt_size, /* size */
	cntr_rbt_add, /* add */
	cntr_rbt_remove,
	cntr_rbt_find,
	cntr_rbt_citer_begin, /* citer_begin */
	cntr_rbt_citer_end  , /* citer_end   */
};

#define BLACK 0
#define RED   1

typedef struct __rbt_node {
	struct __rbt_node* parent;
	struct __rbt_node* left;
	struct __rbt_node* right;
	unsigned int       color;

	void *object;
} rbt_node;

static rbt_node __sentinel = {
	NULL,          /* parent */
	NULL,          /* left   */
	NULL,          /* right  */
	BLACK,
	NULL,          /* object */
};

static rbt_node* const SENT = &__sentinel;

typedef struct __cntr_rbt {
	cntr_rbt_vtable*            __vt;

	int                         size;
	unsigned int                flags;
	rbt_node*                   root;
	pf_compare_object           comp;
	pf_preremove_cb             prerm;
} cntr_rbt;

static void cntr_rbt_init(cntr_rbt* pb, pf_preremove_cb rm, pf_compare_object comp) {
	pb->__vt = &cntr_rbt_ops;
	pb->size = 0;
	pb->flags = 0;
	pb->root = NULL;
	pb->comp = comp;
	pb->prerm = rm;
}

static inline int __compare(const cntr_rbt* pb, const void *lhs, const void *rhs) {
	if (pb->comp) return pb->comp(lhs, rhs);
	else return lhs < rhs; /* directly compare the address */
}

cntr cntr_create_as_rbt() {
	cntr_rbt* pb = (cntr_rbt*)halloc(sizeof(cntr_rbt));

	cntr_rbt_init(pb, NULL, NULL);

	return (cntr)pb;
}

cntr cntr_create_as_rbt_r(pf_preremove_cb prerm) {
	cntr_rbt* pb = (cntr_rbt*)halloc(sizeof(cntr_rbt));

	cntr_rbt_init(pb, prerm, NULL);

	return (cntr)pb;
}

cntr cntr_create_as_rbt_v(pf_compare_object comp) {
	cntr_rbt* pb = (cntr_rbt*)halloc(sizeof(cntr_rbt));

	cntr_rbt_init(pb, NULL, comp);

	return (cntr)pb;
}

cntr cntr_create_as_rbt_rv(pf_preremove_cb prerm, pf_compare_object comp) {
	cntr_rbt* pb = (cntr_rbt*)halloc(sizeof(cntr_rbt));

	cntr_rbt_init(pb, prerm, comp);

	return (cntr)pb;
}

static cattr cntr_rbt_attribute(cntr c) {
	return CNTR_ATTR_BASE;
}

static void  cntr_rbt_destroy(cntr c) {
	cntr_rbt_clear(c);

	hfree(c);
}

static void rbt_clear_traverse(rbt_node* pn, pf_preremove_cb __remove) {
	if (pn == SENT) return;

	rbt_clear_traverse(pn->left, __remove);
	rbt_clear_traverse(pn->right, __remove);

	if (__remove)
		__remove(pn->object);
	hfree(pn);
}

static void  cntr_rbt_clear(cntr c) {
	cntr_rbt* pb = (cntr_rbt*)c;

	if (pb->root != NULL) {
		rbt_clear_traverse(pb->root, pb->prerm);

		pb->root = NULL;
		pb->size = 0;
	}
}

static int   cntr_rbt_size(cntr c) {
	cntr_rbt* pb = (cntr_rbt*)c;

	return pb->size;
}

rbt_node* _grandparent(const rbt_node* n) {
	if (n->parent) return n->parent->parent;

	return NULL;
}

rbt_node* _uncle(const rbt_node* n) {
	rbt_node* g = _grandparent(n);

	if (g) {
		if (n->parent == g->left) return g->right;
		else return g->left;
	}

	return NULL;
}

rbt_node* _sibling(rbt_node* n) {
	dbg_assert(n->parent);
	if (n == n->parent->left)
		return n->parent->right;
	else 
		return n->parent->left;
}

static void _rotate_left(rbt_node* n) {
	rbt_node* r = n->right;
	rbt_node* p = n->parent;
	dbg_assert(r != SENT);

	n->right = r->left;
	n->parent = r;

	/* TODO: should we update the sentinel parent for further use */
	if (r->left != SENT) r->left->parent = n;

	r->parent = p;
	r->left = n;

	if (p != NULL) {
		if (p->left == n) p->left = r;
		else p->right = r;
	}
}

static void _rotate_right(rbt_node* n) {
	rbt_node *l = n->left;
	rbt_node *p = n->parent;
	dbg_assert(l != SENT);

	n->left = l->right;
	n->parent = l;
	if (l->right != SENT) l->right->parent = n;

	l->parent = p;
	l->right = n;

	if (p != NULL) {
		if (p->left == n) p->left = l;
		else p->right = l;
	}
}

static int __total_length;

static void rbt_check_rec(rbt_node* n, int length, int pcolor) {
	if (n == SENT) {
		if (__total_length == -1) 
			__total_length = length;
		else 
			dbg_assert(length == __total_length);

		return;
	}

	if (n->color == BLACK) {
		rbt_check_rec(n->left, length + 1, BLACK);
		rbt_check_rec(n->right, length + 1, BLACK);
	}
	else {
		dbg_assert(pcolor != RED);
		rbt_check_rec(n->left, length, RED);
		rbt_check_rec(n->right, length, RED);
	}

	return;
}

static void rbt_check(rbt_node* n) {
	__total_length = -1;

	dbg_assert(n == NULL || n->color == BLACK);
	if (n != NULL) 
		rbt_check_rec(n, 0, BLACK);
}

static void rbt_add_adjust(rbt_node* n) {
	if (n->parent == NULL) {
		// root node.
		n->color = BLACK;
		return;
	}

	if (n->parent->color == BLACK) {
		// still valid.
		return;
	}

	{
		rbt_node* u = _uncle(n);
		rbt_node* g = _grandparent(n);

		if (u != SENT && u->color == RED) {
			dbg_assert(n->parent->color == RED);
			n->parent->color = BLACK;
			u->color = BLACK;
			g->color = RED;

			// TODO: after this change, tree root may change, pointer to tree root should also change.
			rbt_add_adjust(g);
			return;
		}

		if (n == n->parent->right && n->parent == g->left) {
			_rotate_left(n->parent);
			n = n->left;
		}
		else if (n == n->parent->left && n->parent == g->right) {
			_rotate_right(n->parent);
			n = n->right;
		}

		dbg_assert(n->parent->color == RED && g->color == BLACK);
		n->parent->color = BLACK;
		g->color = RED;
		if (n == n->parent->left) {
			dbg_assert(n->parent == g->left);
			_rotate_right(g);
		}
		else {
			dbg_assert(n->parent == g->right);
			_rotate_left(g);
		}
	}
}

static void cntr_rbt_add(cntr c, void* obj) {
	cntr_rbt* pb = (cntr_rbt*)c;
	rbt_node* n_node = (rbt_node*)halloc(sizeof(rbt_node));
	bool redup = false;

	n_node->parent = NULL;
	n_node->left = SENT;
	n_node->right = SENT;
	n_node->object = obj;
	n_node->color = RED;

	if (pb->root == NULL) {
		dbg_assert(pb->size == 0);

		pb->root = n_node;
		rbt_add_adjust(n_node);
		pb->size ++;
	}
	else {
		rbt_node *fwd = pb->root;
		rbt_node *par = NULL;
		/* find a proper place */
		while (fwd != SENT) {
			int comp_res = __compare(pb, obj, fwd->object);

			dbg_assert(fwd->object);

			par = fwd;
			if (comp_res < 0) fwd = fwd->left;
			else fwd = fwd->right;

			if (comp_res == 0) redup = true;
		}
		dbg_assert(fwd == SENT);

		dbg_assert(par);

		if (!redup || (pb->flags & SET_MULTI_INS)) {
			/* link the node into tree */
			n_node->parent = par;

			if (__compare(pb, obj, par->object) < 0) {
				dbg_assert(par->left == SENT);
				par->left = n_node;
				n_node->parent = par;
			}
			else {
				dbg_assert(par->right == SENT);
				par->right = n_node;
				n_node->parent = par;
			}
			rbt_add_adjust(n_node);
			pb->size ++;
		}
		else {
			/* reduplicated and single instance */
			if (pb->flags & SET_EQUAL_REPLACE) {
				void* old_obj = par->object;
				par->object = obj;
				if (pb->prerm) pb->prerm(old_obj);
			}
			else {
				if (pb->prerm) pb->prerm(obj);
			}

			hfree(n_node);
		}
	}

	/* since after adjusting, tree root may change */
	while (pb->root->parent != NULL) 
		pb->root = pb->root->parent;

	/* debug only */
	/* rbt_check(pb->root); */
}

static inline rbt_node* rbt_predesessor(const rbt_node* pn, bool only_sub) {
	if (pn->left != SENT) {
		rbt_node* fwd = pn->left;

		while (fwd->right != SENT) fwd = fwd->right;
		return fwd;
	}
	else if (!only_sub && pn->parent) {
		const rbt_node *fwd = pn;

		while (fwd->parent != NULL) {
			if (fwd->parent->right == fwd) break;
			fwd = fwd->parent;
		}

		return fwd->parent;
	}
	dbg_assert(false);
	return NULL;
}

static inline rbt_node* rbt_successor(const rbt_node* pn, bool only_sub) {
	if (pn->right != SENT) {
		rbt_node* fwd = pn->right;

		while (fwd->left != SENT) fwd = fwd->left;

		return fwd;
	}
	else if (!only_sub && pn->parent) {
		const rbt_node* fwd = pn;

		while (fwd->parent != NULL) {
			if (fwd->parent->left == fwd) break;
			fwd = fwd->parent;
		}

		return fwd->parent;
	}

	return NULL;
}

static inline bool rbt_isleaf(const rbt_node* pn) {
	return (pn->left == SENT && pn->right == SENT);
}

static void rbt_remove_adjust(rbt_node* n) {
	dbg_assert(n && n->color == BLACK);

	if (n->parent == NULL) return;

	{
		rbt_node *s = _sibling(n);
		dbg_assert(s);

		if (s->color == RED) {
			dbg_assert(n->parent->color == BLACK);
			n->parent->color = RED;
			s->color = BLACK;
			if (n == n->parent->left) 
				_rotate_left(n->parent);
			else 
				_rotate_right(n->parent);
		}

		/* case 3 and 4 */
		s = _sibling(n);
		/* s->color == BLACK */
		if (s->left->color == BLACK &&
			s->right->color == BLACK) {
			/* s has no children or both children are black */
			if (n->parent->color == BLACK) {
				s->color = RED;
				rbt_remove_adjust(n->parent);
				return;
			}
			else {
				s->color = RED;
				n->parent->color = BLACK;
				return;
			}
		}

		dbg_assert(s->color == BLACK);
		//dbg_assert(s->left && s->right && s->left->color != s->right->color);

		if (s->right->color == BLACK && n == n->parent->left) {
			dbg_assert(s->left->color == RED);
			s->color = RED;
			s->left->color = BLACK;
			_rotate_right(s);
		}
		if (s->left->color == BLACK && n == n->parent->right) {
			dbg_assert(s->right->color == RED);
			s->color = RED;
			s->right->color = BLACK;
			_rotate_left(s);
		}

		s = _sibling(n);
		s->color = s->parent->color;
		n->parent->color = BLACK;
		if (n == n->parent->left) {
			s->right->color = BLACK;
			_rotate_left(n->parent);
		}
		else {
			s->left->color = BLACK;
			_rotate_right(n->parent);
		}
	}
}

static void rbt_remove_one_child(rbt_node* n) {
	rbt_node *child = n->left == SENT ? n->right : n->left;
	
	if (n->parent) {
		if (n == n->parent->left) n->parent->left = child;
		else n->parent->right = child;
	}

	dbg_assert(child != SENT || child->parent == NULL);
	/* if child is SENT, change is parent for the following algorithm */
	child->parent = n->parent;

	if (n->color == RED) {
		/* nothing need to do, just remove the node */
	}
	else {
		/* n->color == BLACK */
		if (child->color == RED) {
			child->color = BLACK;
		}
		else {
			rbt_remove_adjust(child);
		}
	}

	if (child == SENT)
		child->parent = NULL;
	hfree(n);
	return;
}

/* remove a single node from a rbt */
static void* rbt_remove(cntr_rbt* pb, rbt_node* pn) {
	void* ref_obj = pn->object;

	dbg_assert(!pn->parent || (pn->parent->left != SENT || pn->parent->right != SENT));
	if (pn->left != SENT && pn->right != SENT) {
		rbt_node* alter = rbt_predesessor(pn, true);
		pn->object = alter->object;
		pn = alter;
		dbg_assert(!pn->parent || (pn->parent->left != SENT || pn->parent->right != SENT));
	}
	else if (pb->root == pn) {
		dbg_assert(pn->color == BLACK);
		pb->root = pn->left == SENT ? pn->right : pn->left;

		if (pb->root == SENT) pb->root = NULL;
	}

	rbt_remove_one_child(pn);

	while (pb->root && pb->root->parent != NULL) 
		pb->root = pb->root->parent;

	/* debug only */
	/* rbt_check(pb->root); */

	pb->size --;
	return ref_obj;
}

static void citer_rbt_to_prev(citer itr) {
	citer_base* cur = (citer_base*)itr;
	rbt_node* node = (rbt_node*)(cur->connection);

	dbg_assert(node);
	cur->connection = rbt_predesessor(node, false);
}

static void citer_rbt_to_next(citer itr) {
	citer_base* cur = (citer_base*)itr;
	rbt_node* node = (rbt_node*)(cur->connection);

	dbg_assert(node);
	cur->connection = rbt_successor(node, false);
}

static void* citer_rbt_get_ref(citer itr) {
	citer_base* cur = (citer_base*)itr;
	rbt_node* node = (rbt_node*)(cur->connection);

	dbg_assert(node);
	return node->object;
}

static void citer_rbt_set_ref(citer itr, void* n_ref) {
	citer_base* cur = (citer_base*)itr;
	rbt_node* node = (rbt_node*)(cur->connection);

	dbg_assert(node);
	node->object = n_ref;
}

static cattr citer_rbt_attribute(citer itr) {
	return CITER_ATTR_BASE | CITER_ATTR_LINK;
}

static citer_base_vtable citer_rbt_operations = {
	citer_rbt_attribute,

	citer_rbt_get_ref,
	citer_rbt_set_ref,
	citer_rbt_to_prev,
	citer_rbt_to_next,
};

void cntr_rbt_remove_proc(citer itr, void* param) {
	cntr_rbt* pb = (cntr_rbt*)param;
	rbt_node* pn = (rbt_node*)((citer_base*)itr)->connection;

	void* obj = rbt_remove(pb, pn);
	if (pb->prerm) 
		pb->prerm(obj);
}

static void  cntr_rbt_remove(cntr c, citer begin, citer end) {
	cntr_rbt* pb = (cntr_rbt*)c;
	
	citer_for_each_v(begin, end, cntr_rbt_remove_proc, (void*)pb);
}

static bool  cntr_rbt_find(cntr c, void* object, citer itr) {
	cntr_rbt* pb = (cntr_rbt*)c;
	citer_base* ci = (citer_base*)itr;

	rbt_node* fwd = pb->root;

	while (fwd != SENT) {
		int comp_res = __compare(pb, object, fwd->object);

		if (comp_res == 0) {
			ci->__vt = &citer_rbt_operations;
			ci->connection = fwd;
			
			return true;
		}

		if (comp_res < 0) fwd = fwd->left;
		else fwd = fwd->right;
	}

	return false;
}

static rbt_node* rbt_minimum(rbt_node* root) {
	if (root == NULL) return NULL;

	while (root->left != SENT) {
		root = root->left;
	}

	return root;
}

static rbt_node* rbt_maximum(rbt_node* root) {
	if (root == NULL) return NULL;

	while (root->right != SENT) {
		root = root->right;
	}

	return root;
}

static void  cntr_rbt_citer_begin(cntr c, citer itr) {
	cntr_rbt* pb = (cntr_rbt*)c;
	citer_base* ci = (citer_base*)itr;

	ci->__vt = &citer_rbt_operations;
	ci->connection = rbt_minimum(pb->root);
}

static void  cntr_rbt_citer_end(cntr c, citer itr) {
	cntr_rbt* pb = (cntr_rbt*)c;
	citer_base* ci = (citer_base*)itr;

	ci->__vt = &citer_rbt_operations;
	ci->connection = rbt_maximum(pb->root);
}


