#include "cls_builtintype.h"

#include "ptc_basic.h"

static unknown* int_clone(const unknown* me, heap_info *heap) {
	int *new_int = (int*)heap_alloc(heap, sizeof(int));
	*new_int = *(int*)me;

	return new_int;
}
static bool int_equals(const unknown* me, const unknown* other) {
	return *(int*)me == *(int*)other;
}
static int int_compare(const unknown* me, const unknown* other) {
	int x = *(int*)me, y = *(int*)other;

	if (x < y)
		return -1;
	else if (x > y)
		return 1;

	return 0;
}
static hashcode int_hashcode(const unknown* me) {
	int x = *(int*)me;
	// TODO
	return x;
}

static struct basic_protocol cls_int_basic_ptc = {
	(ptc_basic_clone)int_clone,
	(ptc_basic_equals)int_equals,
	(ptc_basic_compare)int_compare,
	(ptc_basic_hashcode)int_hashcode
};

static protocol* cls_int_get_protocol(unique_id ptc_id) {
	if (ptc_id == basic_ptc_id)
		return &cls_int_basic_ptc;

	return NULL;
}

#include <string.h>
// TODO: this progress is fixed, could use a map and a function to do.
static unknown *cls_int_get_reponder(unique_id ptc_id, const char *name) {
	if (ptc_id == basic_ptc_id) {
		if (strcmp(name, "iobject_clone")) 
			return int_clone;
		else if (strcmp(name, "iobject_equals"))
			return int_equals;
		else if (strcmp(name, "iobject_compare"))
			return int_compare;
		else if (strcmp(name, "iobject_hashcode"))
			return int_hashcode;
	}

	return NULL;
}

Class Class_Int = {
	cls_int_get_protocol,
	cls_int_get_reponder,
	"int",
	"class wrapper for builtin type int"
};

Object *object_create_as_int(int init_value, heap_info *heap) {
	Object *obj = object_create(heap);
	int *new_int = (int*)heap_alloc(heap, sizeof(int));
	*new_int = init_value;

	object_init(obj, &Class_Int, new_int, heap, true);

	return obj;
}
