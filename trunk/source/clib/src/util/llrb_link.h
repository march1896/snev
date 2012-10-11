#ifndef _LLRB_LINK_H_
#define _LLRB_LINK_H_

#include <cominc.h>

/*
 * LLRB 2-3 tree, a direct copy from Sedgewick's paper
 */
struct llrb_link {
	struct llrb_link *left;
	struct llrb_link *right;
	struct llrb_link *parent;

	unsigned     color;
};
typedef int (*pf_llrb_compare)(const struct llrb_link *l, const struct llrb_link *r);

struct llrb_link *llrb_insert(struct llrb_link *r, struct llrb_link *n, pf_llrb_compare pc);

struct llrb_link *llrb_remove(struct llrb_link *r, struct llrb_link *n, pf_llrb_compare pc);

void llrb_swap_link(struct llrb_link **pa, struct llrb_link **pb);

void llrb_debug_check(struct llrb_link *root, pf_llrb_compare pc);

#endif /* _LLRB_LINK_H_ */

