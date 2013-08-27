#include <iallocator.h>
#include <oallocator.h>

#include <memheap/heap_def.h>
#include <memheap/heap_llrb.h>
#include <memheap/heap_sys.h>
#include <memheap/heap_factory.h>
#include <memheap/heap_global.h>

#include <util/list_link.h>

/*****************************************************************************************
 * allocator methods begin 
 *****************************************************************************************/

inline void  allocator_join     (allocator o) {
	iobject* intf = __fast_cast(o, 0);
	dbg_assert(intf == __cast(o, IALLOCATOR_ID));

	((struct iallocator_vtable*)intf->__vtable)->__join(o);
}
#ifdef _VERBOSE_ALLOC_DEALLOC_
inline void* allocator_acquire_v(allocator o, int size, const char* file, int line) {
	iobject* intf = __fast_cast(o, 0);
	dbg_assert(intf == __cast(o, IALLOCATOR_ID));

	return ((struct iallocator_vtable*)intf->__vtable)->__acquire(o, size, file, line);
}
inline bool  allocator_release_v(allocator o, void* buff, const char* file, int line) {
	iobject* intf = __fast_cast(o, 0);
	dbg_assert(intf == __cast(o, IALLOCATOR_ID));

	return ((struct iallocator_vtable*)intf->__vtable)->__release(o, buff, file, line);
}
#else 
inline void* allocator_acquire_c(allocator o, int size) {
	iobject* intf = __fast_cast(o, 0);
	dbg_assert(intf == __cast(o, IALLOCATOR_ID));

	return ((struct iallocator_vtable*)intf->__vtable)->__acquire(o, size);
}
inline bool  allocator_release_c(allocator o, void* buff) {
	iobject* intf = __fast_cast(o, 0);
	dbg_assert(intf == __cast(o, IALLOCATOR_ID));

	return ((struct iallocator_vtable*)intf->__vtable)->__release(o, buff);
}
#endif
inline allocator allocator_get_parent(allocator o) {
	iobject* intf = __fast_cast(o, 0);
	dbg_assert(intf == __cast(o, IALLOCATOR_ID));

	return ((struct iallocator_vtable*)intf->__vtable)->__get_parent(o);
}

inline void allocator_walk(allocator o, pf_process_block per_block_cb, void* param) {
	iobject* intf = __fast_cast(o, 0);
	dbg_assert(intf == __cast(o, IALLOCATOR_ID));

	((struct iallocator_vtable*)intf->__vtable)->__heap_walk(o, per_block_cb, param);
}

/*****************************************************************************************
 * allocator methods end
 *****************************************************************************************/

/* interface that every allocator object must implement */
enum heap_interfaces {
	e_heap,
	e_heap_count
};

/*****************************************************************************************
 * allocator system default begin 
 *****************************************************************************************/

struct allocator_sysd {
	address                       __offset;
	pf_cast                       __cast;
	
	iobject                       __iftable[e_heap_count];

	allocator                     __parent;
	void*                         __driver;
};

/* we can not spawn/join system default heap as well as the system default allocator,
 * so we don't have these methods */
#include <stdlib.h>
#ifdef _VERBOSE_ALLOC_DEALLOC_
static void* allocator_sysd_acquire_v(allocator alo, int size, const char* file, int line) {
	return malloc(size);
}
static bool allocator_sysd_release_v(allocator alo, void* buff, const char* file, int line) {
	free(buff);
	return true;
}
#define allocator_sysd_acquire allocator_sysd_acquire_v
#define allocator_sysd_release allocator_sysd_release_v
#else 
static void* allocator_sysd_acquire_c(allocator alo, int size) {
	return malloc(size);
}
static bool allocator_sysd_release_c(allocator alo, void* buff) {
	free(buff);
	return true;
}
#define allocator_sysd_acquire allocator_sysd_acquire_c
#define allocator_sysd_release allocator_sysd_release_c
#endif
static allocator allocator_sysd_get_parent(allocator alo) {
	struct allocator_sysd* me = (struct allocator_sysd*)alo;
	return me->__parent;
}

static void allocator_sysd_walk(allocator alo, pf_process_block per_block_cb, void* param) {
	struct allocator_sysd* me = (struct allocator_sysd*)alo;

	/* do nothing, the system heap does not contain walk method */
	//return heap_sysd_walk(me->__driver, per_block_cb, param);
}

