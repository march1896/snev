#ifndef _CNTR_BASE_LOCAL_H_
#define _CNTR_BASE_LOCAL_H_

typedef cattr (*pf_cntr_attribute)      (cntr c);
typedef void (*pf_cntr_base_destroy)    (cntr c);
typedef void (*pf_cntr_base_clear)      (cntr c);
typedef int  (*pf_cntr_base_size)       (cntr c);
typedef void (*pf_cntr_base_add)        (cntr c, void* object);
typedef void (*pf_cntr_base_remove)     (cntr c, citer begin, citer end);
typedef bool (*pf_cntr_base_find)       (cntr c, void* object, __out citer result);
typedef void (*pf_cntr_base_citer_begin)(cntr c, citer itr);
typedef void (*pf_cntr_base_citer_end)  (cntr c, citer itr);


/* TODO: change the number */
#define CNTR_BASE_INTERFACE 0X00000001
struct cntr_base_interface  {
	/* public */
	pf_cntr_attribute        __attrib;
	pf_cntr_base_destroy     __destroy;
	pf_cntr_base_clear       __clear;
	pf_cntr_base_size        __size;
	pf_cntr_base_add         __add;
	pf_cntr_base_remove      __remove;
	pf_cntr_base_find        __find;
	pf_cntr_base_citer_begin __citer_begin;
	pf_cntr_base_citer_end   __citer_end;
};

/* 
 * we have to add allocate/deallocate callback to let the container control
 * the internal memory usage, such as the link's memory.
 * they do not belong to the interfaces, since they vary from object to object.
 * this could be made as an interface, should we ? TODO: ohh.
 */
typedef void* (*pf_cntr_inner_alloc  ) (int size);
typedef void  (*pf_cntr_inner_dealloc) (void* buff);
/*
 * helper functions.
 */

extern inline bool cntr_check_attr(cntr c, cattr base);

#endif /* _CNTR_BASE_LOCAL_H_ */
