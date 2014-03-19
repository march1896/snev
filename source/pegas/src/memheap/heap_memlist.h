#ifndef _HEAP_MEMORY_LIST_H_
#define _HEAP_MEMORY_LIST_H_

static void mem_list_node_dispose(struct list_link* link, void* param) {
	struct mem_list_node* mem_node = 
		container_of(link, struct mem_list_node, link);
	struct heap_llrb* pheap = (struct heap_llrb*)param;

	/* first delete the memory hold by this node */
	dealloc(pheap->__dealloc, pheap->__parent, mem_node->memory);

	/* second delete the memory node itself */
	dealloc(pheap->__dealloc, pheap->__parent, mem_node);
}
#endif /* _HEAP_MEMORY_LIST_H_ */