struct iallocator_vtable __allocator_sysd_vtable = {
	NULL,  /* pf_iallocator_destroy */
	allocator_sysd_acquire,
	allocator_sysd_release,
	allocator_sysd_get_parent,
	allocator_sysd_walk
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

/* defines a static system default allocator, which can not be spawned or joined */
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

allocator global_sysd_allocator = (allocator)&__allocator_sysd;

/*****************************************************************************************
 * allocator system default end 
 *****************************************************************************************/


/*****************************************************************************************
 * allocator llrb begin 
 *****************************************************************************************/

struct allocator_llrb {
	address                       __offset;
	pf_cast                       __cast;
	
	iobject         __iftable[e_heap_count];

	allocator                     __parent;
	struct heap_llrb*             __driver;
};

static void allocator_llrb_join(allocator o) {
	struct allocator_llrb* me = (struct allocator_llrb*)o;
	allocator parent = allocator_get_parent(o);

	/* first join the driver */
	heap_llrb_join(me->__driver);

	/* release the memory of this object */
	allocator_dealloc(parent, me);
}

#ifdef _VERBOSE_ALLOC_DEALLOC_
static void* allocator_llrb_acquire_v(allocator alo, int size, const char* file, int line) {
	struct allocator_llrb* me = (struct allocator_llrb*)alo;

	return heap_llrb_alloc_v(me->__driver, size, file, line);
}
static bool allocator_llrb_release_v(allocator alo, void* buff, const char* file, int line) {
	struct allocator_llrb* me = (struct allocator_llrb*)alo;

	return heap_llrb_dealloc_v(me->__driver, buff, file, line);
}
#define allocator_llrb_acquire allocator_llrb_acquire_v
#define allocator_llrb_release allocator_llrb_release_v
#else 
static void* allocator_llrb_acquire_c(allocator alo, int size) {
	struct allocator_llrb* me = (struct allocator_llrb*)alo;

	return heap_llrb_alloc_c(me->__driver, size);
}
static bool allocator_llrb_release_c(allocator alo, void* buff) {
	struct allocator_llrb* me = (struct allocator_llrb*)alo;

	return heap_llrb_dealloc_c(me->__driver, buff);
}
#define allocator_llrb_acquire allocator_llrb_acquire_c
#define allocator_llrb_release allocator_llrb_release_c
#endif
static allocator allocator_llrb_get_parent(allocator alo) {
	struct allocator_llrb* me = (struct allocator_llrb*)alo;
	return me->__parent;
}

static void allocator_llrb_walk(allocator alo, pf_process_block per_block_cb, void* param) {
	struct allocator_llrb* me = (struct allocator_llrb*)alo;

	heap_llrb_walk(me->__driver, per_block_cb, param);
}

struct iallocator_vtable __allocator_llrb_vtable = {
	allocator_llrb_join,
	allocator_llrb_acquire,
	allocator_llrb_release,
	allocator_llrb_get_parent,
	allocator_llrb_walk
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

allocator allocator_llrb_spawn(allocator parent) {
	struct allocator_llrb* alo = NULL;

	dbg_assert(parent != NULL);
	alo = (struct allocator_llrb*)allocator_alloc(parent, sizeof(struct allocator_llrb));

	alo->__offset = alo;
	alo->__cast   = allocator_llrb_cast;

	alo->__iftable[e_heap].__offset = (address)e_heap;
	alo->__iftable[e_heap].__vtable = &__allocator_llrb_vtable;


	alo->__parent = parent;

	/* here is a little trick, it's interesting */
	alo->__driver = 
		heap_llrb_spawn(parent, (pf_alloc)allocator_acquire, (pf_dealloc)allocator_release);

	return (allocator)alo;
}

extern struct heap_llrb __global_static_heap_llrb;
/* defines a static system default allocator, which can not be spawned or joined */
static struct allocator_llrb __allocator_llrb = {
	(address)&__allocator_llrb,  /* __offset */
	allocator_llrb_cast,         /* __cast   */
	{
		(address)e_heap,
			&__allocator_llrb_vtable
	},                           /* __iftable[e_heap_count]; */
	(allocator)&__allocator_sysd,/* __parent */
	&__global_static_heap_llrb   /* __driver */
};

allocator global_llrb_allocator = (allocator)&__allocator_llrb;

/*****************************************************************************************
 * allocator llrb end 
 *****************************************************************************************/

/*****************************************************************************************
 * allocator buddy begin 
 *****************************************************************************************/

struct allocator_buddy {
	address                       __offset;
	pf_cast                       __cast;
	
