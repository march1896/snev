#ifndef _CNTR_LIST_H_
#define _CNTR_LIST_H_

/*
 * This file defines linked list container, for the design aspect of view see link.h for 
 * more details about the design of this file. 
 */

#include <cntr_iterator.h>
typedef unknown clist;

clist clist_create();
void  clist_destroy    (clist cl);
void  clist_clear      (clist cl);
int   clist_size       (clist cl);
void  clist_add        (clist cl, void* object);
void  clist_remove     (clist cl, citer begin, citer end);
bool  clist_find       (clist cl, void* object, citer itr);
void  clist_citer_begin(clist cl, citer itr);
void  clist_citer_end  (clist cl, citer itr);

void* clist_front      (clist cl);
void* clist_back       (clist cl);
void  clist_add_front  (clist cl, void* object);
void  clist_add_back   (clist cl, void* object);
void* clist_remove_front(clist cl);
void* clist_remove_back (clist cl);


#endif /* _CNTR_LIST_H_ */
