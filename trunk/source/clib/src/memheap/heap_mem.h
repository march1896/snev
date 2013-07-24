#ifndef _HEAP_MEMORY_MGR_H_
#define _HEAP_MEMORY_MGR_H_

#include <heap_def.h>

/* this file exist because it could be shared by different heaps */

struct list_link;
struct heap_mem {
	struct list_link    link;

	pf_mem_increase     cb_mem_inc;
	pf_mem_decrease     cb_mem_dec;
};

void heap_mem_init(pf_mem_increase cb_inc, pf_mem_decrease cb_dec);

void heap_mem_inc();

void heap_mem_dec();

#endif /* _HEAP_MEMORY_MGR_H_ */
