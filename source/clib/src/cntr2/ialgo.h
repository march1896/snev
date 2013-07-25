#ifndef _INTERFACE_ALGORITHM_H_
#define _INTERFACE_ALGORITHM_H_

#include <oo_model.h>
#include <idef.h>

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

typedef void (*pf_ref_process)  (void* __ref);
typedef void (*pf_ref_process_v)(void* __ref, void* param);
typedef bool (*pf_ref_compare)  (void* ref_a, void* ref_b);

void foreach     (itrfwd begin, itrfwd end, pf_ref_process cb);
void foreach_v   (itrfwd begin, itrfwd end, pf_ref_process_v cb, void* param);

void sort_b      (itrbid begin, itrbid end, pf_ref_compare comp);
void sort_r      (itrrac begin, itrrac end, pf_ref_compare comp);

void reverse_b   (itrbid begin, itrbid end);

void prev_permutation_b(itrbid begin, itrbid end);

void next_permutation_b(itrbid begin, itrbid end);

#endif /* _INTERFACE_ALGORITHM_H_ */
