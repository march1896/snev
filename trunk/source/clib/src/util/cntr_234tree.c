#include <_234tree.h>

#include <citer_base.h>
#include <citer_base.local.h>
#include <cntr_base.h>
#include <cntr_base.local.h>
#include <cntr_tree.h>
#include <cntr_algorithm.h>
#include <cntr_factory.h>

/*
 * The 234 tree is not a good name since normally we don't want numbers to 
 * exist in the function or variable name, since 234 tree is a B tree that 
 * hold 2-node, 3-node, and 4-node, here we call 234 tree 
 * fbt(four node(most) tree)
 */

typedef struct __fbt_node {
	struct __fbt_node* parent;
	struct __fbt_node* left;
	struct __fbt_node* right;

	void *object;
} fbt_node;

typedef struct __cntr_fbt_vtable {
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
} cntr_fbt_vtable;

static cattr cntr_fbt_attribute  (cntr c);
static void  cntr_fbt_destroy    (cntr c);
static void  cntr_fbt_clear      (cntr c);
static int   cntr_fbt_size       (cntr c);
static void  cntr_fbt_add        (cntr c, void* object);
static void  cntr_fbt_remove     (cntr c, citer begin, citer end);
static bool  cntr_fbt_find       (cntr c, void* object, citer itr);
static void  cntr_fbt_citer_begin(cntr c, citer itr);
static void  cntr_fbt_citer_end  (cntr c, citer itr);

static cntr_fbt_vtable cntr_fbt_ops = {
	cntr_fbt_attribute, /* __attrib */
	cntr_fbt_destroy, /* destroy */
	cntr_fbt_clear, /* clean */
	cntr_fbt_size, /* size */
	cntr_fbt_add, /* add */
	cntr_fbt_remove,
	cntr_fbt_find,
	cntr_fbt_citer_begin, /* citer_begin */
	cntr_fbt_citer_end  , /* citer_end   */
};

typedef struct _234_node fbt_node;

typedef struct __cntr_fbt {
	cntr_fbt_vtable*            __vt;

	int                         size;
	unsigned int                flags;

	fbt_node*                   root;
	pf_compare_object           comp;
	pf_preremove_cb             prerm;
} cntr_fbt;

static void cntr_fbt_init(cntr_fbt* pb, pf_preremove_cb rm, pf_compare_object comp) {
	pb->__vt = &cntr_fbt_ops;
	pb->size = 0;
	pb->flags = 0;
	pb->root = NULL;
	pb->comp = comp;
	pb->prerm = rm;
}

static inline int __compare(const cntr_fbt* pb, const void *lhs, const void *rhs) {
	if (pb->comp) return pb->comp(lhs, rhs);
	else return lhs < rhs; /* directly compare the address */
}

cntr cntr_create_as_fbt() {
	cntr_fbt* pb = (cntr_fbt*)halloc(sizeof(cntr_fbt));

	cntr_fbt_init(pb, NULL, NULL);

	return (cntr)pb;
}

cntr cntr_create_as_fbt_r(pf_preremove_cb prerm) {
	cntr_fbt* pb = (cntr_fbt*)halloc(sizeof(cntr_fbt));

	cntr_fbt_init(pb, prerm, NULL);

	return (cntr)pb;
}

cntr cntr_create_as_fbt_v(pf_compare_object comp) {
	cntr_fbt* pb = (cntr_fbt*)halloc(sizeof(cntr_fbt));

	cntr_fbt_init(pb, NULL, comp);

	return (cntr)pb;
}

cntr cntr_create_as_fbt_rv(pf_preremove_cb prerm, pf_compare_object comp) {
	cntr_fbt* pb = (cntr_fbt*)halloc(sizeof(cntr_fbt));

	cntr_fbt_init(pb, prerm, comp);

	return (cntr)pb;
}

static cattr cntr_fbt_attribute(cntr c) {
	return CNTR_ATTR_BASE;
}

static void  cntr_fbt_destroy(cntr c) {
	cntr_fbt_clear(c);

	hfree(c);
}

static void fbt_clear_traverse(fbt_node* pn, pf_preremove_cb __delete) {
	if (pn == NULL) return;

	{
		int i = 0;
		for (; i <= pn->dim; i ++) 
			fbt_clear_traverse(pn->children[i], __delete);
	}

	if (__delete) {
		for (; i < pn->dim; i ++) {
			__delete(pn->keys[i]);
		}
	}
	hfree(pn);
}

static void  cntr_fbt_clear(cntr c) {
	cntr_fbt* pb = (cntr_fbt*)c;

	fbt_clear_traverse(pb->root, pb->prerm);

	pb->root = NULL;
	pb->size = 0;
}

static int   cntr_fbt_size(cntr c) {
	cntr_fbt* pb = (cntr_fbt*)c;

	return pb->size;
}

static void cntr_fbt_add(cntr c, void* obj) {
	cntr_fbt* pb = (cntr_fbt*)c;

	_234_node_set_comp(pb->comp);
	pb->root = _234_node_add(pb->root, obj);
	pb->size ++;
}

