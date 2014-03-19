#ifndef _INTERFACE_MAP_H_
#define _INTERFACE_MAP_H_

/* the interface functions which support, used by clients */
#include <cntr2/oo_model.h>
#include <cntr2/idef.h>

typedef struct map_pair_t {
	const unknown* key;
	unknown*       value;
} map_pair;

extern inline void           imap_clear            (_object* obj);
extern inline int            imap_size             (const _object* obj);
extern inline bool           imap_empty            (const _object* obj);
/* if <key, old_value> pair in the map, the old_value will be updated, and the old_value 
 * will be returned, if not, return NULL. */
extern inline void           imap_insert           (_object* obj, const unknown* key, const unknown* value);
extern inline void           imap_insert_p         (_object* obj, const map_pair* pair);
extern inline bool           imap_contains         (const _object* obj, const unknown* key);
/* return NULL if the key is not in the map */
extern inline const unknown* imap_findbykey        (const _object* obj, const unknown* key);
extern inline void           imap_remove           (_object* obj, const unknown* key);

extern inline iterator       imap_itr_create       (const _object* obj, itr_pos pos);
extern inline void           imap_itr_assign       (const _object* obj, /* out */iterator itr, itr_pos pos);
/* points to end if key is not in the map */
extern inline void           imap_itr_find         (const _object* obj, /* out */iterator itr, const unknown* key);
extern inline void           imap_itr_remove       (_object* obj, iterator itr);
/* return a iterator, maybe forward/bidirectional/random accessed. */
extern inline const_iterator imap_itr_begin        (const _object* obj);
extern inline const_iterator imap_itr_end          (const _object* obj);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef       void           (*pf_imap_clear)      (_object* c);
typedef       int            (*pf_imap_size)       (const _object* c);
typedef       bool           (*pf_imap_empty)      (const _object* c);
typedef       void           (*pf_imap_insert)     (_object* c, const unknown* key, const unknown* value);
typedef       bool           (*pf_imap_contains)   (const _object* c, const unknown* key);
typedef       const unknown* (*pf_imap_findbykey)  (const _object* c, const unknown* key);
typedef       void*          (*pf_imap_remove)     (_object* c, const unknown* key);

typedef       iterator       (*pf_imap_itr_create) (const _object* c, itr_pos pos);
typedef       void           (*pf_imap_itr_assign) (const _object* c, iterator itr, itr_pos pos);
typedef       void           (*pf_imap_itr_find)   (const _object* c, iterator itr, const unknown* key);
typedef       void           (*pf_imap_itr_remove) (_object* c, iterator itr);
typedef       const_iterator (*pf_imap_itr_begin)  (const _object* c);
typedef       const_iterator (*pf_imap_itr_end)    (const _object* c);

struct imap_vtable {
	pf_imap_clear       __clear;
	pf_imap_size        __size;
	pf_imap_empty       __empty;
	pf_imap_insert      __insert;
	pf_imap_contains    __contains;
	pf_imap_findbykey   __findbykey;
	pf_imap_remove      __remove;

	pf_imap_itr_create  __itr_create;
	pf_imap_itr_assign  __itr_assign;
	pf_imap_itr_find    __itr_find;
	pf_imap_itr_remove  __itr_remove;
	pf_imap_itr_begin   __itr_begin;
	pf_imap_itr_end     __itr_end;
};

/*******************************************************************************
 * immap declaration start 
 ******************************************************************************/
extern inline void           immap_destroy          (_object* i);
extern inline void           immap_clear            (_object* i);
extern inline int            immap_size             (const _object* i);
extern inline bool           immap_empty            (const _object* i);
extern inline void           immap_insert           (_object* i, const unknown* key, const unknown* value);
extern inline bool           immap_contains         (const _object* i, const unknown* key);
/* return number of reference of key */
extern inline int            immap_count            (const _object* i, const unknown* key);
extern inline bool           immap_remove           (_object* i, const unknown* key);

extern inline iterator       immap_itr_create       (const _object* i, itr_pos pos);
extern inline void           immap_itr_assign       (const _object* i, /* out */iterator itr, itr_pos pos);
/* find the minimum element which is greater or equal to key */
extern inline void           immap_itr_find_lower   (const _object* i, /* out */iterator itr, const unknown* key);
/* find the minimum element which is greater than key */
extern inline void           immap_itr_find_upper   (const _object* i, /* out */iterator itr, const unknown* key);
extern inline void           immap_itr_remove       (_object* i, iterator itr);
/* return a iterator, maybe forward/bidirectional/random accessed. */
extern inline const_iterator immap_itr_begin        (const _object* i);
extern inline const_iterator immap_itr_end          (const _object* i);

/* below is only useful for the container implementer */
/* the virtual functions that each container should implement */
typedef       void           (*pf_immap_destroy)    (_object* o);
typedef       void           (*pf_immap_clear)      (_object* o);
typedef       int            (*pf_immap_size)       (const _object* o);
typedef       bool           (*pf_immap_empty)      (const _object* o);
typedef       void           (*pf_immap_insert)     (_object* o, const unknown* key, const unknown* value);
typedef       bool           (*pf_immap_contains)   (const _object* o, const unknown* key);
typedef       int            (*pf_immap_count)      (const _object* o, const unknown* key);
typedef       bool           (*pf_immap_remove)     (_object* o, const unknown* key);

typedef       iterator       (*pf_immap_itr_create) (const _object* o, itr_pos pos);
typedef       void           (*pf_immap_itr_assign) (const _object* o, iterator itr, itr_pos pos);
typedef       void           (*pf_immap_itr_find_lower)(const _object* o, iterator itr, const unknown* key);
typedef       void           (*pf_immap_itr_find_upper)(const _object* o, iterator itr, const unknown* key);
typedef       void           (*pf_immap_itr_remove) (_object* o, iterator itr);
typedef const_iterator       (*pf_immap_itr_begin)  (const _object* o);
typedef const_iterator       (*pf_immap_itr_end)    (const _object* o);

struct immap_vtable {
	/* public */
	pf_immap_clear          __clear;
	pf_immap_size           __size;
	pf_immap_empty          __empty;
	pf_immap_insert         __insert;
	pf_immap_contains       __contains;
	pf_immap_count          __count;
	pf_immap_remove         __remove;

	pf_immap_itr_create     __itr_create;
	pf_immap_itr_assign     __itr_assign;
	pf_immap_itr_find_lower __itr_find_lower;
	pf_immap_itr_find_upper __itr_find_upper;
	pf_immap_itr_remove     __itr_remove;
	pf_immap_itr_begin      __itr_begin;
	pf_immap_itr_end        __itr_end;
};

#endif /* _INTERFACE_MAP_H_ */
