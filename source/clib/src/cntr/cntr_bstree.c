#include <citer_base.h>
#include <citer_base.local.h>
#include <cntr_base.h>
#include <cntr_base.local.h>
#include <cntr_tree.h>
#include <cntr_algorithm.h>
#include <cntr_factory.h>

typedef struct __bst_node {
	struct __bst_node* parent;
	struct __bst_node* left;
	struct __bst_node* right;

	void *object;
} bst_node;

typedef struct __cntr_bst_vtable {
	/* from cntr_base_vtable */
	pf_cntr_attribute           __attrib;

	pf_cntr_base_destroy        __destroy;
	pf_cntr_base_clear          __clear;
	pf_cntr_base_size           __size;
	pf_cntr_base_add            __add;
	pf_cntr_base_remove         __remove;
	pf_cntr_base_find           __find;
	pf_cntr_base_citer_begin    __citer_begin;
	pf_cntr_base_citer_end      __citer_end;
} cntr_bst_vtable;

static cattr cntr_bst_attribute  (cntr c);
static void  cntr_bst_destroy    (cntr c);
static void  cntr_bst_clear      (cntr c);
static int   cntr_bst_size       (cntr c);
static void  cntr_bst_add        (cntr c, void* object);
static void  cntr_bst_remove     (cntr c, citer begin, citer end);
static bool  cntr_bst_find       (cntr c, void* object, citer itr);
static void  cntr_bst_citer_begin(cntr c, citer itr);
static void  cntr_bst_citer_end  (cntr c, citer itr);

static cntr_bst_vtable cntr_bst_ops = {
	cntr_bst_attribute, /* __attrib */
	cntr_bst_destroy, /* destroy */
	cntr_bst_clear, /* clean */
	cntr_bst_size, /* size */
	cntr_bst_add, /* add */
	cntr_bst_remove,
	cntr_bst_find,
	cntr_bst_citer_begin, /* citer_begin */
	cntr_bst_citer_end  , /* citer_end   */
};

typedef struct __cntr_bst {
	cntr_bst_vtable*            __vt;

	int                         size;
	unsigned int                flags;
	bst_node*                   root;
	pf_compare_object           comp;
	pf_preremove_cb             prerm;
} cntr_bst;

static void cntr_bst_init(cntr_bst* pb, pf_preremove_cb rm, pf_compare_object comp) {
	pb->__vt = &cntr_bst_ops;
	pb->size = 0;
	pb->flags = 0;
	pb->root = NULL;
	pb->comp = comp;
	pb->prerm = rm;
}

static inline int __compare(const cntr_bst* pb, const void *lhs, const void *rhs) {
	if (pb->comp) return pb->comp(lhs, rhs);
	else return lhs < rhs; /* directly compare the address */
}

cntr cntr_create_as_bst() {
	cntr_bst* pb = (cntr_bst*)halloc(sizeof(cntr_bst));

	cntr_bst_init(pb, NULL, NULL);

	return (cntr)pb;
}

cntr cntr_create_as_bst_r(pf_preremove_cb prerm) {
	cntr_bst* pb = (cntr_bst*)halloc(sizeof(cntr_bst));

	cntr_bst_init(pb, prerm, NULL);

	return (cntr)pb;
}

cntr cntr_create_as_bst_v(pf_compare_object comp) {
	cntr_bst* pb = (cntr_bst*)halloc(sizeof(cntr_bst));

	cntr_bst_init(pb, NULL, comp);

	return (cntr)pb;
}

cntr cntr_create_as_bst_rv(pf_preremove_cb prerm, pf_compare_object comp) {
	cntr_bst* pb = (cntr_bst*)halloc(sizeof(cntr_bst));

	cntr_bst_init(pb, prerm, comp);

	return (cntr)pb;
}

static cattr cntr_bst_attribute(cntr c) {
	return CNTR_ATTR_BASE;
}

static void  cntr_bst_destroy(cntr c) {
	cntr_bst_clear(c);

	hfree(c);
}

static void bst_clear_traverse(bst_node* pn, pf_preremove_cb __remove) {
	if (pn == NULL) return;

	bst_clear_traverse(pn->left, __remove);
	bst_clear_traverse(pn->right, __remove);

	if (__remove)
		__remove(pn->object);
	hfree(pn);
}

static void  cntr_bst_clear(cntr c) {
	cntr_bst* pb = (cntr_bst*)c;

	bst_clear_traverse(pb->root, pb->prerm);

	pb->root = NULL;
	pb->size = 0;
}

static int   cntr_bst_size(cntr c) {
	cntr_bst* pb = (cntr_bst*)c;

	return pb->size;
}

