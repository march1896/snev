#ifndef _LIST_CNTR_H_
#define _LIST_CNTR_H_

/*
 * This file define linked list container, for the design aspect of view see link.h for 
 * more details about the design of this file. 
 */

#include <cntr_iterator.h>

struct oplink;
struct cntr_list_t {
	unsigned int size;
	unsigned int flags;

	struct oplink* begin;
	struct oplink* end;
};

typedef cntr_list_t clist;

void* clist_front(clist* pcl);
void* clist_back (clist* pcl);

void  clist_add_front(clist* pcl, void* object);
void  clist_add_back (clist* pcl, void* object);
void  clist_add      (clist* pcl, void* object);

void  clist_remove_front(clist* pcl);
void  clist_remove_back (clist* pcl);

struct citer;
void  clist_citer_begin(clist* pcl, citer* itr);
void  clist_citer_end  (clist* pcl, citer* itr);

#define /* _LIST_CNTR_H_ */
