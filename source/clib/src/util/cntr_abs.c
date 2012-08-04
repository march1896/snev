#include <cntr_abs.h>

#include <cntr_list.h>
#include <cntr_array.h>

typedef cntr (*pf_linear_cntr_create)(cntr c);
typedef void (*pf_linear_cntr_destroy)(cntr c);
typedef void (*pf_linear_cntr_clear)(cntr c);
typedef int  (*pf_linear_cntr_size)(cntr c);
typedef void* (*pf_linear_cntr_front)(cntr c);
typedef void* (*pf_linear_cntr_back)(cntr c);
typedef void (*pf_linear_cntr_add_front)(cntr c, void* obj);
typedef void (*pf_linear_cntr_add_back)(cntr c, void* obj);
typedef void* (*pf_linear_cntr_remove_front)(cntr c);
typedef void* (*pf_linear_cntr_remove_back)(cntr c);
typedef void (*pf_linear_cntr_citer_begin)(cntr c, citer itr);
typedef void (*pf_linear_cntr_citer_end)(cntr c, citer itr);

typedef struct linear_cntr_operations_t {
	pf_linear_cntr_destroy      destroy;
	pf_linear_cntr_clear        clear;
	pf_linear_cntr_size         size;
	pf_linear_cntr_front        front;
	pf_linear_cntr_back         back;
	pf_linear_cntr_add_front    add_front;
	pf_linear_cntr_add_back     add_back;
	pf_linear_cntr_remove_front remove_front;
	pf_linear_cntr_remove_back  remove_back;
	pf_linear_cntr_citer_begin  citer_begin;
	pf_linear_cntr_citer_end    citer_end;
} clinear_operations;

typedef struct linear_cntr_t {
	clinear_operations* __vt;

	unknown             __cont;
} clinear;

static clinear_operations clist_ops = {
	clist_destroy, /* deinit */
	clist_clear, /* clean */
	clist_size, /* size */
	clist_front, /* front */
	clist_back , /* back  */
	clist_add_front, /* add_front */
	clist_add_back , /* add_back  */
	clist_remove_front, /* remove_front */
	clist_remove_back , /* remove_back  */
	clist_citer_begin, /* citer_begin */
	clist_citer_end  , /* citer_end   */
};

static clinear_operations carray_ops = {
	carray_destroy, /* deinit */
	carray_clear, /* clean */
	carray_size, /* size */
	carray_front, /* front */
	carray_back , /* back  */
	carray_add_front, /* add_front */
	carray_add_back , /* add_back  */
	carray_remove_front, /* remove_front */
	carray_remove_back , /* remove_back  */
	carray_citer_begin, /* citer_begin */
	carray_citer_end  , /* citer_end   */
};

cntr clinear_as_list() {
	clist list = clist_create();

	clinear* cl = (clinear*)halloc(sizeof(clinear));
	cl->__vt = &clist_ops;
	cl->__cont = (unknown)list;

	return (cntr)cl;
}

cntr clinear_as_array() {
	carray ary = carray_create();

	clinear* cl = (clinear*)halloc(sizeof(clinear));
	cl->__vt = &carray_ops;
	cl->__cont = (unknown)ary;

	return (cntr)cl;
}

void clinear_destroy(cntr c) {
	clinear* cl = (clinear*)c;

	(cl->__vt)->destroy(cl->__cont);
	hfree(cl);
}

int clinear_size(cntr c) {
	clinear* cl = (clinear*)c;

	return (cl->__vt)->size(cl->__cont);
}

void clinear_clear(cntr c) {
	clinear* cl = (clinear*)c;

	(cl->__vt)->clear(cl->__cont);
}

void* clinear_front(cntr c) {
	clinear* cl = (clinear*)c;

	return (void*)(cl->__vt)->front(cl->__cont);
}

void* clinear_back(cntr c) {
	clinear* cl = (clinear*)c;

	return (void*)(cl->__vt)->back(cl->__cont);
}

void clinear_add_front(cntr c, void* obj) {
	clinear* cl = (clinear*)c;

	(cl->__vt)->add_front(cl->__cont, obj);
}

void clinear_add_back(cntr c, void* obj) {
	clinear* cl = (clinear*)c;

	(cl->__vt)->add_back(cl->__cont, obj);
}

void* clinear_remove_front(cntr c) {
	clinear* cl = (clinear*)c;

	return (void*)(cl->__vt)->remove_front(cl->__cont);
}

void* clinear_remove_back(cntr c) {
	clinear* cl = (clinear*)c;

	return (void*)(cl->__vt)->remove_front(cl->__cont);
}

void clinear_citer_begin(cntr c, citer* itr) {
	clinear* cl = (clinear*)c;

	(cl->__vt)->citer_begin(cl->__cont, itr);
}

void clinear_citer_end(cntr c, citer* itr) {
	clinear* cl = (clinear*)c;

	(cl->__vt)->citer_end(cl->__cont, itr);
}
