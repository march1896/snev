#include <cntr2/oo_bridge.h>
#include <memheap/heap_def.h>

static unknown *pointer_clone(const unknown* ref_x, pf_alloc alc, void* heap) {
	/* just return the reference that ref_x points to */
	return (unknown*)ref_x;
}
static void pointer_destroy(const unknown* ref_x, pf_dealloc dlc, void* heap) {
	/* no need to destroy */
}
static compres pointer_compare_to(const unknown* ref_x, const unknown* ref_y) {
	/* just use the address to compare */
	if (ref_x < ref_y) 
		return -1;
	else if (ref_x > ref_y) 
		return 1;
	return 0;
}
static bool pointer_equals(const unknown* ref_x, const unknown* ref_y) {
	/* just use the address to compare */
	return ref_x == ref_y;
}
static hashcode pointer_hashcode(const unknown* ref_x) {
	// TODO
	return (hashcode)ref_x;
}
unknown_traits pointer_traits = {
	pointer_destroy,
	pointer_clone,
	pointer_compare_to,
	pointer_equals,
	pointer_hashcode
};

static unknown *int_clone(const unknown* ref_x, pf_alloc alc, void* heap) {
	int* new_int = (int*)alloc(alc, heap, sizeof(int));
	*new_int = *(int*)ref_x;
	return (unknown*)new_int;
}
static void int_destroy(const unknown* ref_x, pf_dealloc dlc, void* heap) {
	dealloc(dlc, heap, (void*)ref_x);
}
static compres int_compare_to(const unknown* ref_x, const unknown* ref_y) {
	int x = *(int*)ref_x;
	int y = *(int*)ref_y;

	if (x < y) 
		return -1;
	else if (x > y) 
		return 1;
	return 0;
}
static bool int_equals(const unknown* ref_x, const unknown* ref_y) {
	int x = *(int*)ref_x;
	int y = *(int*)ref_y;
	return x == y;
}
static hashcode int_hashcode(const unknown* ref_x) {
	// TODO
	return (hashcode)(*(int*)ref_x);
}
unknown_traits int_traits = {
	int_destroy,
	int_clone,
	int_compare_to,
	int_equals,
	int_hashcode
};

static unknown *longint_clone(const unknown* ref_x, pf_alloc alc, void* heap) {
	longint* new_longint = (longint*)alloc(alc, heap, sizeof(longint));
	*new_longint = *(longint*)ref_x;
	return (unknown*)new_longint;
}
static void longint_destroy(const unknown* ref_x, pf_dealloc dlc, void* heap) {
	dealloc(dlc, heap, (void*)ref_x);
}
static compres longint_compare_to(const unknown* ref_x, const unknown* ref_y) {
	longint x = *(longint*)ref_x;
	longint y = *(longint*)ref_y;

	if (x < y) 
		return -1;
	else if (x > y) 
		return 1;
	return 0;
}
static bool longint_equals(const unknown* ref_x, const unknown* ref_y) {
	longint x = *(longint*)ref_x;
	longint y = *(longint*)ref_y;
	return x == y;
}
static hashcode longint_hashcode(const unknown* ref_x) {
	// TODO
	return (hashcode)(*(longint*)ref_x);
}
unknown_traits longint_traits = {
	longint_destroy,
	longint_clone,
	longint_compare_to,
	longint_equals,
	longint_hashcode
};

static unknown *float_clone(const unknown* ref_x, pf_alloc alc, void* heap) {
	float* new_float = (float*)alloc(alc, heap, sizeof(float));
	*new_float = *(float*)ref_x;
	return (unknown*)new_float;
}
static void float_destroy(const unknown* ref_x, pf_dealloc dlc, void* heap) {
	dealloc(dlc, heap, (void*)ref_x);
}
static compres float_compare_to(const unknown* ref_x, const unknown* ref_y) {
	float x = *(float*)ref_x;
	float y = *(float*)ref_y;

	if (x < y) 
		return -1;
	else if (x > y) 
		return 1;
	return 0;
}
static bool float_equals(const unknown* ref_x, const unknown* ref_y) {
	float x = *(float*)ref_x;
	float y = *(float*)ref_y;
	return x == y;
}
static hashcode float_hashcode(const unknown* ref_x) {
	// TODO
	return (hashcode)(*(float*)ref_x);
}
unknown_traits float_traits = {
	float_destroy,
	float_clone,
	float_compare_to,
	float_equals,
	float_hashcode
};

