#include <_234tree.h>

#include <citer_base.h>
#include <citer_base.local.h>
#include <cntr_base.h>
#include <cntr_base.local.h>
#include <cntr_tree.h>
#include <cntr_algorithm.h>
#include <cntr_factory.h>

#include <memheap/heap_global.h>

/*
 * The 234 tree is not a good name since normally we don't want numbers to 
 * exist in the function or variable name, since 234 tree is a B tree that 
 * hold 2-node, 3-node, and 4-node, here we call 234 tree 
 * fbt(four node(most) tree)
 */

typedef struct __cntr_fbt_vtable {
	/* from cntr_base */
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

cntr cntr_create_as_fbt() {
	cntr_fbt* pb = (cntr_fbt*)malloc(sizeof(cntr_fbt));

	cntr_fbt_init(pb, NULL, NULL);

	return (cntr)pb;
}

cntr cntr_create_as_fbt_r(pf_preremove_cb prerm) {
	cntr_fbt* pb = (cntr_fbt*)malloc(sizeof(cntr_fbt));

	cntr_fbt_init(pb, prerm, NULL);

	return (cntr)pb;
}

cntr cntr_create_as_fbt_v(pf_compare_object comp) {
	cntr_fbt* pb = (cntr_fbt*)malloc(sizeof(cntr_fbt));

	cntr_fbt_init(pb, NULL, comp);

	return (cntr)pb;
}

cntr cntr_create_as_fbt_rv(pf_preremove_cb prerm, pf_compare_object comp) {
	cntr_fbt* pb = (cntr_fbt*)malloc(sizeof(cntr_fbt));

	cntr_fbt_init(pb, prerm, comp);

	return (cntr)pb;
}

static cattr cntr_fbt_attribute(cntr c) {
	return CNTR_ATTR_BASE;
}

static void  cntr_fbt_destroy(cntr c) {
	cntr_fbt_clear(c);

	free(c);
}

static void fbt_clear_traverse(fbt_node* pn, pf_preremove_cb __delete) {
	if (pn == NULL) return;

	{
		int i = 0;
		for (; i <= pn->dim; i ++) 
			fbt_clear_traverse(pn->children[i], __delete);
	}

	if (__delete) {
		int i = 0;
		for (; i < pn->dim; i ++) {
			__delete(pn->keys[i]);
		}
	}

	free(pn);
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

	/* _234_node_check(pb->root); */
	pb->size ++;
}

static void citer_fbt_to_prev(citer itr) {
	citer_base* cur = (citer_base*)itr;
	fbt_node* node = (fbt_node*)(cur->connection);
	int index = (int)(intptr_t)(cur->param_x);

	dbg_assert(node && index < node->dim);
	//dbg_assert(cur->__vt == &citer_fbt_vtable);

	cur->connection = (void*)_234_node_predecessor(node, index, &index);
	cur->param_x = (void*)(intptr_t)index;
}

static void citer_fbt_to_next(citer itr) {
	citer_base* cur = (citer_base*)itr;
	fbt_node* node = (fbt_node*)(cur->connection);
	int index = (int)(intptr_t)(cur->param_x);

	dbg_assert(node && index < node->dim);
	//dbg_assert(cur->__vt == &citer_fbt_vtable);

	cur->connection = (void*)_234_node_successor(node, index, &index);
	cur->param_x = (void*)(intptr_t)index;
}

static void* citer_fbt_get_ref(citer itr) {
	citer_base* cur = (citer_base*)itr;
	fbt_node* node = (fbt_node*)(cur->connection);
	int index = (int)(intptr_t)(cur->param_x);

	dbg_assert(node && index < node->dim);

	return node->keys[index];
}

static void citer_fbt_set_ref(citer itr, void* n_ref) {
	citer_base* cur = (citer_base*)itr;
	fbt_node* node = (fbt_node*)(cur->connection);
	int index = (int)(intptr_t)(cur->param_x);

	dbg_assert(node && index < node->dim);

	node->keys[index] = n_ref;
}

static cattr citer_fbt_attribute(citer itr) {
	return CITER_ATTR_BASE | CITER_ATTR_LINK;
}

static citer_base_vtable citer_fbt_vtable = {
	citer_fbt_attribute,

	citer_fbt_get_ref,
	citer_fbt_set_ref,
	citer_fbt_to_prev,
	citer_fbt_to_next,
};

void cntr_fbt_remove_proc(citer itr, void* param) {
	cntr_fbt* pb = (cntr_fbt*)param;
	fbt_node* pn = (fbt_node*)((citer_base*)itr)->connection;
	int index = (int)(intptr_t)((citer_base*)itr)->param_x;
	void *obj;

	dbg_assert(index < pn->dim);
	obj = pn->keys[index];

	pb->root = _234_node_delete(pn, index);
	pb->size --;

	/* _234_node_check(pb->root); */

	if (pb->prerm) 
		pb->prerm(obj);
}

static void  cntr_fbt_remove(cntr c, citer begin, citer end) {
	cntr_fbt* pb = (cntr_fbt*)c;
	
	_234_node_set_comp(pb->comp);
	citer_for_each_v(begin, end, cntr_fbt_remove_proc, (void*)pb);
}

static bool  cntr_fbt_find(cntr c, void* object, citer itr) {
	cntr_fbt* pb = (cntr_fbt*)c;
	citer_base* ci = (citer_base*)itr;
	fbt_node* n = NULL;
	
	int index;
	_234_node_set_comp(pb->comp);
	n = _234_node_find(pb->root, object, &index);

	ci->__vt = &citer_fbt_vtable;
	ci->connection = (void*)n;
	ci->param_x = (void*)(intptr_t)index;

	return n == NULL ? false : true;
}

static void  cntr_fbt_citer_begin(cntr c, citer itr) {
	cntr_fbt* pb = (cntr_fbt*)c;
	citer_base* ci = (citer_base*)itr;
	fbt_node *n = pb->root;

	while (n) {
		if (n->left == NULL) break;
		n = n->left;
	}

	ci->__vt = &citer_fbt_vtable;
	ci->connection = (void*)n;
	ci->param_x = (void*)0;
}

static void  cntr_fbt_citer_end(cntr c, citer itr) {
	cntr_fbt* pb = (cntr_fbt*)c;
	citer_base* ci = (citer_base*)itr;
	fbt_node *n = pb->root;

	while (n) {
		if (n->right == NULL) break;
		n = n->right;
	}

	ci->__vt = &citer_fbt_vtable;
	ci->connection = (void*)n;
	ci->param_x = n == NULL ? (void*)0 : (void*)(intptr_t)(n->dim-1);
}


