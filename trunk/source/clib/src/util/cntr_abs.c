#include <cntr_abs.h>

#include <cntr_list.h>
#include <cntr_array.h>

#include <cntr_spec.h>
typedef struct linear_cntr_operations_t {
	pf_linear_cntr_init         init;
	pf_linear_cntr_deinit       deinit;
	pf_linear_cntr_clean        clean;
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
	void* pcont;

	clinear_operations* ops;
} clinear;

/*
 * To make cntr specific method adaptive to container spec interface.
 *
 * the below macro are used to disable 
 * warning: initialization from incompatible pointer type [enabled by default]
#define make_callback(cntr_type, return_type, func) \
	static inline return_type cb_##func(container cntr) {\
		return func((cntr_type*)cntr);\
	}

#define make_callback_param(cntr_type, return_type, func, param_type) \
	static inline return_type cb_##func(container cntr, param_type param) {\
		return func((cntr_type*)cntr, param);\
	}

make_callback(clist, void, clist_init);
make_callback(clist, void, clist_deinit);
make_callback(clist, void, clist_clean);
make_callback(clist, int, clist_size);
make_callback(clist, void*, clist_front);
make_callback(clist, void*, clist_back );
make_callback_param(clist, void, clist_add_front, void*);
make_callback_param(clist, void, clist_add_back, void*);
make_callback(clist, void*, clist_remove_front);
make_callback(clist, void*, clist_remove_back );
make_callback_param(clist, void, clist_citer_begin, citer*);
make_callback_param(clist, void, clist_citer_end  , citer*);
 */

static clinear_operations clist_ops = {
	clist_init, /* init */
	clist_deinit, /* deinit */
	clist_clean, /* clean */
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

container clinear_as_list() {
	clinear* c = (clinear*)halloc(sizeof(clinear));

	c->pcont = (void*)halloc(sizeof(clist));
	c->ops = &clist_ops;

	(c->ops)->init((clist*)c->pcont);

	return (container)c;
}

container clinear_as_array() {

}

void clinear_deinit(container cntr) {
	clinear* c = (clinear*)cntr;

	hfree(c->pcont);
	hfree(c);
}

int clinear_size(container cntr) {
	clinear* c = (clinear*)cntr;

	return (c->ops)->size(c->pcont);
}

void clinear_clean(container cntr) {
	clinear* c = (clinear*)cntr;

	(c->ops)->clean(c->pcont);
}

void* clinear_front(container cntr) {
	clinear* c = (clinear*)cntr;

	return (void*)(c->ops)->front(c->pcont);
}

void* clinear_back(container cntr) {
	clinear* c = (clinear*)cntr;

	return (void*)(c->ops)->back(c->pcont);
}

void clinear_add_front(container cntr, void* obj) {
	clinear* c = (clinear*)cntr;

	(c->ops)->add_front(c->pcont, obj);
}

void clinear_add_back(container cntr, void* obj) {
	clinear* c = (clinear*)cntr;

	(c->ops)->add_back(c->pcont, obj);
}

void* clinear_remove_front(container cntr) {
	clinear* c = (clinear*)cntr;

	return (void*)(c->ops)->remove_front(c->pcont);
}

void* clinear_remove_back(container cntr) {
	clinear* c = (clinear*)cntr;

	return (void*)(c->ops)->remove_front(c->pcont);
}

void clinear_citer_begin(container cntr, citer* itr) {
	clinear* c = (clinear*)cntr;

	(c->ops)->citer_begin(c->pcont, itr);
}

void clinear_citer_end(container cntr, citer* itr) {
	clinear* c = (clinear*)cntr;

	(c->ops)->citer_end(c->pcont, itr);
}
