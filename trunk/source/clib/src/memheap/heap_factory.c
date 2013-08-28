#include <heap_factory.h>

#include <memheap/heap_sys.h>
#include <memheap/heap_wrap.h>
#include <memheap/heap_llrb.h>
#include <memheap/heap_buddy.h>
#include <memheap/heap_pool.h>

struct heap_wrap* heap_wrap_spawn(void* __parent, pf_alloc __alloc, pf_dealloc __dealloc) {
	struct heap_wrap* wheap = (struct heap_wrap*)
		alloc(__alloc, __parent, sizeof(struct heap_wrap));

	heap_wrap_init(wheap, __parent, __alloc, __dealloc);

	return wheap;
}

void heap_wrap_join(struct heap_wrap* pheap) {
	void* __parent       = pheap->__parent;
	pf_dealloc __dealloc = pheap->__dealloc;

	heap_wrap_deinit(pheap);

	dealloc(__dealloc, __parent, pheap);
}

struct heap_llrb* heap_llrb_spawn(void* __parent, pf_alloc __alloc, pf_dealloc __dealloc) {
	struct heap_llrb* wheap = (struct heap_llrb*)
		alloc(__alloc, __parent, sizeof(struct heap_llrb));

	heap_llrb_init(wheap, __parent, __alloc, __dealloc);

	return wheap;
}

struct heap_llrb* heap_llrb_spawn_v(void* __parent, pf_alloc __alloc, pf_dealloc __dealloc, int split_threshold, int expand_size) {
	struct heap_llrb* wheap = (struct heap_llrb*)
		alloc(__alloc, __parent, sizeof(struct heap_llrb));

	heap_llrb_init_v(wheap, __parent, __alloc, __dealloc, split_threshold, expand_size);

	return wheap;
}

void heap_llrb_join(struct heap_llrb* pheap) {
	void* __parent       = pheap->__parent;
	pf_dealloc __dealloc = pheap->__dealloc;

	heap_llrb_deinit(pheap);

	dealloc(__dealloc, __parent, pheap);
}

struct heap_buddy* heap_buddy_spawn(void* __parent, pf_alloc __alloc, pf_dealloc __dealloc) {
	struct heap_buddy* wheap = (struct heap_buddy*)
		alloc(__alloc, __parent, sizeof(struct heap_buddy));

	heap_buddy_init(wheap, __parent, __alloc, __dealloc);

	return wheap;
}

struct heap_buddy* heap_buddy_spawn_v(void* __parent, pf_alloc __alloc, pf_dealloc __dealloc, int split_threshold, int expand_size) {
	struct heap_buddy* wheap = (struct heap_buddy*)
		alloc(__alloc, __parent, sizeof(struct heap_buddy));

	heap_buddy_init_v(wheap, __parent, __alloc, __dealloc, split_threshold, expand_size);

	return wheap;
}

void heap_buddy_join(struct heap_buddy* pheap) {
	void* __parent       = pheap->__parent;
	pf_dealloc __dealloc = pheap->__dealloc;

	heap_buddy_deinit(pheap);

	dealloc(__dealloc, __parent, pheap);
}

struct heap_spool* heap_spool_spawn(void* __parent, pf_alloc __alloc, pf_dealloc __dealloc) {
	struct heap_spool* sheap = (struct heap_spool*)
		alloc(__alloc, __parent, sizeof(struct heap_spool));

	heap_spool_init(sheap, __parent, __alloc, __dealloc);

	return sheap;
}

void heap_spool_join(struct heap_spool* pheap) {
	void* __parent       = pheap->__parent;
	pf_dealloc __dealloc = pheap->__dealloc;

	heap_spool_deinit(pheap);

	dealloc(__dealloc, __parent, pheap);
}

struct heap_mpool* heap_mpool_spawn(void* __parent, pf_alloc __alloc, pf_dealloc __dealloc, int max_diff_type) {
	struct heap_mpool* mheap = (struct heap_mpool*)
		alloc(__alloc, __parent, sizeof(struct heap_mpool));

	heap_mpool_init(mheap, __parent, __alloc, __dealloc, max_diff_type);

	return mheap;
}

void heap_mpool_join(struct heap_mpool* pheap) {
	void* __parent       = pheap->__parent;
	pf_dealloc __dealloc = pheap->__dealloc;

	heap_mpool_deinit(pheap);

	dealloc(__dealloc, __parent, pheap);
}
