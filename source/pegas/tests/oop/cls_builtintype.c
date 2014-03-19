#include <oop/cls_builtintype.h>
#include <oop/ptc_basic.h>

#include <memheap/heap_global.h>

void cls_builtintype_test() {
	Object *i = object_create_as_int(1, &g_heap);
	Object *j = object_create_as_int(1, &g_heap);
	Object *k = object_create_as_int(2, &g_heap);

	Object *x = NULL;

	dbg_assert(pobject_compare(i, j) == 0);
	dbg_assert(pobject_equals(i, j) == true);
	dbg_assert(pobject_compare(i, k) < 0);

	x = pobject_clone(k, &g_heap);
	dbg_assert(pobject_compare(x, k) == 0);

	object_release(i);
	object_release(j);
	object_release(k);
	object_release(x);

	return;
}
