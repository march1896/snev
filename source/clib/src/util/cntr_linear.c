#include <cntr_linear.h>
#include <cntr.local.h>

#include <cntr_list.h>
#include <cntr_array.h>

typedef void* (*pf_linear_cntr_front)(cntr c);
typedef void* (*pf_linear_cntr_back)(cntr c);
typedef void (*pf_linear_cntr_add_front)(cntr c, void* obj);
typedef void (*pf_linear_cntr_add_back)(cntr c, void* obj);
typedef void* (*pf_linear_cntr_remove_front)(cntr c);
typedef void* (*pf_linear_cntr_remove_back)(cntr c);

typedef struct linear_cntr_operations_t {
	/* front callbacks must stay same with cntr_operations */
	pf_cntr_destroy             __destroy;
	pf_cntr_clear               __clear;
	pf_cntr_size                __size;
	pf_cntr_add                 __add;
	pf_cntr_remove              __remove;
	pf_cntr_find                __find;
	pf_cntr_citer_begin         __citer_begin;
	pf_cntr_citer_end           __citer_end;

	pf_linear_cntr_front        __front;
	pf_linear_cntr_back         __back;
	pf_linear_cntr_add_front    __add_front;
	pf_linear_cntr_add_back     __add_back;
	pf_linear_cntr_remove_front __remove_front;
	pf_linear_cntr_remove_back  __remove_back;
} clinear_operations;

typedef struct cntr_linear_body_t {
	clinear_operations* __vt;

	unknown             __cont;
} cntr_linear_body;

static clinear_operations clist_ops = {
	clist_destroy, /* destroy */
	clist_clear, /* clean */
	clist_size, /* size */
	clist_add, /* add */
	clist_remove,
	clist_find,
	clist_citer_begin, /* citer_begin */
	clist_citer_end  , /* citer_end   */

	clist_front, /* front */
	clist_back , /* back  */
	clist_add_front, /* add_front */
	clist_add_back , /* add_back  */
	clist_remove_front, /* remove_front */
	clist_remove_back , /* remove_back  */
};

static clinear_operations carray_ops = {
	carray_destroy, /* destroy */
	carray_clear, /* clean */
	carray_size, /* size */
	carray_add,
	carray_remove,
	carray_find,
	carray_citer_begin, /* citer_begin */
	carray_citer_end  , /* citer_end   */

	carray_front, /* front */
	carray_back , /* back  */
	carray_add_front, /* add_front */
	carray_add_back , /* add_back  */
	carray_remove_front, /* remove_front */
	carray_remove_back , /* remove_back  */
};

cntr clinear_as_list() {
	clist list = clist_create();

	cntr_linear_body* cl = (cntr_linear_body*)halloc(sizeof(cntr_linear_body));
	cl->__vt = &clist_ops;
	cl->__cont = (unknown)list;

	return (cntr)cl;
}

cntr clinear_as_array() {
	carray ary = carray_create();

	cntr_linear_body* cl = (cntr_linear_body*)halloc(sizeof(cntr_linear_body));
	cl->__vt = &carray_ops;
	cl->__cont = (unknown)ary;

	return (cntr)cl;
}

void clinear_destroy(cntr c) {
	cntr_destroy(c);
}

void clinear_clear(cntr c) {
	cntr_clear(c);
}

int clinear_size(cntr c) {
	return cntr_size(c);
}

void clinear_add(cntr c, void* obj) {
	cntr_add(c, obj);
}

void clinear_remove(cntr c, citer begin, citer end) {
	cntr_remove(c, begin, end);
}

bool clinear_find(cntr c, void* obj, citer itr) {
	return cntr_find(c, obj, itr);
}

void clinear_citer_begin(cntr c, citer itr) {
	cntr_citer_begin(c, itr);
}

void clinear_citer_end(cntr c, citer itr) {
	cntr_citer_end(c, itr);
}

void* clinear_front(cntr c) {
	cntr_linear_body* cl = (cntr_linear_body*)c;

	return (void*)(cl->__vt)->__front(cl->__cont);
}

void* clinear_back(cntr c) {
	cntr_linear_body* cl = (cntr_linear_body*)c;

	return (void*)(cl->__vt)->__back(cl->__cont);
}

void clinear_add_front(cntr c, void* obj) {
	cntr_linear_body* cl = (cntr_linear_body*)c;

	(cl->__vt)->__add_front(cl->__cont, obj);
}

void clinear_add_back(cntr c, void* obj) {
	cntr_linear_body* cl = (cntr_linear_body*)c;

	(cl->__vt)->__add_back(cl->__cont, obj);
}

void* clinear_remove_front(cntr c) {
	cntr_linear_body* cl = (cntr_linear_body*)c;

	return (void*)(cl->__vt)->__remove_front(cl->__cont);
}

void* clinear_remove_back(cntr c) {
	cntr_linear_body* cl = (cntr_linear_body*)c;

	return (void*)(cl->__vt)->__remove_back(cl->__cont);
}

