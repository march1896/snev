#ifndef _INTERFACE_FACTORY_H_
#define _INTERFACE_FACTORY_H_

#include <oo_model.h>
#include <imemmgr.h>

/*
iobject* as_list(object* o);
iobject* as_queue(object* o);
iobject* as_stack(object* o);
iobject* as_set(object* o);
*/

#define as_list(obj)  (iobject*)__cast((obj), ILIST_ID)
#define as_queue(obj) (iobject*)__cast((obj), IQUEUE_ID)
#define as_stack(obj) (iobject*)__cast((obj), ISTACK_ID)
#define as_set(obj)   (iobject*)__cast((obj), ISET_ID)

#define as_fitr(obj)  (iobject*)__cast((obj), IFITR_ID)
#define as_bitr(obj)  (iobject*)__cast((obj), IBITR_ID)
#define as_ritr(obj)  (iobject*)__cast((obj), IRITR_ID)

object* create_dblinked_list();
object* create_dblinked_list_v(pf_alloc alloc, pf_dealloc dealloc, pf_copy copy, pf_dispose dispose);

#endif /* _INTERFACE_FACTORY_H_ */
