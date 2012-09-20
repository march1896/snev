#ifndef _DOUBLE_LINKED_LIST_DOC_H_
#define _DOUBLE_LINKED_LIST_DOC_H_

/* 
 * This file define data structure and algorithm for double linked list.
 * ****** open link list are moved to link_cntr.h, and renamed as *******
 * ****** list container, so open link mentioned in this file are *******
 * ****** just concepts in design phrase, not implemented.        *******
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
 * From operations aspeck, here are three basic operations for list.
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
 *
 ***********************************************************************************
 *
 * About memory management:
 * * For OBJECTS that connected with links, we are not taking care of their lifetime,
 *   clients should master the objects' allocation/deallocation themselves. On the 
 *   contrary, C++ containers may manage the objects lifetime, since for each class
 *   compiler add default copy constructor, and also it use templates, so container 
 *   knows what exactly the object is. But as the same as JAVA, C++ could also keep 
 *   pointers in the container, and leave the lifetime management to GC or the client.
 *   Back to the concrete problem, in c language, the list knows nothing about what is 
 *   saved in it(by some tricky #define, we may get the type information), so list will
 *   LEAVE LIFETIME MANAGEMENT for LIST ELEMENTS TO YOU.
 * * For the link pointers(prev/next), we should hide details. The link pointers 
 *   should be transparent to clients. In other words, for open links, the container  
 *   should maintain(allocate/deallocate) link pointers' memory itself. For embeded 
 *   links the pointers' memory are allocated when the object is ready, since we do not 
 *   touch the memory management of elements, we also do not need to (allocate/deallocate)
 *   the memory for list pointers.
 *
 ***********************************************************************************
 *
 * About whether or not expose iterator.
 * First, what is an iterator, iterator is absolutely not a parent(unique name) or elements
 * strored in all kinds of containers, it is a design pattern. from wikipedia, iterator 
 * design patter is defined as:
 *   "iterator pattern is a design pattern in which an iterator is used to traverse a 
 *   container and access the container's elements. The iterator pattern decouples 
 *   algorithms from containers; in some cases, algorithms are necessarily container-specific
 *   and thus cannot be decoupled."
 * In C++, std::algorithm implements most of generic algorithms, that can be shared by containers,
 * in JAVA, I have not found a std::algorithm like lib but it is easy to write generic 
 * algorithms by passing only iterator as parameter.
 * In C, if we use object-oriented stills, like open list, we should also implements 
 * a set of algorithm that only operates on iterator, so decouple these algorithms from 
 * container, but for OO, iterator knows about itself( it knows that operations like prev/next)
 * by virtual functions, so in C, if we want to implement generic algorithm, we must give 
 * callbacks to iterator to let it maintain itself, like below,
 *   struct iterator_operations {
 *     struct iterator* (*pf_prev)(iterator* cur);
 *     struct iterator* (*pf_next)(iterator* cur);
 *   }
 *   struct iterator {
 *     void* itr; // pointer to the iterator
 *     iterator_operations iop;
 *   }
 * In other words, we should manully implement virtual functions before we using iterator  
 * design pattern.
 *
 ***********************************************************************************
 *
 * Conclusion:
 * * For open link list, since it is a object-oriented container like structure, we could 
 *   provide iterators. But iterator has nothing relate to link headers, they different 
 *   concepts. And for method on open list, we provided opertions that from container, like
 *   find/add/remove, and other generic algorithm will be provided by iterator.
 * * For embeded link list, since we have expose the detail of link to the objects, and what 
 *   we do is to link thing together, if structure using embeded links, it only means they can 
 *   be connected. 
 *
 */

#endif /* _DOUBLE_LINKED_LIST_DOC_H_ */