	iobject         __iftable[e_heap_count];

	allocator                     __parent;
	struct heap_buddy*            __driver;
};

static void allocator_buddy_join(allocator o) {
	struct allocator_buddy* me = (struct allocator_buddy*)o;
	allocator parent = allocator_get_parent(o);

	/* first join the driver */
	heap_buddy_join(me->__driver);

	/* release the memory of this object */
	allocator_dealloc(parent, me);
}

#ifdef _VERBOSE_ALLOC_DEALLOC_
static void* allocator_buddy_acquire_v(allocator alo, int size, const char* file, int line) {
	struct allocator_buddy* me = (struct allocator_buddy*)alo;

	return heap_buddy_alloc_v(me->__driver, size, file, line);
}
static bool allocator_buddy_release_v(allocator alo, void* buff, const char* file, int line) {
	struct allocator_buddy* me = (struct allocator_buddy*)alo;

	return heap_buddy_dealloc_v(me->__driver, buff, file, line);
}
#define allocator_buddy_acquire allocator_buddy_acquire_v
#define allocator_buddy_release allocator_buddy_release_v
#else 
static void* allocator_buddy_acquire_c(allocator alo, int size) {
	struct allocator_buddy* me = (struct allocator_buddy*)alo;

	return heap_buddy_alloc_c(me->__driver, size);
}
static bool allocator_buddy_release_c(allocator alo, void* buff) {
	struct allocator_buddy* me = (struct allocator_buddy*)alo;

	return heap_buddy_dealloc_c(me->__driver, buff);
}
#define allocator_buddy_acquire allocator_buddy_acquire_c
#define allocator_buddy_release allocator_buddy_release_c
#endif
static allocator allocator_buddy_get_parent(allocator alo) {
	struct allocator_buddy* me = (struct allocator_buddy*)alo;
	return me->__parent;
}

static void allocator_buddy_walk(allocator alo, pf_process_block per_block_cb, void* param) {
	struct allocator_buddy* me = (struct allocator_buddy*)alo;

	heap_buddy_walk(me->__driver, per_block_cb, param);
}

struct iallocator_vtable __allocator_buddy_vtable = {
	allocator_buddy_join,
	allocator_buddy_acquire,
	allocator_buddy_release,
	allocator_buddy_get_parent,
	allocator_buddy_walk
};

static unknown allocator_buddy_cast(unknown x, unique_id intf_id);
static unknown allocator_buddy_cast(unknown x, unique_id intf_id) {
	struct allocator_buddy* o = (struct allocator_buddy*)x;

	dbg_assert(__is_object(x));

	switch (intf_id) {
	case IALLOCATOR_ID:
		return (unknown)&o->__iftable[e_heap];
	default:
		return NULL;
	}

	return NULL;
}

allocator allocator_buddy_spawn(allocator parent) {
	struct allocator_buddy* alo = NULL;

	dbg_assert(parent != NULL);
	alo = (struct allocator_buddy*)allocator_alloc(parent, sizeof(struct allocator_buddy));

	alo->__offset = alo;
	alo->__cast   = allocator_buddy_cast;

	alo->__iftable[e_heap].__offset = (address)e_heap;
	alo->__iftable[e_heap].__vtable = &__allocator_buddy_vtable;


	alo->__parent = parent;

	/* here is a little trick, it's interesting */
	alo->__driver = 
		heap_buddy_spawn(parent, (pf_alloc)allocator_acquire, (pf_dealloc)allocator_release);

	return (allocator)alo;
}

extern struct heap_buddy __global_static_heap_buddy;
/* defines a static system default allocator, which can not be spawned or joined */
static struct allocator_buddy __allocator_buddy = {
	(address)&__allocator_buddy,  /* __offset */
	allocator_buddy_cast,         /* __cast   */
	{
		(address)e_heap,
		&__allocator_buddy_vtable
	},                           /* __iftable[e_heap_count]; */
	(allocator)&__allocator_sysd,/* __parent */
	&__global_static_heap_buddy   /* __driver */
};

allocator global_buddy_allocator = (allocator)&__allocator_buddy;

/*****************************************************************************************
 * allocator buddy end 
 *****************************************************************************************/
