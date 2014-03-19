#ifndef _INTERFACE_ITR_H_
#define _INTERFACE_ITR_H_

/* the interface functions which support, used by clients */
#include <cntr2/oo_model.h>
#include <cntr2/idef.h>
#include <cntr2/iobject.h>

enum iterator_position {
	itr_begin,
	itr_end,
	itr_pos_count
};

#define itr_destroy iobject_destroy
#define itr_clone   iobject_clone
#define itr_equals  iobject_equals

/* itr_getvar return the reference to object on the global heap, you must destroy it on global default heap. */
extern inline unknown*       itr_get_obj  (const_iterator itr);
/* the object managed by the object will be destroyed, and a copy of __ref will replace its place */
extern inline void           itr_set_obj  (iterator itr, const unknown* __ref);
/* itr_get_ref return the object managed by the container internally, you can not modify it */
extern inline const unknown* itr_get_ref  (const_iterator itr);
/* TODO: there seems a relationship between readable and accessible, that is swap able, in which case, 
 * the itr is readable but can not be set a new value, instead it can swap to each other */
/* we would have a itr_set_ref, which set the ref managed by the container internally, but this will 
 * definitely cause memory leak, so we support another way of set_ref, called swap_ref, this api will
 * serve most case when we want to reorder the container */
extern inline void           itr_swap_ref (iterator itr, iterator other);
extern inline void           itr_to_prev  (iterator itr);
extern inline void           itr_to_next  (iterator itr);
extern inline void           itr_advance  (iterator itr, int length);
extern inline int            itr_distance (const_iterator from, const_iterator to);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef bool           (*pf_itr_equals)   (const_iterator itr_a, const_iterator itr_b);
typedef unknown*       (*pf_itr_get_obj)  (const_iterator citr);
typedef void           (*pf_itr_set_obj)  (iterator citr, const unknown* __ref);
typedef const unknown* (*pf_itr_get_ref)  (const_iterator citr);
typedef void           (*pf_itr_swap_ref) (iterator citr, iterator other);
typedef void           (*pf_itr_to_prev)  (iterator citr);
typedef void           (*pf_itr_to_next)  (iterator citr);
typedef void           (*pf_itr_advance)  (iterator citr, int length);
typedef int            (*pf_itr_distance) (const_iterator citr_from, const_iterator citr_to);

#define is_itrref(itr) (__cast(itr, ITR_REF_ID) != NULL)
#define is_itracc(itr) (__cast(itr, ITR_ACC_ID) != NULL)
#define is_itrfwd(itr) (__cast(itr, ITR_FWD_ID) != NULL)
#define is_itrbid(itr) (__cast(itr, ITR_BID_ID) != NULL)
#define is_itrrac(itr) (__cast(itr, ITR_RAC_ID) != NULL)

enum itr_implemetation_conversion {
	itr_interface_iobject = 0,
	itr_interface_iterator,
	itr_interface_count
};

/* extends itr_base_vtable */
struct itr_readable_vtable { 
	/* readable interface */
	pf_itr_get_obj          __get_obj;
	pf_itr_get_ref          __get_ref;
};

/* extends itr_readable_vtable */
struct itr_accessible_vtable {
	/* readable interface */
	pf_itr_get_obj          __get_obj;
	pf_itr_get_ref          __get_ref;

	/* accessible interface */
	pf_itr_set_obj          __set_obj;
	pf_itr_swap_ref         __swap_ref;
};

/* extends itr_accessible_vtable */
struct itr_forward_vtable {
	/* readable interface */
	pf_itr_get_obj          __get_obj;
	pf_itr_get_ref          __get_ref;

	/* accessible interface */
	pf_itr_set_obj          __set_obj;
	pf_itr_swap_ref         __swap_ref;

	/* forward interface */
	pf_itr_to_next          __to_next;
};

/* extends itr_forward_vtable */
struct itr_bidirectional_vtable {
	/* readable interface */
	pf_itr_get_obj          __get_obj;
	pf_itr_get_ref          __get_ref;

	/* accessible interface */
	pf_itr_set_obj          __set_obj;
	pf_itr_swap_ref         __swap_ref;

	/* forward interface */
	pf_itr_to_next          __to_next;

	/* bidirectional interface */
	pf_itr_to_prev          __to_prev;
};

/* extends itr_bidirectional_vtable */
struct itr_randomaccessible_vtable {
	/* readable interface */
	pf_itr_get_obj          __get_obj;
	pf_itr_get_ref          __get_ref;

	/* accessible interface */
	pf_itr_set_obj          __set_obj;
	pf_itr_swap_ref         __swap_ref;

	/* forward interface */
	pf_itr_to_next          __to_next;

	/* bidirectional interface */
	pf_itr_to_prev          __to_prev;

	/* random accessible interface */
	pf_itr_advance          __advance;
	pf_itr_distance         __distance;
};

#endif /* _INTERFACE_ITR_H_ */
