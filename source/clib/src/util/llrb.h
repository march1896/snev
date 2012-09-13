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

struct llrb_link *llrb_insert(struct llrb_link *r, struct llrb_link *n);

struct llrb_link *llrb_remove(struct llrb_link *r, struct llrb_link *n);

struct llrb_link *llrb_search(struct llrb_link *r, struct llrb_link *n);

void   llrb_init(struct llrb_link *n);

typedef bool (*pf_llrb_compare)(const struct llrb_link *l, const struct llrb_link *r);
void llrb_set_compare(pf_llrb_compare pc);


#endif /* _LLRB_LINK_H_ */

