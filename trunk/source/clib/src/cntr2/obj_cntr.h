#ifndef _OBJECT_CNTR_H_
#define _OBJECT_CNTR_H_

#include <cntr_def.h>

/* cntr base interface functions begin */
void cntr_destroy     (cntr c);

void cntr_clear       (cntr c);

int  cntr_size        (cntr c);

void cntr_add         (cntr c, void* object);

void cntr_remove      (cntr c, citer begin, citer end);

bool cntr_find        (cntr c, void* object, /* __out */citer result);

void cntr_citer_begin (cntr c, citer itr);

void cntr_citer_end   (cntr c, citer itr);
/* cntr base interface functions end */

/* cntr linear interface function begin */
void* cntr_front      (cntr c);

void* cntr_back       (cntr c);

void  cntr_add_front  (cntr c, void* obj);

void  cntr_add_back   (cntr c, void* obj);

void* cntr_remove_front(cntr c);

void* cntr_remove_back(cntr c);
/* cntr linear interface function end */

#endif /* _OBJECT_CNTR_H_ */
