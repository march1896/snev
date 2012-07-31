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

typedef void* container;

container clinear_as_list();

container clinear_as_array();

void clinear_deinit(container cntr);

void clinear_clean(container cntr);

int  clinear_size(container cntr);

void* clinear_front(container cntr);

void* clinear_back(container cntr);

void clinear_add_front(container cntr, void* obj);

void clinear_add_back(container cntr, void* obj);

void* clinear_remove_front(container cntr);

void* clinear_remove_back(container cntr);

void clinear_citer_begin(container cntr, citer* itr);

void clinear_citer_end(container cntr, citer* itr);

void clinear_citer_copy(container cntr, citer* begin, citer* end);


#endif /* _CNTR_ABSTRACTION_ */
