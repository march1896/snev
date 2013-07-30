#include <heap_global.h>

#include <memheap/heap_sys.h>
#include <memheap/heap_llrb.h>
#include <memheap/heap_wrap.h>
#include <memheap/heap_buddy.h>

#include <memheap/heap_factory.h>

void*              __global_heap_sysd  = NULL;
struct heap_llrb*  __global_heap_llrb  = NULL;
struct heap_buddy* __global_heap_buddy = NULL;

static int __system_global_heap_magic;

/* sys default heap does not need to init, we just give it a magic number,
 * then we spawn two heap(a llrb and a buddy) from the sys default heap */
void heap_global_init() {
	if (__global_heap_sysd == NULL) {
		__global_heap_sysd = (void*)&__system_global_heap_magic;
	}

	if (__global_heap_llrb == NULL) {
		__global_heap_llrb = heap_llrb_spawn(
				__global_heap_sysd, 
				heap_sysd_alloc,
				heap_sysd_dealloc
				);

		dbg_assert(__global_heap_llrb != NULL);
	}

	if (__global_heap_buddy == NULL) {
		__global_heap_buddy = heap_buddy_spawn(
				__global_heap_sysd, 
				heap_sysd_alloc,
				heap_sysd_dealloc
				);

		dbg_assert(__global_heap_buddy != NULL);
	}
}

void heap_global_deinit() {
	if (__global_heap_sysd != NULL) {
		__global_heap_sysd = NULL;
	}

	if (__global_heap_llrb != NULL) {
		heap_llrb_join(__global_heap_llrb);
		__global_heap_llrb = NULL;
	}

	if (__global_heap_buddy != NULL) {
		heap_buddy_join(__global_heap_buddy);
		__global_heap_buddy = NULL;
	}
}
