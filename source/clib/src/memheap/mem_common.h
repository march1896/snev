#ifndef _MEMORY_COMMON_H_
#define _MEMORY_COMMON_H_

/* this module defines a list of memory used by the heap */

struct memory_node {
	struct list_link  link;
	void*             memory;
}

struct memory_node* expand(int size);

#endif /* _MEMORY_COMMON_H_ */
