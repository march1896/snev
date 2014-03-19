#ifndef _OO_MODEL_2_H_
#define _OO_MODEL_2_H_

#include "../util/cominc.h"
#include "../memheap/heap_def.h"
/*
 * For more detail about the oo model, please see oo_model.doc.h
 */
typedef void         unknown;
typedef unsigned int unique_id;
typedef unsigned int hashcode;
typedef int          compres;

/*
 * link time unique id.
 * we could not use this id at compile time, for example in switch case statement 
 */
#define DECLARE_UID(id_name) extern unique_id id_name
#define DEFINE_UID(id_name) unique_id id_name = (unique_id)&id_name;

typedef struct object_t Object;
typedef unknown protocol;
typedef protocol* (*pf_get_protocol)(unique_id ptc_id);
typedef unknown*  (*pf_get_responder)(unique_id ptc_id, const char *msg_name);

typedef struct class_t {
	/* we dont support inherit */
	/* Class*          __parent; */
	pf_get_protocol  get_protocol;

	pf_get_responder get_responder;

	const char       *name;
	const char       *description;
} Class;

typedef unsigned int object_desc;
struct object_t {
	const Class *is_a;

	object_desc flags;

	int         ref_count;
	heap_info   *object_heap;

	unknown     *content;
	heap_info   *content_heap;
};

Object        *object_create(heap_info *heap);
void           object_init(Object *x, const Class *cls, unknown *__ref, heap_info *__ref_heap, bool owns);
const unknown *object_getref(const Object *x);

void           object_release(Object *x);
void           object_attain(Object *x);
int            object_refcount(const Object *x);

//bool __is_class_of(const Object *x, unique_id cls_id);

// get the responder function via the message name 
unknown *__get_responder(const Object *x, unique_id ptc_id, const char *ptc_msg_name);
// get the protocol struct via protocol id 
protocol *__get_protocol(const Object *x, unique_id ptc_id);

/*
#define declare_dispatch_function_0(ptc_name, msg_name, ret_type) \
	ret_type i##ptc_name##_##msg_name(Object *x);
#define declare_dispatch_function_1(ptc_name, msg_name, ret_type, p1_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type param1);
#define declare_dispatch_function_2(ptc_name, msg_name, ret_type, p1_type, p2_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2);
#define declare_dispatch_function_3(ptc_name, msg_name, ret_type, p1_type, p2_type, p3_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2, p3_type p3);
#define declare_dispatch_function_4(ptc_name, msg_name, ret_type, p1_type, p2_type, p3_type, p4_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2, p3_type p3, p4_type p4);
#define declare_dispatch_function_5(ptc_name, msg_name, ret_type, p1_type, p2_type, p3_type, p4_type, p5_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2, p3_type p3, p4_type p4, p5_type p5);
#define declare_dispatch_function_0(ptc_name, msg_name, ret_type) \
	ret_type i##ptc_name##_##msg_name(const Object *x);
#define declare_dispatch_function_const1(ptc_name, msg_name, ret_type, p1_type) \
	ret_type i##ptc_name##_##msg_name(const Object *x, p1_type param1);
#define declare_dispatch_function_const2(ptc_name, msg_name, ret_type, p1_type, p2_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2);
#define declare_dispatch_function_const3(ptc_name, msg_name, ret_type, p1_type, p2_type, p3_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2, p3_type p3);
#define declare_dispatch_function_const4(ptc_name, msg_name, ret_type, p1_type, p2_type, p3_type, p4_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2, p3_type p3, p4_type p4);
#define declare_dispatch_function_const5(ptc_name, msg_name, ret_type, p1_type, p2_type, p3_type, p4_type, p5_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2, p3_type p3, p4_type p4, p5_type p5);

#define define_dispatch_function_0(ptc_name, msg_name, ret_type) \
	ret_type i##ptc_name##_##msg_name(Object *x) { \
		ptc_name##_protocol *ptc = x->__is_a->__get_protocol(x, ptc_name##_ptc_id); \
		return ptc->##msg_name(x->__content); \
	}
#define define_dispatch_function_1(ptc_name, msg_name, ret_type, p1_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type param1) { \
		ptc_name##_protocol *ptc = x->__is_a->__get_protocol(x, ptc_name##_ptc_id); \
		return ptc->##msg_name(x->__content, param1); \
	}
#define define_dispatch_function_2(ptc_name, msg_name, ret_type, p1_type, p2_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2);
#define define_dispatch_function_3(ptc_name, msg_name, ret_type, p1_type, p2_type, p3_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2, p3_type p3);
#define define_dispatch_function_4(ptc_name, msg_name, ret_type, p1_type, p2_type, p3_type, p4_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2, p3_type p3, p4_type p4);
#define define_dispatch_function_5(ptc_name, msg_name, ret_type, p1_type, p2_type, p3_type, p4_type, p5_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2, p3_type p3, p4_type p4, p5_type p5);
#define define_dispatch_function_const0(ptc_name, msg_name, ret_type) \
	ret_type i##ptc_name##_##msg_name(Object *x) { \
		ptc_name##_protocol *ptc = x->__is_a->__get_protocol(x, ptc_name##_ptc_id); \
		return ptc->##msg_name(x->__content); \
	}
#define define_dispatch_function_const1(ptc_name, msg_name, ret_type, p1_type) \
	ret_type i##ptc_name##_##msg_name(const Object *x) { \
		ptc_name##_protocol *ptc = x->__is_a->__get_protocol(x, ptc_name##_ptc_id); \
		return ptc->##msg_name(x->__content); \
	}
#define define_dispatch_function_const2(ptc_name, msg_name, ret_type, p1_type, p2_type) \
	ret_type i##ptc_name##_##msg_name(const Object *x, p1_type param1) { \
		ptc_name##_protocol *ptc = x->__is_a->__get_protocol(x, ptc_name##_ptc_id); \
		return ptc->##msg_name(x->__content, param1); \
	}
#define define_dispatch_function_const3(ptc_name, msg_name, ret_type, p1_type, p2_type, p3_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2, p3_type p3);
#define define_dispatch_function_const4(ptc_name, msg_name, ret_type, p1_type, p2_type, p3_type, p4_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2, p3_type p3, p4_type p4);
#define define_dispatch_function_const5(ptc_name, msg_name, ret_type, p1_type, p2_type, p3_type, p4_type, p5_type) \
	ret_type i##ptc_name##_##msg_name(Object *x, p1_type p1, p2_type p2, p3_type p3, p4_type p4, p5_type p5);
	*/

#endif /* _OO_MODEL_2_H_ */
