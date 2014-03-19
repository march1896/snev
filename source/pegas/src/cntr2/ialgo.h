#ifndef _INTERFACE_ALGORITHM_H_
#define _INTERFACE_ALGORITHM_H_

#include <cntr2/oo_model.h>
#include <cntr2/idef.h>

/* 
 * the problem of open interval VS closed interval.
 * for a pair of iterators (begin, end), what the interval it represent?
 * generally, there are to kinds.
 * [begin, end) (half opened interval, which is used in C++ STL)
 * [begin, end] (closed interval, which is easier to implement, I think)
 *
 * I used to think this is only a style problem, you can choose any of them 
 * obey it all the way.
 *
 * So what happened when acquire a begin/end iterator for an empty container?
 * The iterator is not well defined. 
 * [begin, end) style seems solved this problem. end, as a sentinel, works well.
 *
 * But this half opened interval also has its own drawbacks.
 * It assumes that the algorithm will traverse from begin, and finally walks to end.
 */

/* iterator should be at least forward able.
 * the callback will get a reference managed by the container internally, modify it on your own risk. */
void foreach     (const_iterator begin, const_iterator end, pf_ref_visit cb);
/* iterator should be at least forward able.
 * the callback will get a reference managed by the container internally, modify it on your own risk. */
void foreach_v   (const_iterator begin, const_iterator end, pf_ref_visit_v cb, void* param);

/* bidirectional iterator sort */
void sort_b      (const_iterator begin, const_iterator end, pf_ibridge_compare comp);
/* random accessible iterator sort */
void sort_r      (const_iterator begin, const_iterator end, pf_ibridge_compare comp);

/* iterator should be bidirectional */
void reverse_b   (const_iterator begin, const_iterator end);

/* iterator should be bidirectional */
void prev_permutation_b(const_iterator begin, const_iterator end);

/* iterator should be bidirectional */
void next_permutation_b(const_iterator begin, const_iterator end);

#endif /* _INTERFACE_ALGORITHM_H_ */
