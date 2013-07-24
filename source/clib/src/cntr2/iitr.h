#ifndef _INTERFACE_ITR_H_
#define _INTERFACE_ITR_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>

/* forward iterator interface */
extern inline void ifitr_destroy         (iobject* iitr);
extern inline bool ifitr_equals          (iobject* ia, iobject* ib);
extern inline void ifitr_to_next         (iobject* iitr);
extern inline void* ifitr_get_ref        (iobject* iitr);
extern inline void ifitr_set_ref         (iobject* iitr, void* ref);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef void     (*pf_ifitr_destroy)     (object* citr);
typedef bool     (*pf_ifitr_equals)      (object* itr_a, object* itr_b);
typedef void     (*pf_ifitr_to_next)     (object* citr);
typedef void*    (*pf_ifitr_get_ref)     (object* citr);
typedef void     (*pf_ifitr_set_ref)     (object* citr, void* object);

#define IFITR_ID 0X00000110
struct ifitr_vtable {
	/* public */
	pf_ifitr_destroy     __destroy;
	pf_ifitr_equals      __equals;
	pf_ifitr_to_next     __to_next;
	pf_ifitr_get_ref     __get_ref;
	pf_ifitr_set_ref     __set_ref;
};

/* bidirectional iterator interface */
extern inline void ibitr_destroy         (iobject* iitr);
extern inline bool ibitr_equals          (iobject* ia, iobject* ib);
extern inline void ibitr_to_prev         (iobject* iitr);
extern inline void ibitr_to_next         (iobject* iitr);
extern inline void* ibitr_get_ref        (iobject* iitr);
extern inline void ibitr_set_ref         (iobject* iitr, void* ref);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef void     (*pf_ibitr_destroy)     (object* citr);
typedef bool     (*pf_ibitr_equals)      (object* itr_a, object* itr_b);
typedef void     (*pf_ibitr_to_prev)     (object* citr);
typedef void     (*pf_ibitr_to_next)     (object* citr);
typedef void*    (*pf_ibitr_get_ref)     (object* citr);
typedef void     (*pf_ibitr_set_ref)     (object* citr, void* object);

#define IBITR_ID 0X00000111
struct ibitr_vtable {
	/* public */
	pf_ibitr_destroy     __destroy;
	pf_ibitr_equals      __equals;
	pf_ibitr_to_prev     __to_prev;
	pf_ibitr_to_next     __to_next;
	pf_ibitr_get_ref     __get_ref;
	pf_ibitr_set_ref     __set_ref;
};

/* random accessed iterator interface */
extern inline void iritr_destroy         (iobject* iitr);
extern inline bool iritr_equals          (iobject* ia, iobject* ib);
extern inline void iritr_to_prev         (iobject* iitr);
extern inline void iritr_to_next         (iobject* iitr);
extern inline void* iritr_get_ref        (iobject* iitr);
extern inline void iritr_set_ref         (iobject* iitr, void* ref);
extern inline void iritr_advance         (iobject* iitr, int length);
extern inline int  iritr_distance        (iobject* ifrom, iobject* ito);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef void     (*pf_iritr_destroy)     (object* citr);
typedef bool     (*pf_iritr_equals)      (object* itr_a, object* itr_b);
typedef void     (*pf_iritr_to_prev)     (object* citr);
typedef void     (*pf_iritr_to_next)     (object* citr);
typedef void*    (*pf_iritr_get_ref)     (object* citr);
typedef void     (*pf_iritr_set_ref)     (object* citr, void* object);
typedef void*    (*pf_iritr_advance)     (object* citr, int length);
typedef int      (*pf_iritr_distance)    (object* citr_from, object* citr_to);

#define IRITR_ID 0X00000112
struct iritr_vtable {
	/* public */
	pf_iritr_destroy     __destroy;
	pf_iritr_equals      __equals;
	pf_iritr_to_prev     __to_prev;
	pf_iritr_to_next     __to_next;
	pf_iritr_get_ref     __get_ref;
	pf_iritr_set_ref     __set_ref;
	pf_iritr_advance     __advance;
	pf_iritr_distance    __distance;
};

#endif /* _INTERFACE_ITR_H_ */
