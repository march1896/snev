#include <cntr_factory.h>
#include <cntr_linear.local.h>
#include <citer_cont.local.h>

typedef void (*pf_cntr_array_assign_capacity)(cntr pca, int n_size);

typedef struct __cntr_array_vtable {
	/* from cntr base */
	pf_cntr_attribute           __attrib;

	pf_cntr_base_destroy        __destroy;
	pf_cntr_base_clear          __clear;
	pf_cntr_base_size           __size;
	pf_cntr_base_add            __add;
	pf_cntr_base_remove         __remove;
	pf_cntr_base_find           __find;
	pf_cntr_base_citer_begin    __citer_begin;
	pf_cntr_base_citer_end      __citer_end;

	/* from cntr linear */
	pf_cntr_linear_front        __front;
	pf_cntr_linear_back         __back;
	pf_cntr_linear_add_front    __add_front;
	pf_cntr_linear_add_back     __add_back;
	pf_cntr_linear_remove_front __remove_front;
	pf_cntr_linear_remove_back  __remove_back;

	/* array specific */
	pf_cntr_array_assign_capacity __assign_capacity;
} cntr_array_vtable;

static cattr cntr_array_attribute  (cntr ca);
static void  cntr_array_destroy    (cntr ca);
static void  cntr_array_clear      (cntr ca);
static int   cntr_array_size       (cntr ca);
static void  cntr_array_add        (cntr ca, void* object);
static void  cntr_array_remove     (cntr ca, citer begin, citer end);
static bool  cntr_array_find       (cntr ca, void* object, citer itr);
static void  cntr_array_citer_begin(cntr ca, citer itr);
static void  cntr_array_citer_end  (cntr ca, citer itr);

static void* cntr_array_front      (cntr ca);
static void* cntr_array_back       (cntr ca);
static void  cntr_array_add_front  (cntr ca, void* object);
static void  cntr_array_add_back   (cntr ca, void* object);
static void* cntr_array_remove_front(cntr ca);
static void* cntr_array_remove_back (cntr ca);

static void cntr_array_assign_capacity(cntr pca, int n_size);

static cntr_array_vtable cntr_array_ops = {
	cntr_array_attribute, /* __attrib */

	cntr_array_destroy, /* destroy */
	cntr_array_clear, /* clean */
	cntr_array_size, /* size */
	cntr_array_add,
	cntr_array_remove,
	cntr_array_find,
	cntr_array_citer_begin, /* citer_begin */
	cntr_array_citer_end  , /* citer_end   */

	cntr_array_front, /* front */
	cntr_array_back , /* back  */
	cntr_array_add_front, /* add_front */
	cntr_array_add_back , /* add_back  */
	cntr_array_remove_front, /* remove_front */
	cntr_array_remove_back , /* remove_back  */

	cntr_array_assign_capacity /* assign capacity */
};

typedef struct __cntr_array {
	cntr_array_vtable*      __vt;

	int                     size;
	int                     capacity;
	int                     expand_size;
	unsigned                flags;
	void**                  data;

	pf_preremove_cb         prerm;
} cntr_array;

#define default_init_capacity 64
#define default_expand_size 64
cntr cntr_create_as_array() {
	return (cntr)cntr_create_as_array_v(default_init_capacity, default_expand_size);
}

static void cntr_array_init(cntr_array* pca, int init_capacity, int expand_size, pf_preremove_cb prm) {
	pca->__vt = &cntr_array_ops; 
	pca->size = 0;
	pca->capacity = init_capacity;
	pca->expand_size = expand_size;
	pca->flags = 0;
	pca->prerm = prm;
	pca->data = (void**)halloc(init_capacity * sizeof(void*));
}

cntr cntr_create_as_array_v(int init_capacity, int expand_size) {
	cntr_array* pca = (cntr_array*)halloc(sizeof(cntr_array));

	cntr_array_init(pca, init_capacity, expand_size, NULL);

	return (cntr)pca;
}

