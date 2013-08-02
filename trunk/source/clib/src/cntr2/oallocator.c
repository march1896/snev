#include <iallocator.h>

#include <memheap/heap_def.h>
#include <memheap/heap_llrb.h>
#include <memheap/heap_sys.h>
#include <memheap/heap_factory.h>
#include <memheap/heap_global.h>

#include <imemmgr.h>
#include <util/list_link.h>

enum heap_interfaces {
	e_heap,
	e_heap_count
};

/* by using the below adapter, we could mock the 'allocater heap' as a heap to 
 * pass it to pf_alloc/pf_dealloc interface. */

#ifdef _VERBOSE_ALLOC_DEALLOC_

static void* allocator_alloc_adapter_v(void* pheap, int size, const char* file, int line) {
	iobject* allocator = (iobject*)pheap;

	return iallocator_alloc_v(allocator, size, file, line);
}

static bool allocator_dealloc_adapter_v(void* pheap, void* buff, const char* file, int line) {
	iobject* allocator = (iobject*)pheap;

	return iallocator_dealloc_v(allocator, buff, file, line);
}

#define allocator_alloc_adapter allocator_alloc_adapter_v
#define allocator_dealloc_adapter allocator_dealloc_adapter_v

#else 

static void* allocator_alloc_adapter_c(void* pheap, int size) {
	iobject* allocator = (iobject*)pheap;

	return iallocator_alloc_c(allocator, size);
}

static bool allocator_dealloc_adapter_c(void* pheap, void* size) {
	iobject* allocator = (iobject*)pheap;

	iallocator_dealloc_c(allocator, buff);
}

#define allocator_alloc_adapter allocator_alloc_adapter_c
#define allocator_dealloc_adapter allocator_dealloc_adapter_c

#endif


struct allocator_llrb {
	address                       __offset;
	pf_cast                       __cast;
	
	struct base_interface         __iftable[e_heap_count];

	object*                       __parent;
	struct heap_llrb*             __driver;
};

static void allocator_llrb_join(object* o) {
	struct allocator_llrb* me = (struct allocator_llrb*)o;
	iobject* ime = __cast(me, IALLOCATOR_ID);
	object* parent = iallocator_get_parent(ime);
	iobject* iparent = __cast(parent, IALLOCATOR_ID);

	/* first join the driver */
	heap_llrb_join(me->__driver);

	/* release the memory of this object */
	iallocator_dealloc(iparent, me);
}

#ifdef _VERBOSE_ALLOC_DEALLOC_
static void* allocator_llrb_alloc_v(object* pheap, int size, const char* file, int line) {
	struct allocator_llrb* me = (struct allocator_llrb*)pheap;

	return heap_llrb_alloc_v(me->__driver, size, file, line);
}
static bool allocator_llrb_dealloc_v(object* pheap, void* buff, const char* file, int line) {
	struct allocator_llrb* me = (struct allocator_llrb*)pheap;

	return heap_llrb_dealloc_v(me->__driver, buff, file, line);
}
#define allocator_llrb_alloc allocator_llrb_alloc_v
#define allocator_llrb_dealloc allocator_llrb_dealloc_v
#else 
static void* allocator_llrb_alloc_c(object* pheap, int size) {
	struct allocator_llrb* me = (struct allocator_llrb*)pheap;

	return heap_llrb_alloc_c(me->__driver, size);
}
static bool allocator_llrb_dealloc_c(object* pheap, void* buff) {
	struct allocator_llrb* me = (struct allocator_llrb*)pheap;

	return heap_llrb_dealloc_c(me->__driver, buff);
}
#define allocator_llrb_alloc allocator_llrb_alloc_c
#define allocator_llrb_dealloc allocator_llrb_dealloc_c
#endif
static object* allocator_llrb_get_parent(object* pheap) {
	struct allocator_llrb* me = (struct allocator_llrb*)pheap;
	return me->__parent;
}
//static void allocator_llrb_walk(object* pheap, pf_process_block per_block_cb, void* param);

