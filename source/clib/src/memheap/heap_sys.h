#ifndef _HEAP_SYSTEM_H_
#define _HEAP_SYSTEM_H_

#include <heap_def.h>

/* Implement the following interface one by one */
/*
typedef void* (*pf_alloc_c)   (void* pheap, int size);
typedef void* (*pf_alloc_v)   (void* pheap, int size, const char* file, int line);

typedef bool (*pf_dealloc_c)  (void* pheap, void* buff);
typedef bool (*pf_dealloc_v)  (void* pheap, void* buff, const char* file, int line);

typedef void (*pf_mem_process)  (void* mem);
typedef void (*pf_mem_process_v)(void* mem, void* param);
typedef void (*pf_mem_walk)   (void* pheap, pf_mem_process allocated_cb, pf_mem_process freed_cb);
typedef void (*pf_mem_walk_v) (void* pheap, pf_mem_process_v allocated_cb, pf_mem_process_v freed_cb, void* param);
*/

typedef void* heap_system;

void* sheap_alloc   (heap_system h, int size);
void* sheap_alloc_v (heap_system h, int size, const char* file ,int line);

bool  sheap_dealloc (heap_system h, void* buff);
bool  sheap_dealloc (heap_system h, void* buff, const char* file, int line);

void  sheap_walk    (heap_system h, pf_mem_process allocated_cb, pf_mem_process freed_cb);
void  sheap_walk_v  (heap_system h, pf_mem_process_v allocated_cb, pf_mem_process_v freed_cb, void* param);

#endif /* _HEAP_SYSTEM_H_ */
