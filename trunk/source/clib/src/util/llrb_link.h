#ifndef _LLRB_LINK_H_
#define _LLRB_LINK_H_

#include <cominc.h>

/*
 * LLRB 2-3 tree, a direct copy from Sedgewick's paper
 */
struct llrb_link {
	struct llrb_link*  left;
	struct llrb_link*  right;
	struct llrb_link*  parent;

	unsigned           color;
};

typedef int (*pf_llrb_compare)  (const struct llrb_link* l, const struct llrb_link* r);
/* TODO: two points must be clear 
 * 1, what happens when target is already in the tree.
 * 2, what happens when comp(somelink, target) == 0, in other words, what happens when target's value is in the tree. */
struct llrb_link* llrb_insert   (struct llrb_link* root, struct llrb_link* target, pf_llrb_compare comp);
struct llrb_link* llrb_insert_s (struct llrb_link* root, struct llrb_link* target, pf_llrb_compare comp, bool* dup);
struct llrb_link* llrb_remove   (struct llrb_link* root, struct llrb_link* target, pf_llrb_compare comp);

typedef int (*pf_llrb_compare_v)(const struct llrb_link* l, const struct llrb_link* r, void* param);
struct llrb_link* llrb_insert_v (struct llrb_link* root, struct llrb_link* target, pf_llrb_compare_v comp, void* param);
struct llrb_link* llrb_insert_sv(struct llrb_link* root, struct llrb_link* target, pf_llrb_compare_v comp, void* param, bool* dup);
struct llrb_link* llrb_remove_v (struct llrb_link* root, struct llrb_link* target, pf_llrb_compare_v comp, void* param);

typedef int (*pf_llrb_direct)(const struct llrb_link* cur, void* param);
struct llrb_link* llrb_search(struct llrb_link* root, pf_llrb_direct direct, void* param);

/* TODO: why this interface is here */
void llrb_swap_link  (struct llrb_link** pa, struct llrb_link** pb);

void llrb_debug_check(struct llrb_link* root, pf_llrb_compare comp);

struct llrb_link* llrb_min(struct llrb_link* root);

struct llrb_link* llrb_max(struct llrb_link* root);

struct llrb_link* llrb_predesessor(const struct llrb_link* link, bool only_sub);

struct llrb_link* llrb_successor  (const struct llrb_link* link, bool only_sub);

#endif /* _LLRB_LINK_H_ */

