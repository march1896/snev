#ifndef _CNTR_ABSTRACTION_
#define _CNTR_ABSTRACTION_

/*
 * This is file contains abstracts two types(linear/set) of container.
 *
 * The prototype is difined like below:
 *
 * clinear cntr;
 * cntr = clinear_as_list(&cntr);
 *
 * clinear_add(&cntr, &object);
 *
 * citer first, last;
 * clinear_citer_begin(&cntr, &first);
 * clinear_citer_end(&cntr, &last);
 *
 * quick_sort(&cntr, &last, compare_func)
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

void clinear_as_list(container cntr);

void clinear_as_array(container cntr);

void clinear_deinit(container cntr);

void clinear_clean(container cntr);

int  clinear_size(container cntr);

containee clinear_front(container cntr);

containee clinear_back(container cntr);

void clinear_add_front(container cntr, containee obj);

void clinear_add_back(container cntr, containee obj);

containee clinear_remove_front(container cntr);

containee clinear_remove_back(container cntr);

void clinear_citer_begin(container cntr, citer* itr);

void clinear_citer_end(container cntr, citer* itr);

void clinear_citer_copy(container cntr, citer* begin, citer* end);


#endif /* _CNTR_ABSTRACTION_ */
