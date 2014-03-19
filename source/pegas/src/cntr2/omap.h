#ifndef _OBJECT_ORDERED_MAP_H_
#define _OBJECT_ORDERED_MAP_H_

void           imap_destroy        (object* o);
void           imap_clear          (object* o);
void           imap_clear_v        (object* o, pf_ref_dispose_v dispose, void* context);
int            imap_size           (const object* o);
bool           imap_empty          (const object* o);
void*          imap_insert_s       (object* o, const void* key, void* value);
void*          imap_insert_m       (object* o, const void* key, void* value);
bool           imap_contains       (const object* o, const void* key);
void*          imap_findbykey      (const object* o, const void* key);
void*          imap_remove         (object* o, void* key);
iterator       imap_itr_create     (const object* o, itr_pos pos);
void           imap_itr_assign     (const object* o, /* out */iterator itr, itr_pos pos);

void           imap_itr_find       (const object* o, /* out */iterator itr, void* key);

void*          imap_itr_remove     (object* o, iterator itr);
const_iterator imap_itr_begin      (const object* o);
const_iterator imap_itr_end        (const object* o);

int            immap_count         (const object* o, const void* key);
void           immap_itr_find_lower(const object* o, /* out */iterator itr, void* key);
void           immap_itr_find_upper(const object* o, /* out */iterator itr, void* key);

#endif
