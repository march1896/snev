#ifndef _INTERFACE_FACTORY_H_
#define _INTERFACE_FACTORY_H_

#include <oo_model.h>
#include <idef.h>

#define as_list(obj)  (iobject*)__cast((obj), ILIST_ID)
#define as_queue(obj) (iobject*)__cast((obj), IQUEUE_ID)
#define as_stack(obj) (iobject*)__cast((obj), ISTACK_ID)
#define as_set(obj)   (iobject*)__cast((obj), ISET_ID)
#define as_mset(obj)  (iobject*)__cast((obj), IMSET_ID)

/* compare two object that holds in the container, useful when the container holds object in some order */
typedef int   (*pf_compare)(const void* ref_a, const void* ref_b);

/* give the container right to dispose the object it holds, set this function only if you want to 
 * transfer the object lifetime management to the container */
typedef void  (*pf_dispose)(void* buff);

object* cntr_create_olist();
object* cntr_create_olist_v(allocator alc, pf_dispose dispose);

object* cntr_create_ollrb(pf_compare comp);
/* if alc is NULL, create_v will create with multi-pool allocator to gain best efficiency */
object* cntr_create_ollrb_v(pf_compare comp, allocator alc, pf_dispose dispose);

object* cntr_create_osplay(pf_compare comp);
/* if alc is NULL, create_v will create with multi-pool allocator to gain best efficiency */
object* cntr_create_osplay_v(pf_compare comp, allocator alc, pf_dispose dispose);

object* cntr_create_oskiplist(pf_compare comp);
/* if alc is NULL, create_v will create with multi-pool allocator to gain best efficiency */
object* cntr_create_oskiplist_v(pf_compare comp, allocator alc, pf_dispose dispose);

#endif /* _INTERFACE_FACTORY_H_ */
