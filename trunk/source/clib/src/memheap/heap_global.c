#include <heap_global.h>

#include <memheap/heap_sys.h>
#include <memheap/heap_llrb.h>
#include <memheap/heap_wrap.h>
#include <memheap/heap_buddy.h>

#include <memheap/heap_factory.h>

void*              __global_heap_sysd  = NULL;

static int __system_global_heap_magic;

struct heap_llrb __global_static_heap_llrb = {
	NULL,                /* parent */
	heap_sysd_alloc,     /* alloc  */
	heap_sysd_dealloc,   /* dealloc */
	NULL,                /* llrb_root */
	{
		&(__global_static_heap_llrb.memlist),
		&(__global_static_heap_llrb.memlist)
	},                   /* mem_list */
	sizeof(struct heap_llrb_block),  /* split_threshold */
	HEAP_MINIMUM_EXPAND_SIZE         /* expand_size */
};

struct heap_llrb*  __global_heap_llrb  = &__global_static_heap_llrb;

static struct heap_buddy __global_static_heap_buddy = {
	NULL,                /* parent */
	heap_sysd_alloc,     /* alloc  */
	heap_sysd_dealloc,   /* dealloc */
	{
		NULL,            /* buddy[BUDDY_COUNT] */
	},
	{
		&(__global_static_heap_buddy.memlist),
		&(__global_static_heap_buddy.memlist)
	},                   /* mem_list */
	sizeof(struct heap_buddy_block),  /* split_threshold */
	HEAP_MINIMUM_EXPAND_SIZE         /* expand_size */
};

struct heap_buddy* __global_heap_buddy = &__global_static_heap_buddy;

/* sys default heap does not need to init, we just give it a magic number,
 * then we spawn two heap(a llrb and a buddy) from the sys default heap */
static void heap_global_init() {
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

static void heap_global_deinit() {
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
