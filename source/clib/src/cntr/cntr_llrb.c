#include <citer_base.h>
#include <citer_base.local.h>
#include <cntr_base.h>
#include <cntr_base.local.h>
#include <cntr_tree.h>
#include <cntr_algorithm.h>
#include <cntr_factory.h>

#include <util/llrb_link.h>

typedef struct __cntr_llrb_vtable {
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
} cntr_llrb_vtable;

/* actual virtual functions declarations */
static cattr cntr_llrb_attribute  (cntr c);
static void  cntr_llrb_destroy    (cntr c);
static void  cntr_llrb_clear      (cntr c);
static int   cntr_llrb_size       (cntr c);
static void  cntr_llrb_add        (cntr c, void* object);
static void  cntr_llrb_remove     (cntr c, citer begin, citer end);
static bool  cntr_llrb_find       (cntr c, void* object, citer itr);
static void  cntr_llrb_citer_begin(cntr c, citer itr);
static void  cntr_llrb_citer_end  (cntr c, citer itr);

static cntr_llrb_vtable cntr_llrb_ops = {
	cntr_llrb_attribute,      /* __attrib */
	cntr_llrb_destroy,        /* destroy */
	cntr_llrb_clear,          /* clean */
	cntr_llrb_size,           /* size */
	cntr_llrb_add,            /* add */
	cntr_llrb_remove,
	cntr_llrb_find,
	cntr_llrb_citer_begin,    /* citer_begin */
	cntr_llrb_citer_end  ,    /* citer_end   */
};

struct object_link {
	struct llrb_link  link;
	void*             object;
};

static void* object_from_llrb_link(const struct llrb_link* link) {
	struct object_link* obj_link = container_of(link, struct object_link, link);

	return obj_link->object;
}

typedef struct __cntr_llrb {
	cntr_llrb_vtable*            __vt;

	int                         size;
	unsigned int                flags;
	struct llrb_link*           root;
	pf_compare_object           comp;
	pf_preremove_cb             prerm;
} cntr_llrb;

static void cntr_llrb_init(cntr_llrb* pb, pf_preremove_cb rm, pf_compare_object comp) {
	pb->__vt = &cntr_llrb_ops;
	pb->size = 0;
	pb->flags = 0;
	pb->root = NULL;
	pb->comp = comp;
	pb->prerm = rm;
}

/* Is the below functions needed, create method without compare function, the only reason is 
 * that they are compare by the llrb_link's address, which is supported by the llrb_link, at 
 * least we should give comment on this TODO */
cntr cntr_create_as_llrb() {
	cntr_llrb* pb = (cntr_llrb*)halloc(sizeof(cntr_llrb));

	cntr_llrb_init(pb, NULL, NULL);

	return (cntr)pb;
}

cntr cntr_create_as_llrb_r(pf_preremove_cb prerm) {
	cntr_llrb* pb = (cntr_llrb*)halloc(sizeof(cntr_llrb));

	cntr_llrb_init(pb, prerm, NULL);

	return (cntr)pb;
}

cntr cntr_create_as_llrb_v(pf_compare_object comp) {
	cntr_llrb* pb = (cntr_llrb*)halloc(sizeof(cntr_llrb));

	cntr_llrb_init(pb, NULL, comp);

	return (cntr)pb;
}

cntr cntr_create_as_llrb_rv(pf_preremove_cb prerm, pf_compare_object comp) {
	cntr_llrb* pb = (cntr_llrb*)halloc(sizeof(cntr_llrb));

	cntr_llrb_init(pb, prerm, comp);

	return (cntr)pb;
}

static cattr cntr_llrb_attribute(cntr c) {
	return CNTR_ATTR_BASE;
}

static void  cntr_llrb_destroy(cntr c) {
	cntr_llrb_clear(c);

	hfree(c);
}

static void llrb_clear_traverse(struct llrb_link* link, pf_preremove_cb __remove) {
	struct object_link* obj_link;

	if (link == NULL) return;

	llrb_clear_traverse(link->left, __remove);
	llrb_clear_traverse(link->right, __remove);

	obj_link = container_of(link, struct object_link, link);
	/* first remove the object */
	if (__remove)
		__remove(obj_link->object);
	/* second delete the node itself */
	hfree(obj_link);
}

static void  cntr_llrb_clear(cntr c) {
	cntr_llrb* pb = (cntr_llrb*)c;

	llrb_clear_traverse(pb->root, pb->prerm);

	pb->root = NULL;
	pb->size = 0;
}

static int   cntr_llrb_size(cntr c) {
	cntr_llrb* pb = (cntr_llrb*)c;

	return pb->size;
}

