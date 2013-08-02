#ifndef _OBJECT_ALLOCATOR_H_
#define _OBJECT_ALLOCATOR_H_

extern object* default_allocator;

object* allocator_llrb_spawn(object* parent);

object* allocator_buddy_spawn(object* parent);

object* allocator_pool_spawn(object* parent);

#endif /* _OBJECT_ALLOCATOR_H_ */
