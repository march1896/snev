#ifndef _OO_MODEL_H_
#define _OO_MODEL_H_

/*
 * For more detail about the oo model, please see oo_model.doc.h
 */
typedef void*        unknown;
typedef unsigned int unique_id;

#define OBJECT_ME 0X7F7F7F7F
/* MAKE SURE the actual callback will cast OBJECT_ME to its own address, 
 * this is essential for the error check, since we really have little information about type. */
typedef unknown (*pf_cast)(unknown obj, unique_id interface_id);

struct object {
	pf_cast                   __cast;

	/* interfaces below: */
	/*
	struct interface*         __intf1
	struct interface*         __intf1
	*/

	/* dynamic members below */
	/*
	int x;
	struct someobject y;
	*/
};

inline unknown __cast(unknown x, unique_id id) {
	struct object* obj = ((struct object*)x);

	dbg_assert(obj->__cast(x, OBJECT_ME) == x);
	return obj->__cast(x, id);
}

#endif /* _OO_MODEL_H_ */
