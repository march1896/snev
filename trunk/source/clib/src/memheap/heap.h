#ifndef _HEAP_H_
#define _HEAP_H_

#include <cominc.h>
#include <heap_interface.h>

void heap_destroy(heap_handle pheap);

void* heap_alloc(heap_handle pheap, int size, const char* file, size_t line);

void heap_dealloc(heap_handle pheap, void *buff, const char* file, size_t line);

void* heap_realloc(heap_handle pheap, void *buff, int size, const char* file, size_t line);

void heap_dump(heap_handle pheap);

#endif /* _HEAP_H_ */