static void cntr_bst_add(cntr c, void* obj) {
	cntr_bst* pb = (cntr_bst*)c;
	bst_node* n_node = (bst_node*)halloc(sizeof(bst_node));
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
		bst_node *fwd = pb->root;
		bst_node *par = NULL;
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

static inline bst_node* bst_predesessor(const bst_node* pn, bool only_sub) {
	if (pn->left) {
		bst_node* fwd = pn->left;

		while (fwd->right) fwd = fwd->right;
		return fwd;
	}
	else if (!only_sub && pn->parent) {
		const bst_node *fwd = pn;

		while (fwd->parent != NULL) {
			if (fwd->parent->right == fwd) break;
			fwd = fwd->parent;
		}

		return fwd->parent;
	}
	return NULL;
}

static inline bst_node* bst_successor(const bst_node* pn, bool only_sub) {
	if (pn->right) {
		bst_node* fwd = pn->right;

		while (fwd->left) fwd = fwd->left;

		return fwd;
	}
	else if (!only_sub && pn->parent) {
		const bst_node* fwd = pn;

		while (fwd->parent != NULL) {
			if (fwd->parent->left == fwd) break;
			fwd = fwd->parent;
		}

		return fwd->parent;
	}

	return NULL;
}

static inline bool bst_isleaf(const bst_node* pn) {
	return (pn->left == NULL && pn->right == NULL);
}

/* remove a single node from a bst */
static void* bst_remove(cntr_bst* pb, bst_node* pn) {
	void* ref_obj = pn->object;

	if (pn->left == NULL && pn->right == NULL) {
		if (pb->root == pn) pb->root = NULL;
		else {
			bst_node* par = pn->parent;
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
			bst_node* par = pn->parent;
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
			bst_node* par = pn->parent;
			if (par->left == pn) par->left = pn->left;
			else par->right = pn->left;

			pn->left->parent = par;
		}
		
		hfree(pn);
		pb->size --;
	}
	else {
		bst_node* alter = bst_predesessor(pn, true);
		pn->object = alter->object;
		bst_remove(pb, alter);
	}

	return ref_obj;
}

static void citer_bst_to_prev(citer itr) {
	citer_base* cur = (citer_base*)itr;
	bst_node* node = (bst_node*)(cur->connection);

	dbg_assert(node);
	cur->connection = bst_predesessor(node, false);
}

static void citer_bst_to_next(citer itr) {
	citer_base* cur = (citer_base*)itr;
	bst_node* node = (bst_node*)(cur->connection);

	dbg_assert(node);
	cur->connection = bst_successor(node, false);
}

static void* citer_bst_get_ref(citer itr) {
	citer_base* cur = (citer_base*)itr;
	bst_node* node = (bst_node*)(cur->connection);

	dbg_assert(node);
	return node->object;
}

static void citer_bst_set_ref(citer itr, void* n_ref) {
	citer_base* cur = (citer_base*)itr;
	bst_node* node = (bst_node*)(cur->connection);

	dbg_assert(node);
	node->object = n_ref;
}

static cattr citer_bst_attribute(citer itr) {
	return CITER_ATTR_BASE | CITER_ATTR_LINK;
}

static citer_base_vtable citer_bst_operations = {
	citer_bst_attribute,

	citer_bst_get_ref,
	citer_bst_set_ref,
	citer_bst_to_prev,
	citer_bst_to_next,
};

void cntr_bst_remove_proc(citer itr, void* param) {
	cntr_bst* pb = (cntr_bst*)param;
	bst_node* pn = (bst_node*)((citer_base*)itr)->connection;

	void* obj = bst_remove(pb, pn);
	if (pb->prerm) 
		pb->prerm(obj);
}

static void  cntr_bst_remove(cntr c, citer begin, citer end) {
	cntr_bst* pb = (cntr_bst*)c;
	
	citer_for_each_v(begin, end, cntr_bst_remove_proc, (void*)pb);
}

static bool  cntr_bst_find(cntr c, void* object, citer itr) {
	cntr_bst* pb = (cntr_bst*)c;
	citer_base* ci = (citer_base*)itr;

	bst_node* fwd = pb->root;

	while (fwd != NULL) {
		int comp_res = __compare(pb, object, fwd->object);

		if (comp_res == 0) {
			ci->__vt = &citer_bst_operations;
			ci->connection = fwd;
			
			return true;
		}

		if (comp_res < 0) fwd = fwd->left;
		else fwd = fwd->right;
	}

	return false;
}

static bst_node* bst_minimum(bst_node* root) {
	if (root == NULL) return NULL;

	while (root->left != NULL) {
		root = root->left;
	}

	return root;
}

static bst_node* bst_maximum(bst_node* root) {
	if (root == NULL) return NULL;

	while (root->right != NULL) {
		root = root->right;
	}

	return root;
}

static void  cntr_bst_citer_begin(cntr c, citer itr) {
	cntr_bst* pb = (cntr_bst*)c;
	citer_base* ci = (citer_base*)itr;

	ci->__vt = &citer_bst_operations;
	ci->connection = bst_minimum(pb->root);
}

static void  cntr_bst_citer_end(cntr c, citer itr) {
	cntr_bst* pb = (cntr_bst*)c;
	citer_base* ci = (citer_base*)itr;

	ci->__vt = &citer_bst_operations;
	ci->connection = bst_maximum(pb->root);
}

