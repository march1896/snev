#include "oo_model2.h"

Object *object_create(heap_info *heap) {
	Object *obj = (Object*)heap_alloc(heap, sizeof(Object));

	obj->is_a = NULL;
	obj->ref_count = 1;
	obj->flags = 0; // TODO:
	obj->object_heap = heap;

	obj->content = NULL;

	return obj;
}

static void object_set_owns(Object *x, bool v) {
}

static bool object_get_owns(const Object *x) {
}

void object_init(Object *x, const Class *cls, unknown *__ref, heap_info *ref_heap, bool owns) {
	x->is_a = cls;

	x->content = __ref;
	x->content_heap = ref_heap;

	object_set_owns(x, owns);
}

const unknown *object_getref(const Object *x) {
	return x->content;
}

void object_release(Object *x) {
	x->ref_count --;

	if (x->ref_count == 0) {
		if (object_get_owns(x)) {
			heap_dealloc(x->content_heap, x->content);
		}
	}

	// debug purpose
	x->is_a = NULL;
	x->content = NULL;

	heap_dealloc(x->object_heap, x);
}

void object_attain(Object *x) {
	x->ref_count ++;
}

int object_refcount(const Object *x) {
	return x->ref_count;
}

protocol *__get_protocol(const Object *x, unique_id ptc_id) {
	return x->is_a->get_protocol(ptc_id);
}

unknown *__get_responder(const Object *x, unique_id ptc_id, const char *ptc_msg_name) {
	return x->is_a->get_responder(ptc_id, ptc_msg_name);
}
