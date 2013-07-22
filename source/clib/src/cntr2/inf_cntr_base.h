#ifndef _CNTR_BASE_INTERFACE_H_
#define _CNTR_BASE_INTERFACE_H_

struct base_interface; /* from oo_model.h */

/* the interface functions which support, used by clients */

extern inline void inf_cntr_base_destroy     (struct base_interface* cntr_base);

extern inline void inf_cntr_base_clear       (struct base_interface* cntr_base);

extern inline int  inf_cntr_base_size        (struct base_interface* cntr_base);

extern inline void inf_cntr_base_add         (struct base_interface* cntr_base, void* object);

extern inline void inf_cntr_base_remove      (struct base_interface* cntr_base, citer begin, citer end);

extern inline bool inf_cntr_base_find        (struct base_interface* cntr_base, void* object, /* __out */citer result);

extern inline void inf_cntr_base_citer_begin (struct base_interface* cntr_base, citer itr);

extern inline void inf_cntr_base_citer_end   (struct base_interface* cntr_base, citer itr);

/* 
 * since the interface should get the address of the object, the below functions give fast access 
 * in case you know the object which carry the interface, in other words.
 * __object_from_interface(cntr_base) == obj
 */
extern inline void inf_cntr_base_destroy_o     (struct base_interface* cntr_base, unknown obj);

extern inline void inf_cntr_base_clear_o       (struct base_interface* cntr_base, unknown obj);

extern inline int  inf_cntr_base_size_o        (struct base_interface* cntr_base, unknown obj);

extern inline void inf_cntr_base_add_o         (struct base_interface* cntr_base, unknown obj, void* object);

extern inline void inf_cntr_base_remove_o      (struct base_interface* cntr_base, unknown obj, citer begin, citer end);

extern inline bool inf_cntr_base_find_o        (struct base_interface* cntr_base, unknown obj, void* object, /* __out */citer result);

extern inline void inf_cntr_base_citer_begin_o (struct base_interface* cntr_base, unknown obj, citer itr);

extern inline void inf_cntr_base_citer_end_o   (struct base_interface* cntr_base, unknown obj, citer itr);


/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef void (*pf_cntr_base_destroy)    (cntr c);
typedef void (*pf_cntr_base_clear)      (cntr c);
typedef int  (*pf_cntr_base_size)       (cntr c);
typedef void (*pf_cntr_base_add)        (cntr c, void* object);
typedef void (*pf_cntr_base_remove)     (cntr c, citer begin, citer end);
typedef bool (*pf_cntr_base_find)       (cntr c, void* object, /*__out*/ citer result);
typedef void (*pf_cntr_base_citer_begin)(cntr c, citer itr);
typedef void (*pf_cntr_base_citer_end)  (cntr c, citer itr);


/* TODO: change the number */
#define CNTR_BASE_INTERFACE_ID 0X00000001
#define CNTR_BASE_INTERFACE_OFFSET 0
struct inf_cntr_base_vtable {
	/* public */
	pf_cntr_base_destroy     __destroy;
	pf_cntr_base_clear       __clear;
	pf_cntr_base_size        __size;
	pf_cntr_base_add         __add;
	pf_cntr_base_remove      __remove;
	pf_cntr_base_find        __find;
	pf_cntr_base_citer_begin __citer_begin;
	pf_cntr_base_citer_end   __citer_end;
};

#endif /* _CNTR_BASE_INTERFACE_H_ */
