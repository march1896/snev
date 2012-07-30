#ifndef _CNTR_LIST_H_
#define _CNTR_LIST_H_

/*
 * This file define linked list container, for the design aspect of view see link.h for 
 * more details about the design of this file. 
 */

#include <cntr_iterator.h>

typedef struct open_link_t oplink;
typedef struct cntr_list_t {
	unsigned int size;
	unsigned int flags;

	oplink* begin;
	oplink* end;
} clist;

void clist_init(clist* pcl);
void clist_deinit(clist* pcl);

int clist_size(clist* pcl);

void* clist_front(clist* pcl);
void* clist_back (clist* pcl);

void  clist_add_front(clist* pcl, void* object);
void  clist_add_back (clist* pcl, void* object);
void  clist_add      (clist* pcl, void* object);

void* clist_remove_front(clist* pcl);
void* clist_remove_back (clist* pcl);

struct citer;
void  clist_citer_begin(clist* pcl, citer* itr);
void  clist_citer_end  (clist* pcl, citer* itr);

#endif /* _CNTR_LIST_H_ */
