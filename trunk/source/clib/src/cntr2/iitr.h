#ifndef _INTERFACE_ITR_H_
#define _INTERFACE_ITR_H_

/* the interface functions which support, used by clients */
#include <oo_model.h>
#include <idef.h>

extern inline void itr_destroy         (iterator itr);
extern inline bool itr_equals          (const iterator ia, const iterator ib);
extern inline const void* itr_get_ref  (const iterator itr);
extern inline void itr_set_ref         (iterator itr, const void* __ref);
extern inline void itr_to_prev         (iterator itr);
extern inline void itr_to_next         (iterator itr);
extern inline void itr_advance         (iterator itr, int length);
extern inline int  itr_distance        (const iterator from, const iterator to);

/* TODO: should we implement the below functions 
 * these are the 'real' interface functions, the above is not 'interface method',
 * strictly specking, the above functions should not be in this 'interface' file.*/
extern inline void iitr_destroy        (iobject* iitr);
extern inline bool iitr_equals         (const iobject* ia, const iobject* ib);
extern inline const void* iitr_get_ref (const iobject* iitr);
extern inline void iitr_set_ref        (iobject* iitr, const void* __ref);
extern inline void iitr_to_prev        (iobject* iitr);
extern inline void iitr_to_next        (iobject* iitr);
extern inline void iitr_advance        (iobject* iitr, int length);
extern inline int  iitr_distance       (const iobject* ifrom, const iobject* ito);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef void     (*pf_itr_destroy)     (iterator citr);
typedef bool     (*pf_itr_equals)      (const iterator itr_a, const iterator itr_b);
typedef const void* (*pf_itr_get_ref)  (const iterator citr);
typedef void     (*pf_itr_set_ref)     (iterator citr, const void* object);
typedef void     (*pf_itr_to_prev)     (iterator citr);
typedef void     (*pf_itr_to_next)     (iterator citr);
typedef void*    (*pf_itr_advance)     (iterator citr, int length);
typedef int      (*pf_itr_distance)    (const iterator citr_from, const iterator citr_to);

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