struct iallocator_vtable __allocator_llrb_vtable = {
	allocator_llrb_join,
	allocator_llrb_alloc,
	allocator_llrb_dealloc,
	allocator_llrb_get_parent
};

static unknown allocator_llrb_cast(unknown x, unique_id intf_id);
static unknown allocator_llrb_cast(unknown x, unique_id intf_id) {
	struct allocator_llrb* o = (struct allocator_llrb*)x;

	dbg_assert(__is_object(x));

	switch (intf_id) {
	case IALLOCATOR_ID:
		return (unknown)&o->__iftable[e_heap];
	default:
		return NULL;
	}

	return NULL;
}

object* allocator_llrb_spawn(object* oparent) {
	iobject* iparent = __cast(oparent, IALLOCATOR_ID);
	struct allocator_llrb* allocator = NULL;

	dbg_assert(iparent != NULL);
	allocator = (struct allocator_llrb*)iallocator_alloc(iparent, sizeof(struct allocator_llrb));

	allocator->__offset = allocator;
	allocator->__cast   = allocator_llrb_cast;

	allocator->__iftable[e_heap].__offset = (address)e_heap;
	allocator->__iftable[e_heap].__vtable = &__allocator_llrb_vtable;


	allocator->__parent = oparent;

	/* here is a little trick, it's interesting */
	allocator->__driver = 
		heap_llrb_spawn(iparent, allocator_alloc_adapter, allocator_dealloc_adapter);

	return (object*)allocator;
}


/*****************************************************************************************
 * allocator llrb ends 
 *
 * allocator system default begin 
 *****************************************************************************************/

struct allocator_sysd {
	address                       __offset;
	pf_cast                       __cast;
	
	struct base_interface         __iftable[e_heap_count];

	object*                       __parent;
	void*                         __driver;
};

/* we can not spawn/join system default heap as well as the system default allocator,
 * so we don't have these methods */
#include <stdlib.h>
#ifdef _VERBOSE_ALLOC_DEALLOC_
static void* allocator_sysd_alloc_v(object* pheap, int size, const char* file, int line) {
	return malloc(size);
}
static bool allocator_sysd_dealloc_v(object* pheap, void* buff, const char* file, int line) {
	free(buff);
	return true;
}
#define allocator_sysd_alloc allocator_sysd_alloc_v
#define allocator_sysd_dealloc allocator_sysd_dealloc_v
#else 
static void* allocator_sysd_alloc_c(object* pheap, int size) {
	return malloc(size);
}
static bool allocator_sysd_dealloc_c(object* pheap, void* buff) {
	free(buff);
	return true;
}
#define allocator_sysd_alloc allocator_sysd_alloc_c
#define allocator_sysd_dealloc allocator_sysd_dealloc_c
#endif
static object* allocator_sysd_get_parent(object* pheap) {
	struct allocator_sysd* me = (struct allocator_sysd*)pheap;
	return me->__parent;
}
//static void allocator_sysd_walk(object* pheap, pf_process_block per_block_cb, void* param);

struct iallocator_vtable __allocator_sysd_vtable = {
	NULL,  /* pf_iallocator_destroy */
	allocator_sysd_alloc,
	allocator_sysd_dealloc,
	allocator_sysd_get_parent
};

static unknown allocator_sysd_cast(unknown x, unique_id intf_id);
static unknown allocator_sysd_cast(unknown x, unique_id intf_id) {
	struct allocator_sysd* o = (struct allocator_sysd*)x;

	dbg_assert(__is_object(x));

	switch (intf_id) {
	case IALLOCATOR_ID:
		return (unknown)&o->__iftable[e_heap];
	default:
		return NULL;
	}

	return NULL;
}

static struct allocator_sysd __allocator_sysd = {
	(address)&__allocator_sysd,  /* __offset */
	allocator_sysd_cast,         /* __cast   */
	{
		(address)e_heap,
		&__allocator_sysd_vtable
	},                           /* __iftable[e_heap_count]; */
	NULL,                        /* __parent */
	NULL                         /* __driver, should we use __global_heap_sysd instead? */
};

object* default_allocator = (object*)&__allocator_sysd;
