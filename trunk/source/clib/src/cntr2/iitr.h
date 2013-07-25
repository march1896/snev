#ifndef _INTERFACE_ITR_H_
#define _INTERFACE_ITR_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

extern inline void itr_destroy         (object* itr);
extern inline bool itr_equals          (object* ia, object* ib);
extern inline void* itr_get_ref        (object* itr);
extern inline void itr_set_ref         (object* itr, void* ref);
extern inline void itr_to_prev         (object* itr);
extern inline void itr_to_next         (object* itr);
extern inline void itr_advance         (object* itr, int length);
extern inline int  itr_distance        (object* from, object* to);

extern inline void iitr_destroy         (iobject* iitr);
extern inline bool iitr_equals          (iobject* ia, iobject* ib);
extern inline void* iitr_get_ref        (iobject* iitr);
extern inline void iitr_set_ref         (iobject* iitr, void* ref);
extern inline void iitr_to_prev         (iobject* iitr);
extern inline void iitr_to_next         (iobject* iitr);
extern inline void iitr_advance         (iobject* iitr, int length);
extern inline int  iitr_distance        (iobject* ifrom, iobject* ito);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef void     (*pf_itr_destroy)     (object* citr);
typedef bool     (*pf_itr_equals)      (object* itr_a, object* itr_b);
typedef void*    (*pf_itr_get_ref)     (object* citr);
typedef void     (*pf_itr_set_ref)     (object* citr, void* object);
typedef void     (*pf_itr_to_prev)     (object* citr);
typedef void     (*pf_itr_to_next)     (object* citr);
typedef void*    (*pf_itr_advance)     (object* citr, int length);
typedef int      (*pf_itr_distance)    (object* citr_from, object* citr_to);

#define is_itrbas(itr) (__cast(itr, ITR_BAS_ID) != NULL)
#define is_itrref(itr) (__cast(itr, ITR_REF_ID) != NULL)
#define is_itracc(itr) (__cast(itr, ITR_ACC_ID) != NULL)
#define is_itrfwd(itr) (__cast(itr, ITR_FWD_ID) != NULL)
#define is_itrbid(itr) (__cast(itr, ITR_BID_ID) != NULL)
#define is_itrrac(itr) (__cast(itr, ITR_RAC_ID) != NULL)

struct itr_base_vtable {
	pf_itr_destroy     __destroy;
	pf_itr_equals      __equals;
};

/* extends itr_base_vtable */
struct itr_readable_vtable { 
	/* base interface */
	pf_itr_destroy     __destroy;
	pf_itr_equals      __equals;

	/* readable interface */
	pf_itr_get_ref     __get_ref;
};

/* extends itr_readable_vtable */
struct itr_accessible_vtable {
	/* base interface */
	pf_itr_destroy     __destroy;
	pf_itr_equals      __equals;

	/* readable interface */
	pf_itr_get_ref     __get_ref;

	/* accessible interface */
	pf_itr_set_ref     __set_ref;
};

/* extends itr_accessible_vtable */
struct itr_forward_vtable {
	/* base interface */
	pf_itr_destroy     __destroy;
	pf_itr_equals      __equals;

	/* readable interface */
	pf_itr_get_ref     __get_ref;

	/* accessible interface */
	pf_itr_set_ref     __set_ref;

	/* forward interface */
	pf_itr_to_next     __to_next;
};

/* extends itr_forward_vtable */
struct itr_bidirectional_vtable {
	/* base interface */
	pf_itr_destroy     __destroy;
	pf_itr_equals      __equals;

	/* readable interface */
	pf_itr_get_ref     __get_ref;

	/* accessible interface */
	pf_itr_set_ref     __set_ref;

	/* forward interface */
	pf_itr_to_next     __to_next;

	/* bidirectional interface */
	pf_itr_to_prev     __to_prev;
};

/* extends itr_bidirectional_vtable */
struct itr_randomaccessible_vtable {
	/* base interface */
	pf_itr_destroy     __destroy;
	pf_itr_equals      __equals;

	/* readable interface */
	pf_itr_get_ref     __get_ref;

	/* accessible interface */
	pf_itr_set_ref     __set_ref;

	/* forward interface */
	pf_itr_to_next     __to_next;

	/* bidirectional interface */
	pf_itr_to_prev     __to_prev;

	/* random accessible interface */
	pf_itr_advance     __advance;
	pf_itr_distance    __distance;
};

#endif /* _INTERFACE_ITR_H_ */
