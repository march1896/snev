#ifndef _INTERFACE_FACTORY_H_
#define _INTERFACE_FACTORY_H_

#include <oo_model.h>
#include <idef.h>

#define as_list(obj)  (iobject*)__cast((obj), ILIST_ID)
#define as_queue(obj) (iobject*)__cast((obj), IQUEUE_ID)
#define as_stack(obj) (iobject*)__cast((obj), ISTACK_ID)
#define as_set(obj)   (iobject*)__cast((obj), ISET_ID)

/* TODO: the below functions should be moved to some header named like object_trait.h */
/* now we only keep reference, not instance, in container, so copy is useless.
typedef void* (*pf_copy   )(void* buff);
 */

/* compare two object that holds in the container, useful when the container holds object in some order */
typedef bool  (*pf_compare)(const void* ref_a, const void* ref_b);

/* give the container right to dispose the object it holds, set this function only if you want to 
 * transfer the object lifetime management to the container */
typedef void  (*pf_dispose)(void* buff);

object* create_dblinked_list();
object* create_dblinked_list_v(allocator alc, pf_dispose dispose);

#endif /* _INTERFACE_FACTORY_H_ */
