#ifndef _CNTR_DEF_H_
#define _CNTR_DEF_H_
/* TODO: remove this file, no longer needed */

#include <cominc.h>
#include <oo_model.h>

#define __default_alloc   halloc
#define __default_dealloc hfree

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

#endif /* _CNTR_DEF_H_ */