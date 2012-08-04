#ifndef _CNTR_ABSTRACTION_
#define _CNTR_ABSTRACTION_

/*
 * This is file defines linear container.
 *
 * The prototype of linear is demoed as below:
 *
 * container cntr = clinear_as_list();
 *
 * clinear_add(cntr, &object);
 *
 * citer first, last;
 * citer_dos(first, NULL);
 * citer_dos(last, NULL);
 * clinear_iteraotr_begin(cntr, first);
 * clinear_iterator_end(cntr, last);
 *
 * quick_sort(first, last, compare_func)
 *
 * cset cntr;
 * cntr = cset_as_tree();
 *
 * cset_add(cntr, &object);
 *
 * citer_dos(itr);
 * if (cset_find(cntr, itr, &object)) { 
 *     cset_remove(cntr, itr);
 * }
 */

#include <cntr_iterator.h>

cntr clinear_as_list();

cntr clinear_as_array();

void clinear_destroy(cntr c);

void clinear_clear(cntr c);

int  clinear_size(cntr c);

void clinear_add(cntr c, void* obj);

void clinear_remove(cntr c, citer begin, citer end);

bool clinear_find(cntr c, void* obj, __out citer itr);

void* clinear_front(cntr c);

void* clinear_back(cntr c);

void clinear_add_front(cntr c, void* obj);

void clinear_add_back(cntr c, void* obj);

void* clinear_remove_front(cntr c);

void* clinear_remove_back(cntr c);

/*
 * TODO: container specific algorithms with iterator.
 */
void clinear_citer_begin(cntr c, citer itr);

void clinear_citer_end(cntr c, citer itr);


#endif /* _CNTR_ABSTRACTION_ */
