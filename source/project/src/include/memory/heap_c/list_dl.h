#ifndef _DOUBLE_LINKED_LIST_H_
#define _DOUBLE_LINKED_LIST_H_

/* 
 * This file define data structure and algorithm for double linked list.
 *
 * From implementation aspect of view, there are many ways make linked-list 
 * common, to make as little work as possible for people who want to use 
 * linked list. The main difference between these different implementations is 
 * the strategy to maintain the pointers that link objects together.
 * * The first way is to let a common linked list to carry object's information,
 * * the second way is to embed the pointers into object's definition.
 * Since this is a experimental project, both ways are implemented.
 *
 * From the shape of list, it can be divided into two.
 * * The list is closed, it constructs a loop
 * * The list is open
 *
 * From the ording of elements aspect, list can be divided into two types
 * * Elements in list are ordered.
 * * Elements in list are random placed.
 *
 * From operations aspeck, hhere are three basic operations for list.
 * * search/find/exist find a certain object in a list, or find an object that 
 *   satisfy some constraints. this operation can be extend to find a set of 
 *   objects.
 * * insert insert an object into a list.
 * * erase erase an object from a list. when we want to erase a element from 
 *   list, we may instantly got a handle from somewhere, and when we try to 
 *   erase this element, we should keep in mind that this element maybe the 
 *   first or even only element in that list. So after erasing this element,
 *   the list entry point should be changed, there are two ways to avoid this
 *   case, one is to keep a pointer to the address of entry point, and change 
 *   it when needed, the other is to keep a sentinel element, the sentinel will
 *   be the header of the list and never changed, so the list entry point will
 *   always points to it. here we will choose the second method.
 */

struct link_header_t {
	void*        plist;
	unsigned int flags;
};

struct link_iterator_t {

}

struct link_dll_t {
	struct link_dll_t* prev;
	struct link_dll_t* next;

	void* object;
};

struct link_embeded_dll_t {
	struct link_embeded_dll_t* prev;
	struct link_embeded_dll_t* next;
};



#endif /* _DOUBLE_LINKED_LIST_H_ */
