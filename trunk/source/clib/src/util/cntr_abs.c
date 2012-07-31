#include <cntr_abs.h>

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
	void* container;

	clinear_operations* ops;
} clinear;

