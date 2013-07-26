#ifndef _INTERFACE_MEMORY_MANAGEMENT_H_
#define _INTERFACE_MEMORY_MANAGEMENT_H_
/* TODO: remove this file, no longer needed */

#include <cominc.h>
#include <oo_model.h>

/* TODO: move these definitions to a proper place */
typedef void* (*pf_alloc  ) (int size);
typedef void  (*pf_dealloc) (void* buff);
typedef void  (*pf_dispose) (void* buff);
typedef void* (*pf_copy   ) (void* buff);

inline void* sys_alloc(int size) {
	return halloc(size);
}

inline void sys_dealloc(void* buff) {
	hfree(buff);
}

#endif /* _INTERFACE_MEMORY_MANAGEMENT_H_ */