cntr cntr_create_as_array_r(pf_preremove_cb pre_rm) {
	cntr_array* pca = (cntr_array*)halloc(sizeof(cntr_array));

	cntr_array_init(pca, default_init_capacity, default_expand_size, pre_rm);

	return (cntr)pca;
}

cntr cntr_create_as_array_rv(pf_preremove_cb pre_rm, int init_capacity, int expand_size) {
	cntr_array* pca = (cntr_array*)halloc(sizeof(cntr_array));

	cntr_array_init(pca, init_capacity, expand_size, pre_rm);

	return (cntr)pca;
}

static cattr cntr_array_attribute(cntr ca) {
	return CNTR_ATTR_BASE | CNTR_ATTR_LINEAR | CNTR_ATTR_ARRAY;
}

static void  cntr_array_destroy    (cntr ca) {
	cntr_array* pca = (cntr_array*)ca;

	hfree(pca->data);

	hfree(pca);
}

static void  cntr_array_clear      (cntr ca) {
	cntr_array* pca = (cntr_array*)ca;

	if (pca->prerm) {
		int i = 0;
		for (i = 0; i < pca->size; i ++)
			pca->prerm(pca->data[i]);
	}

	pca->size = 0;
}

static int   cntr_array_size       (cntr ca) {
	cntr_array* pca = (cntr_array*)ca;
	
	return pca->size;
}

static void  cntr_array_add        (cntr ca, void* object) {
	cntr_array_add_back(ca, object);
}

static void* cntr_array_front      (cntr ca) {
	cntr_array* pca = (cntr_array*)ca;

	if (pca->size == 0) return NULL;

	return pca->data[0];
}

static void* cntr_array_back       (cntr ca) {
	cntr_array* pca = (cntr_array*)ca;

	if (pca->size == 0) return NULL;

	return pca->data[pca->size - 1];
}

static void memory_move(char* dest, char* src, int size) {
	if (dest >= src && dest < src + size) {
		dest += size - 1;
		src += size - 1;

		while (size --) *(dest --) = *(src --);
		return;
	}

	while (size --) *(dest ++) = *(src ++);
}

static void __expand_capacity(cntr_array* pca) {
	void** n_data = NULL;

	pca->capacity += pca->expand_size;
	n_data = (void**)halloc(sizeof(void*) * pca->capacity);

	memory_move((char*)n_data, (char*)pca->data, pca->size * sizeof(void*) / sizeof(char));
	hfree((void*)pca->data);
	pca->data = n_data;
}

static void  cntr_array_add_front  (cntr ca, void* object) {
	cntr_array* pca = (cntr_array*)ca;

	dbg_assert(pca->capacity >= pca->size);
	if (pca->capacity == pca->size) __expand_capacity(pca);

	memory_move((char*)(pca->data + 1), (char*)(pca->data), pca->size * sizeof(void*) / sizeof(char));

	pca->data[0] = object;
	pca->size ++;
}

static void  cntr_array_add_back   (cntr ca, void* object) {
	cntr_array* pca = (cntr_array*)ca;

	dbg_assert(pca->capacity >= pca->size);
	if (pca->capacity == pca->size) __expand_capacity(pca);

	pca->data[pca->size++] = object;
}

static void* cntr_array_remove_front(cntr ca) {
	cntr_array* pca = (cntr_array*)ca;
	void* obj = pca->data[0];

	dbg_assert(pca->size > 0);

	pca->size --;
	memory_move((char*)(pca->data), (char*)(pca->data + 1), pca->size * sizeof(void*) / sizeof(char));
	return obj;
}

static void* cntr_array_remove_back (cntr ca) {
	cntr_array* pca = (cntr_array*)ca;

	dbg_assert(pca->size > 0);
	return pca->data[--pca->size];
}

/* iterator related operations */

