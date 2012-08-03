#ifndef _CNTR_ABSTRACTION_
#define _CNTR_ABSTRACTION_

/*
 * This is file contains abstracts two types(linear/set) of container.
 *
 * The prototype is difined like below:
 *
 * container cntr = clinear_as_list();
 *
 * clinear_add(cntr, &object);
 *
 * citer first, last;
 * iterator first = iterator_create();
 * iterator last = iterator_create();
 * clinear_iteraotr_begin(cntr, first);
 * clinear_iterator_end(cntr, last);
 *
 * quick_sort(&cntr, &last, compare_func)
 *
 * iterator_destroy(first);
 * iterator_destroy(last);
 *
 * cset cntr;
 * cntr = cset_as_tree(&cntr);
 *
 * cset_add(&cntr, &object);
 *
 * if (cset_exist(&cntr, &object)) { do something; }
 *
 * citer itr;
 * cset_find(&cntr, &object, &itr);
 *
 * if (citer_valid(&itr)) {
 *     cset_remove(&cntr, &itr);
 * }
 *
 */

#include <cntr_iterator.h>

typedef unknown cntr;

cntr clinear_as_list();

cntr clinear_as_array();

void clinear_destroy(cntr c);

void clinear_clear(cntr c);

int  clinear_size(cntr c);

void* clinear_front(cntr c);

void* clinear_back(cntr c);

void clinear_add_front(cntr c, void* obj);

void clinear_add_back(cntr c, void* obj);

void* clinear_remove_front(cntr c);

void* clinear_remove_back(cntr c);

void clinear_citer_begin(cntr c, citer itr);

void clinear_citer_end(cntr c, citer itr);

void clinear_citer_copy(cntr c, citer begin, citer end);



#endif /* _CNTR_ABSTRACTION_ */
