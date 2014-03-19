#ifndef _OBJECT_REDUCE_TO_REFERENCE_H_
#define _OBJECT_REDUCE_TO_REFERENCE_H_

/* 
 * C is not an object-oriented language, (even c++ is not, object is not the base element of the language, in fact,
 * all languages that have to abstract the hardware are not good to be fully object-oriented), we have to handle the 
 * the case when object reduce to raw pointers. For example,
 * the element stored in container are most likely to be raw pointers, but not objects.
 * This module defines some common basic operation for the raw-pointers-objects.
 * NOTE: it's a pity that so many "const" are added only for decoration.
 */

#include <memheap/heap_def.h>
#include <cntr2/oo_model.h>

typedef struct unknown_traits_t unknown_traits;
/* unknown_traits serve as the class of a non-oo-model object, since we could not get the class information
 * from the object instance, we should pass the class info explicitly */
typedef unknown* (*pf_ibridge_clone)(const unknown *x, pf_alloc alc, void* heap);
typedef void     (*pf_ibridge_destroy)(const unknown *x, pf_dealloc alc, void* heap);
typedef compres  (*pf_ibridge_compare)(const unknown *x, const unknown *y);
typedef bool     (*pf_ibridge_equals)(const unknown *x, const unknown *y);
typedef hashcode (*pf_ibridge_hashcode)(const unknown *x);

struct unknown_traits_t {
	pf_ibridge_destroy  __destroy;
	pf_ibridge_clone    __clone;
	pf_ibridge_compare  __compare_to;
	pf_ibridge_equals   __equals;
	pf_ibridge_hashcode __hashcode;
	// TODO: how to pass the information if the one type traits info depends on other object or traits.
	void*               __type_depends;
};

typedef long long longint;
extern unknown_traits pointer_traits;
extern unknown_traits int_traits;
extern unknown_traits longint_traits;
extern unknown_traits float_traits;
extern unknown_traits double_traits;
extern unknown_traits char_traits;
extern unknown_traits bool_traits;
extern unknown_traits string_traits;

typedef void (*pf_ref_visit)(const unknown *x);
typedef void (*pf_ref_visit_v)(const unknown *x, void* context);

typedef void (*pf_ref_process)(unknown *x);
typedef void (*pf_ref_process_v)(unknown *x, void* context);

#endif /* _OBJECT_REDUCE_TO_REFERENCE_H_ */