static unknown *double_clone(const unknown* ref_x, pf_alloc alc, void* heap) {
	double* new_double = (double*)alloc(alc, heap, sizeof(double));
	*new_double = *(double*)ref_x;
	return (unknown*)new_double;
}
static void double_destroy(const unknown* ref_x, pf_dealloc dlc, void* heap) {
	dealloc(dlc, heap, (void*)ref_x);
}
static compres double_compare_to(const unknown* ref_x, const unknown* ref_y) {
	double x = *(double*)ref_x;
	double y = *(double*)ref_y;

	if (x < y) 
		return -1;
	else if (x > y) 
		return 1;
	return 0;
}
static bool double_equals(const unknown* ref_x, const unknown* ref_y) {
	double x = *(double*)ref_x;
	double y = *(double*)ref_y;
	return x == y;
}
static hashcode double_hashcode(const unknown* ref_x) {
	// TODO
	return (hashcode)(*(double*)ref_x);
}
unknown_traits double_traits = {
	double_destroy,
	double_clone,
	double_compare_to,
	double_equals,
	double_hashcode
};

static unknown *char_clone(const unknown* ref_x, pf_alloc alc, void* heap) {
	char* new_char = (char*)alloc(alc, heap, sizeof(char));
	*new_char = *(char*)ref_x;
	return (unknown*)new_char;
}
static void char_destroy(const unknown* ref_x, pf_dealloc dlc, void* heap) {
	dealloc(dlc, heap, (void*)ref_x);
}
static compres char_compare_to(const unknown* ref_x, const unknown* ref_y) {
	char x = *(char*)ref_x;
	char y = *(char*)ref_y;

	if (x < y) 
		return -1;
	else if (x > y) 
		return 1;
	return 0;
}
static bool char_equals(const unknown* ref_x, const unknown* ref_y) {
	char x = *(char*)ref_x;
	char y = *(char*)ref_y;
	return x == y;
}
static hashcode char_hashcode(const unknown* ref_x) {
	// TODO
	return (hashcode)(*(char*)ref_x);
}
unknown_traits char_traits = {
	char_destroy,
	char_clone,
	char_compare_to,
	char_equals,
	char_hashcode
};

#include <string.h>
static unknown *string_clone(const unknown* ref_x, pf_alloc alc, void* heap) {
	char* old_str = (char*)ref_x;
	int str_len = (int)strlen(old_str);
	char* new_str = (char*)alloc(alc, heap, sizeof(char) * (str_len + 1));
	strcpy(new_str, old_str);
	return (unknown*)new_str;
}
static void string_destroy(const unknown* ref_x, pf_dealloc dlc, void* heap) {
	dealloc(dlc, heap, (void*)ref_x);
}
static compres string_compare_to(const unknown* ref_x, const unknown* ref_y) {
	char* x = (char*)ref_x;
	char* y = (char*)ref_y;

	int res = strcmp(x, y);

	return res;
}
static bool string_equals(const unknown* ref_x, const unknown* ref_y) {
	char* x = (char*)ref_x;
	char* y = (char*)ref_y;

	int res = strcmp(x, y);

	return res == 0;
}
static hashcode string_hashcode(const unknown* ref_x) {
	// TODO
	return (hashcode)((char*)ref_x);
}
unknown_traits string_traits = {
	string_destroy,
	string_clone,
	string_compare_to,
	string_equals,
	string_hashcode
};