static void citer_local_to_prev(citer itr) {
	citer_cont* cur = (citer_cont*)itr;

	void** ele = (void**)cur->connection;
	cur->connection = ele - 1;
}

static void citer_local_to_next(citer itr) {
	citer_cont* cur = (citer_cont*)itr;

	void** ele = (void**)cur->connection;
	cur->connection = ele + 1;
}

static cattr citer_local_attribute(citer itr) {
	return CITER_ATTR_BASE | CITER_ATTR_CONT;
}

static void* citer_local_get_ref(citer itr) {
	citer_cont* cur = (citer_cont*)itr;

	void** ele = (void**)cur->connection;
	dbg_assert(ele);

	return *ele;
}

static void citer_local_set_ref(citer itr, void* n_ref) {
	citer_cont* cur = (citer_cont*)itr;

	void** ele = (void**)cur->connection;
	dbg_assert(ele);

	*ele = n_ref;
}

static int citer_local_distance(const citer from, const citer to) {
	return (void**)((citer_cont*)to)->connection - (void**)((citer_cont*)from)->connection + 1;
}

static void citer_local_move_n(citer from, int dis) {
	citer_cont* cur = (citer_cont*)from;
	void** new_addr = (void**)cur->connection + dis;

	cur->connection = (void*)new_addr;
}

static citer_cont_vtable cntr_array_citer_operations = {
	citer_local_attribute,

	citer_local_get_ref,
	citer_local_set_ref,
	citer_local_to_prev,
	citer_local_to_next,

	citer_local_distance,
	citer_local_move_n
};

static void  cntr_array_citer_begin(cntr ca, citer ci) {
	citer_cont* itr = (citer_cont*)ci;
	cntr_array* pca = (cntr_array*)ca;

	itr->__vt = &cntr_array_citer_operations;
	itr->connection = pca->size ? (void*)&pca->data[0] : NULL;
}

static void  cntr_array_citer_end  (cntr ca, citer ci) {
	citer_cont* itr = (citer_cont*)ci;
	cntr_array* pca = (cntr_array*)ca;

	itr->__vt = &cntr_array_citer_operations;
	itr->connection = pca->size ? (void*)&pca->data[pca->size-1] : NULL;
}

static void  cntr_array_remove     (cntr ca, citer begin, citer end) {
	cntr_array* pca = (cntr_array*)ca;

	void** ppb = (void**)((citer_cont*)begin)->connection;
	void** ppe = (void**)((citer_cont*)end)->connection;

	int count = 0;

	dbg_assert(ppb >= pca->data && ppe < pca->data + pca->size);

	if (pca->prerm) {
		void** addr = ppb;
		for (; addr <= ppe; addr ++) {
			pca->prerm(*addr);
		}
	}

	count = ppe - ppb + 1;
	memory_move((char*)(ppb), (char*)(ppe + 1), 
		(((pca->data + pca->size) - ppe) - 1) * sizeof(void*) / sizeof(char));
	pca->size -= count;
}

static bool  cntr_array_find       (cntr ca, void* object, citer itr) {
	cntr_array* pca = (cntr_array*)ca;
	citer_cont* ci = (citer_cont*)itr;
	int i = 0;

	for (i = 0; i < pca->size; i ++) {
		if (*(pca->data + i) == object) {
			ci->connection = pca->data + i;
			ci->__vt = &cntr_array_citer_operations;

			return true;
		}
	}
	
	ci->connection = NULL;
	ci->__vt = NULL;
	return false;
}

static void cntr_array_assign_capacity(cntr ca, int n_size) {
	cntr_array* pca = (cntr_array*)ca;
	void** n_data = NULL;

	dbg_assert(n_size >= pca->size);

	pca->capacity = n_size;
	n_data = (void**)halloc(sizeof(void*) * pca->capacity);

	memory_move((char*)n_data, (char*)pca->data, pca->size * sizeof(void*) / sizeof(char));
	hfree((void*)pca->data);
	pca->data = n_data;
}
