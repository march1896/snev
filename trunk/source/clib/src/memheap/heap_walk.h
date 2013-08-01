#ifndef _HEAP_WALK_H_
#define _HEAP_WALK_H_

/* 
 * this module defines kinds of traverse method used for a given heap which has 
 * defined * pf_heap_walk() *
 * all method in this module should obey the prototype of 
 * typedef void (*pf_process_block)(struct heap_blockinfo* block_info, void* param);
 */

/* 
 * heap_leak_print_to_terminal is designed for direct use, for example.
 * heap_llrb_walk(pheap, heap_leak_print_to_terminal, NULL);
 */
void heap_leak_print_to_terminal(struct heap_blockinfo* binfo, void* param);

struct heap_state {
	/* total_allocable, total_inuse means memory size, not the block size */
	int     total_allocable;
	int     total_inuse;
	/* max allocable is does not mean we can not acquire a bigger block from this heap,
	 * just means if we try to acquire a larger block, the heap must acquire memory from 
	 * its parent */
	int     max_allocable;
};

void heap_state_init(struct heap_state* stat);
/*
 * to retrieve heap_state, we should create a heap_state and pass to heap_walk, i.e.
 * struct heap_state cur_state;
 * heap_state_init(&cur_state);
 * heap_llrb_walk(pheap, heap_query_state, (void*)&cur_state);
 */
void heap_query_state(struct heap_blockinfo* binfo, void* param);

#endif /* _HEAP_WALK_H_ */
