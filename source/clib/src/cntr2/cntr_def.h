#ifndef _CNTR_DEF_H_
#define _CNTR_DEF_H_
/* TODO: remove this file, no longer needed */

#include <cominc.h>
#include <oo_model.h>

typedef unknown citer;
typedef unknown cntr;

#define __default_alloc   halloc
#define __default_dealloc hfree

/* TODO: move these definitions to a proper place */
typedef void* (*pf_alloc  ) (int size);
typedef void  (*pf_dealloc) (void* buff);
typedef void  (*pf_dispose) (void* buff);

#endif /* _CNTR_DEF_H_ */