static pf_compare_object static_comp_func = NULL;

static int llrb_compare_cb(const struct llrb_link* l, const struct llrb_link* r) {
	void* lo = object_from_llrb_link(l);
	void* ro = object_from_llrb_link(r);

	dbg_assert(static_comp_func != NULL);
	return static_comp_func(lo, ro);
}

static void cntr_llrb_add(cntr c, void* obj) {
	cntr_llrb* pb = (cntr_llrb*)c;
	struct object_link* obj_link = (struct object_link*)halloc(sizeof(struct object_link));

	obj_link->object = obj;

	/* TODO, really bad idea to use a static member */
	static_comp_func = pb->comp;
	pb->root = llrb_insert(pb->root, &obj_link->link, llrb_compare_cb);
	static_comp_func = NULL;

	pb->size ++;
}

static cattr citer_llrb_attribute(citer itr);
static void* citer_llrb_get_ref(citer itr);
static void  citer_llrb_set_ref(citer itr, void* n_ref);
static void  citer_llrb_to_prev(citer itr);
static void  citer_llrb_to_next(citer itr);

static citer_base_vtable citer_llrb_operations = {
	citer_llrb_attribute,

	citer_llrb_get_ref,
	citer_llrb_set_ref,
	citer_llrb_to_prev,
	citer_llrb_to_next,
};


void cntr_llrb_remove_proc(citer itr, void* param) {
	cntr_llrb* pb = (cntr_llrb*)param;
	struct llrb_link* link = (struct llrb_link*)((citer_base*)itr)->connection;
	struct object_link* obj_link = container_of(link, struct object_link, link);

	/* first pre remove the object */
	if (pb->prerm) 
		pb->prerm(obj_link->object);

	static_comp_func = pb->comp;
	pb->root = llrb_remove(pb->root, link, llrb_compare_cb);
	static_comp_func = NULL;

	/* second delete the target link */
	hfree(obj_link);

	pb->size --;
}

static void  cntr_llrb_remove(cntr c, citer begin, citer end) {
	citer_for_each_v(begin, end, cntr_llrb_remove_proc, (void*)c);
}

static cattr citer_llrb_attribute(citer itr) {
	return CITER_ATTR_BASE | CITER_ATTR_LINK;
}

static bool  cntr_llrb_find(cntr c, void* object, citer itr) {
	cntr_llrb* pb = (cntr_llrb*)c;
	citer_base* ci = (citer_base*)itr;

	struct llrb_link* fwd = pb->root;

	while (fwd != NULL) {
		int comp_res = pb->comp(object, object_from_llrb_link(fwd));

		if (comp_res == 0) {
			ci->__vt = &citer_llrb_operations;
			ci->connection = fwd;

			return true;
		}

		if (comp_res < 0) fwd = fwd->left;
		else fwd = fwd->right;
	}

	return false;
}

static void citer_llrb_to_prev(citer itr) {
	citer_base* cur = (citer_base*)itr;
	struct llrb_link* link = (struct llrb_link*)(cur->connection);

	dbg_assert(link);
	cur->connection = llrb_predesessor(link, false);
}

static void citer_llrb_to_next(citer itr) {
	citer_base* cur = (citer_base*)itr;
	struct llrb_link* link = (struct llrb_link*)(cur->connection);

	dbg_assert(link);
	cur->connection = llrb_successor(link, false);
}

static void* citer_llrb_get_ref(citer itr) {
	citer_base* cur = (citer_base*)itr;
	struct llrb_link* link = (struct llrb_link*)(cur->connection);

	dbg_assert(link);
	return object_from_llrb_link(link);
}

static void citer_llrb_set_ref(citer itr, void* n_ref) {
	citer_base* cur = (citer_base*)itr;
	struct llrb_link* link = (struct llrb_link*)(cur->connection);
	struct object_link* obj_link = container_of(link, struct object_link, link);

	dbg_assert(obj_link != NULL);
	obj_link->object = n_ref;
}

static void  cntr_llrb_citer_begin(cntr c, citer itr) {
	cntr_llrb* pb = (cntr_llrb*)c;
	citer_base* ci = (citer_base*)itr;

	ci->__vt = &citer_llrb_operations;
	ci->connection = llrb_min(pb->root);
}

static void  cntr_llrb_citer_end(cntr c, citer itr) {
	cntr_llrb* pb = (cntr_llrb*)c;
	citer_base* ci = (citer_base*)itr;

	ci->__vt = &citer_llrb_operations;
	ci->connection = llrb_max(pb->root);
}


