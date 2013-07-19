#ifndef _HEAP_DEF_H_
#define _HEAP_DEF_H_

#define _MEM_DEBUG_

typedef void* heap_handle;

typedef void* (*pf_mem_increase)(int size);

typedef void (*pf_mem_decrease)(void* buff);

#define _USING_GLOBAL_LLRB_HEAP_
//#define _USING_GLOBAL_BUDDY_HEAP_

#endif /* _HEAP_DEF_H_ */
