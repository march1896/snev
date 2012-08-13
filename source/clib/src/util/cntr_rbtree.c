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
	if (pn == NULL) return;

	rbt_clear_traverse(pn->left, __remove);
	rbt_clear_traverse(pn->right, __remove);

	if (__remove)
		__remove(pn->object);
	hfree(pn);
}

static void  cntr_rbt_clear(cntr c) {
	cntr_rbt* pb = (cntr_rbt*)c;

	rbt_clear_traverse(pb->root, pb->prerm);

	pb->root = NULL;
	pb->size = 0;
}

static int   cntr_rbt_size(cntr c) {
	cntr_rbt* pb = (cntr_rbt*)c;

	return pb->size;
}

static void cntr_rbt_add(cntr c, void* obj) {
	cntr_rbt* pb = (cntr_rbt*)c;
	rbt_node* n_node = (rbt_node*)halloc(sizeof(rbt_node));
	bool redup = false;

	n_node->parent = NULL;
	n_node->left = NULL;
	n_node->right = NULL;
	n_node->object = obj;

	if (pb->root == NULL) {
		dbg_assert(pb->size == 0);

		pb->root = n_node;
		pb->size ++;
	}
	else {
		rbt_node *fwd = pb->root;
		rbt_node *par = NULL;
		/* find a proper place */
		while (fwd != NULL) {
			int comp_res = __compare(pb, obj, fwd->object);

			dbg_assert(fwd->object);

			par = fwd;
			if (comp_res < 0) fwd = fwd->left;
			else fwd = fwd->right;

			if (comp_res == 0) redup = true;
		}
		dbg_assert(fwd == NULL);

		dbg_assert(par);

		if (!redup || (pb->flags & SET_MULTI_INS)) {
			/* link the node into tree */
			n_node->parent = par;

			if (__compare(pb, obj, par->object) < 0) {
				dbg_assert(par->left == NULL);
				par->left = n_node;
				n_node->parent = par;
			}
			else {
				dbg_assert(par->right == NULL);
				par->right = n_node;
				n_node->parent = par;
			}
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
}

static inline rbt_node* rbt_predesessor(const rbt_node* pn, bool only_sub) {
	if (pn->left) {
		rbt_node* fwd = pn->left;

		while (fwd->right) fwd = fwd->right;
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
	return NULL;
}

static inline rbt_node* rbt_successor(const rbt_node* pn, bool only_sub) {
	if (pn->right) {
		rbt_node* fwd = pn->right;

		while (fwd->left) fwd = fwd->left;

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
	return (pn->left == NULL && pn->right == NULL);
}

/* remove a single node from a rbt */
static void* rbt_remove(cntr_rbt* pb, rbt_node* pn) {
	void* ref_obj = pn->object;

	if (pn->left == NULL && pn->right == NULL) {
		if (pb->root == pn) pb->root = NULL;
		else {
			rbt_node* par = pn->parent;
			if (par->left == pn) par->left = NULL;
			else par->right = NULL;
		}
		hfree(pn);
		pb->size --;
	}
	else if (pn->left == NULL) {
		if (pb->root == pn) {
			dbg_assert(pn->parent == NULL);
			pb->root = pn->right;
			pn->right->parent = NULL;
		}
		else {
			rbt_node* par = pn->parent;
			if (par->left == pn) par->left = pn->right;
			else par->right = pn->right;

			pn->right->parent = par;
		}
		
		hfree(pn);
		pb->size --;
	}
	else if (pn->right == NULL) {
		if (pb->root == pn) {
			dbg_assert(pn->parent == NULL);
			pb->root = pn->left;
			pn->left->parent = NULL;
		}
		else {
			rbt_node* par = pn->parent;
			if (par->left == pn) par->left = pn->left;
			else par->right = pn->left;

			pn->left->parent = par;
		}
		
		hfree(pn);
		pb->size --;
	}
	else {
		rbt_node* alter = rbt_predesessor(pn, true);
		pn->object = alter->object;
		rbt_remove(pb, alter);
	}

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

	while (fwd != NULL) {
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

	while (root->left != NULL) {
		root = root->left;
	}

	return root;
}

static rbt_node* rbt_maximum(rbt_node* root) {
	if (root == NULL) return NULL;

	while (root->right != NULL) {
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


