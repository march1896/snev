#include <cntr_array.h>
#include <cntr_iterator.h>
#include <cntr_iterator.local.h>

typedef struct cntr_array_t {
	int size;
	int capacity;
	int expand_size;
	unsigned flags;

	void** data;
} cntr_array;

carray carray_create() {
	return (carray)carray_create_v(default_init_capacity, default_expand_size);
}

carray carray_create_v(int init_capacity, int expand_size) {
	cntr_array* pca = (cntr_array*)halloc(sizeof(cntr_array));

	pca->size = 0;
	pca->capacity = init_capacity;
	pca->expand_size = expand_size;
	pca->flags = 0;
	pca->data = (void**)halloc(init_capacity * sizeof(void*));

	return (carray)pca;
}

void  carray_destroy    (carray ca) {
	cntr_array* pca = (cntr_array*)ca;

	hfree(pca->data);

	hfree(pca);
}

void  carray_clear      (carray ca) {
	cntr_array* pca = (cntr_array*)ca;

	pca->size = 0;
}

int   carray_size       (carray ca) {
	cntr_array* pca = (cntr_array*)ca;
	
	return pca->size;
}

void  carray_add        (carray ca, void* object) {
	carray_add_back(ca, object);
}

void* carray_front      (carray ca) {
	cntr_array* pca = (cntr_array*)ca;

	if (pca->size == 0) return NULL;

	return pca->data[0];
}

void* carray_back       (carray ca) {
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

void  carray_add_front  (carray ca, void* object) {
	cntr_array* pca = (cntr_array*)ca;

	dbg_assert(pca->capacity >= pca->size);
	if (pca->capacity == pca->size) __expand_capacity(pca);

	memory_move((char*)(pca->data + 1), (char*)(pca->data), pca->size * sizeof(void*) / sizeof(char));

	pca->data[0] = object;
	pca->size ++;
}

void  carray_add_back   (carray ca, void* object) {
	cntr_array* pca = (cntr_array*)ca;

	dbg_assert(pca->capacity >= pca->size);
	if (pca->capacity == pca->size) __expand_capacity(pca);

	pca->data[pca->size++] = object;
}

void* carray_remove_front(carray ca) {
	cntr_array* pca = (cntr_array*)ca;
	void* obj = pca->data[0];

	dbg_assert(pca->size > 0);

	pca->size --;
	memory_move((char*)(pca->data), (char*)(pca->data + 1), pca->size * sizeof(void*) / sizeof(char));
	return obj;
}

void* carray_remove_back (carray ca) {
	cntr_array* pca = (cntr_array*)ca;

	dbg_assert(pca->size > 0);
	return pca->data[--pca->size];
}

/* iterator related operations */

static void citer_local_to_prev(citer itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;

	void** ele = (void**)cur->connection;
	cur->connection = ele - 1;
}

static void citer_local_to_next(citer itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;

	void** ele = (void**)cur->connection;
	cur->connection = ele + 1;
}

/*
 * TODO: if no container information here, we could not justfy if it's valid
 * TODO: implement algorithm, test if we really valid method.
 */
static bool citer_local_valid(citer itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;
	void** ele = cur->connection;

	if (ele) return true;
	else return false;
}

static void* citer_local_get_ref(citer itr) {
	cntr_iterator* cur = (cntr_iterator*)itr;

	void** ele = (void**)cur->connection;
	dbg_assert(ele);

	return *ele;
}

static void citer_local_set_ref(citer itr, void* n_ref) {
	cntr_iterator* cur = (cntr_iterator*)itr;

	void** ele = (void**)cur->connection;
	dbg_assert(ele);

	*ele = n_ref;
}

static citer_interface carray_citer_operations = {
	citer_local_valid,
	citer_local_get_ref,
	citer_local_set_ref,
	citer_local_to_prev,
	citer_local_to_next,
};

void  carray_citer_begin(carray ca, citer ci) {
	cntr_iterator* itr = (cntr_iterator*)ci;
	cntr_array* pca = (cntr_array*)ca;

	itr->__vt = &carray_citer_operations;
	itr->connection = pca->size ? (void*)&pca->data[0] : NULL;
}

void  carray_citer_end  (carray ca, citer ci) {
	cntr_iterator* itr = (cntr_iterator*)ci;
	cntr_array* pca = (cntr_array*)ca;

	itr->__vt = &carray_citer_operations;
	itr->connection = pca->size ? (void*)&pca->data[pca->size-1] : NULL;
}

void  carray_remove     (carray ca, citer begin, citer end) {
	// TODO:
}

bool  carray_find       (carray ca, void* object, citer itr) {
	// TODO:
	return false;
}

