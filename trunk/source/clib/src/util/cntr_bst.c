#include <citer_base.h>
#include <citer_base.local.h>
#include <cntr_base.h>
#include <cntr_base.local.h>
#include <cntr_algorithm.h>

typedef struct bst_node_t {
	struct bst_node_t* parent;
	struct bst_node_t* left;
	struct bst_node_t* right;

	void *object;
} bst_node;

typedef struct cntr_bst_vtable_t {
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

#define BST_MULTI_INS           (1 << 0)

typedef struct cntr_bst_t {
	cntr_bst_vtable*            __vt;

	int                         size;
	unsigned int                flags;
	bst_node*                   root;
	pf_compare_object           comp;
} cntr_bst;

cntr cntr_create_as_bst(pf_compare_object comp) {
	cntr_bst* pb = (cntr_bst*)halloc(sizeof(cntr_bst));

	pb->__vt = &cntr_bst_ops;
	pb->size = 0;
	pb->flags = 0;
	pb->root = NULL;
	pb->comp = comp;

	return (cntr)pb;
}

static cattr cntr_bst_attribute(cntr c) {
	return CNTR_ATTR_BASE;
}

static void  cntr_bst_destroy(cntr c) {
	cntr_bst_clear(c);

	hfree(c);
}

static void bst_clear_traverse(bst_node* pn) {
	if (pn == NULL) return;

	bst_clear_traverse(pn->left);
	bst_clear_traverse(pn->right);

	hfree(pn);
}

static void  cntr_bst_clear(cntr c) {
	cntr_bst* pb = (cntr_bst*)c;

	bst_clear_traverse(pb->root);

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
			bool comp_res = pb->comp(obj, fwd->object);

			dbg_assert(fwd->object);

			par = fwd;
			if (comp_res < 0) fwd = fwd->left;
			else fwd = fwd->right;

			if (comp_res == 0) redup = true;
		}

		dbg_assert(par);
		if (!redup || (pb->flags & BST_MULTI_INS)) {
			/* link the node into tree */
			n_node->parent = par;

			if (pb->comp(obj, par->object) < 0) {
				dbg_assert(par->left == NULL);
				par->left = n_node;
			}
			else {
				dbg_assert(par->right == NULL);
				par->right = n_node;
			}
			pb->size ++;
		}
		else {
			/* dont insert into tree, clean up */
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

	bst_remove(pb, pn);
}

static void  cntr_bst_remove(cntr c, citer begin, citer end) {
	cntr_bst* pb = (cntr_bst*)c;
	
	citer_for_each_v(begin, end, cntr_bst_remove_proc, (void*)pb);
	// TODO:
}

static bool  cntr_bst_find(cntr c, void* object, citer itr) {
	cntr_bst* pb = (cntr_bst*)c;
	citer_base* ci = (citer_base*)itr;

	bst_node* fwd = pb->root;

	while (fwd != NULL) {
		bool comp_res = pb->comp(object, fwd->object);

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