static inline fbt_node* fbt_predesessor(const fbt_node* pn, bool only_sub) {
	if (pn->left) {
		fbt_node* fwd = pn->left;

		while (fwd->right) fwd = fwd->right;
		return fwd;
	}
	else if (!only_sub && pn->parent) {
		const fbt_node *fwd = pn;

		while (fwd->parent != NULL) {
			if (fwd->parent->right == fwd) break;
			fwd = fwd->parent;
		}

		return fwd->parent;
	}
	return NULL;
}

static inline fbt_node* fbt_successor(const fbt_node* pn, bool only_sub) {
	if (pn->right) {
		fbt_node* fwd = pn->right;

		while (fwd->left) fwd = fwd->left;

		return fwd;
	}
	else if (!only_sub && pn->parent) {
		const fbt_node* fwd = pn;

		while (fwd->parent != NULL) {
			if (fwd->parent->left == fwd) break;
			fwd = fwd->parent;
		}

		return fwd->parent;
	}

	return NULL;
}

/* remove a single node from a fbt */
static void* fbt_remove(cntr_fbt* pb, fbt_node* pn, int index) {
	void* ref_obj = pn->object;

	if (pn->left == NULL && pn->right == NULL) {
		if (pb->root == pn) pb->root = NULL;
		else {
			fbt_node* par = pn->parent;
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
			fbt_node* par = pn->parent;
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
			fbt_node* par = pn->parent;
			if (par->left == pn) par->left = pn->left;
			else par->right = pn->left;

			pn->left->parent = par;
		}
		
		hfree(pn);
		pb->size --;
	}
	else {
		fbt_node* alter = fbt_predesessor(pn, true);
		pn->object = alter->object;
		fbt_remove(pb, alter);
	}

	return ref_obj;
}

static void citer_fbt_to_prev(citer itr) {
	citer_base* cur = (citer_base*)itr;
	fbt_node* node = (fbt_node*)(cur->connection);

	dbg_assert(node);
	cur->connection = fbt_predesessor(node, false);
}

static void citer_fbt_to_next(citer itr) {
	citer_base* cur = (citer_base*)itr;
	fbt_node* node = (fbt_node*)(cur->connection);

	dbg_assert(node);
	cur->connection = fbt_successor(node, false);
}

static void* citer_fbt_get_ref(citer itr) {
	citer_base* cur = (citer_base*)itr;
	fbt_node* node = (fbt_node*)(cur->connection);

	dbg_assert(node);
	return node->object;
}

static void citer_fbt_set_ref(citer itr, void* n_ref) {
	citer_base* cur = (citer_base*)itr;
	fbt_node* node = (fbt_node*)(cur->connection);

	dbg_assert(node);
	node->object = n_ref;
}

static cattr citer_fbt_attribute(citer itr) {
	return CITER_ATTR_BASE | CITER_ATTR_LINK;
}

static citer_base_vtable citer_fbt_operations = {
	citer_fbt_attribute,

	citer_fbt_get_ref,
	citer_fbt_set_ref,
	citer_fbt_to_prev,
	citer_fbt_to_next,
};

void cntr_fbt_remove_proc(citer itr, void* param) {
	cntr_fbt* pb = (cntr_fbt*)param;
	fbt_node* pn = (fbt_node*)((citer_base*)itr)->connection;

	void* obj = fbt_remove(pb, pn);
	if (pb->prerm) 
		pb->prerm(obj);
}

static void  cntr_fbt_remove(cntr c, citer begin, citer end) {
	cntr_fbt* pb = (cntr_fbt*)c;
	
	citer_for_each_v(begin, end, cntr_fbt_remove_proc, (void*)pb);
}

static bool  cntr_fbt_find(cntr c, void* object, citer itr) {
	cntr_fbt* pb = (cntr_fbt*)c;
	citer_base* ci = (citer_base*)itr;

	fbt_node* fwd = pb->root;

	while (fwd != NULL) {
		int comp_res = __compare(pb, object, fwd->object);

		if (comp_res == 0) {
			ci->__vt = &citer_fbt_operations;
			ci->connection = fwd;
			
			return true;
		}

		if (comp_res < 0) fwd = fwd->left;
		else fwd = fwd->right;
	}

	return false;
}

static fbt_node* fbt_minimum(fbt_node* root) {
	if (root == NULL) return NULL;

	while (root->left != NULL) {
		root = root->left;
	}

	return root;
}

static fbt_node* fbt_maximum(fbt_node* root) {
	if (root == NULL) return NULL;

	while (root->right != NULL) {
		root = root->right;
	}

	return root;
}

static void  cntr_fbt_citer_begin(cntr c, citer itr) {
	cntr_fbt* pb = (cntr_fbt*)c;
	citer_base* ci = (citer_base*)itr;

	ci->__vt = &citer_fbt_operations;
	ci->connection = fbt_minimum(pb->root);
}

static void  cntr_fbt_citer_end(cntr c, citer itr) {
	cntr_fbt* pb = (cntr_fbt*)c;
	citer_base* ci = (citer_base*)itr;

	ci->__vt = &citer_fbt_operations;
	ci->connection = fbt_maximum(pb->root);
}


