#ifndef _LLRB_LINK_H_
#define _LLRB_LINK_H_

#include <cominc.h>

/**
 * @brief LLRB 2-3 tree, a direct copy from Sedgewick's paper, 
 * 		http://www.cs.princeton.edu/~rs/talks/LLRB/LLRB.pdf
 */
struct llrb_link {
	struct llrb_link*  left;
	struct llrb_link*  right;
	struct llrb_link*  parent;

	unsigned           color;
};

typedef int (*pf_llrb_compare)  (const struct llrb_link* l, const struct llrb_link* r);
typedef int (*pf_llrb_compare_v)(const struct llrb_link* l, const struct llrb_link* r, void* param);
/**
 * @brief insert target into a tree.
 * 		if root == target, this function will change nothing, just return the original tree root.
 * 		if comp(root, target) == 0, it will directly compare the address of current line and target to insert.
 *
 * @param root the original root.
 * @param target the new link to insert.
 * @param comp tree compare function.
 *
 * @return the new tree root after insertion.
 */
struct llrb_link* llrb_insert(struct llrb_link* root, struct llrb_link* target, pf_llrb_compare comp);

/**
 * @brief insert a single instance into the tree.
 * 		if comp(root, target) == 0, the tree will not changed, *dup will be set to true.
 * 		if comp(root, target) != 0, target will be insert into the tree, *dup will be false.
 *
 * @param root the original tree root.
 * @param target the new link to insert.
 * @param comp tree compare function.
 * @param dup represent the target is in the tree or not.
 *
 * @return the new tree root after insertion.
 */
struct llrb_link* llrb_insert_s(struct llrb_link* root, struct llrb_link* target, pf_llrb_compare comp, bool* dup);

/**
 * @brief remove a link from a tree.
 * 		if target is not in the tree, the behavior is NOT DEFINED.
 * 		if target is in the tree, it will be removed.
 *
 * @param root the original tree root.
 * @param target the link to be removed.
 * @param comp tree compare function.
 *
 * @return the new tree root after remove, NULL if no element in the tree.
 */
struct llrb_link* llrb_remove(struct llrb_link* root, struct llrb_link* target, pf_llrb_compare comp);

/* the verbose version of insert/remove */
struct llrb_link* llrb_insert_v(struct llrb_link* root, struct llrb_link* target, pf_llrb_compare_v comp, void* param);
struct llrb_link* llrb_insert_sv(struct llrb_link* root, struct llrb_link* target, pf_llrb_compare_v comp, void* param, bool* dup);
struct llrb_link* llrb_remove_v(struct llrb_link* root, struct llrb_link* target, pf_llrb_compare_v comp, void* param);

/**
 * @brief llrb_search callback, given the current link, this fucntion will deside which child will be searched next.
 * 		if return value < 0, the left child tree will be searched.
 * 		if return value == 0, the current link will be returned.
 * 		if return value > 0, the right child tree will be searched.
 *
 * @param the current tree link.
 * @param param custom parameter which is used for desiding which side to search next.
 *
 * @return see @brief
 */
typedef int (*pf_llrb_direct)(const struct llrb_link* cur, void* param);

struct llrb_link* llrb_search(struct llrb_link* root, pf_llrb_direct direct, void* param);

struct llrb_link* llrb_min        (struct llrb_link* root);
struct llrb_link* llrb_max        (struct llrb_link* root);
struct llrb_link* llrb_predesessor(const struct llrb_link* link, bool only_sub);
struct llrb_link* llrb_successor  (const struct llrb_link* link, bool only_sub);

void              llrb_debug_check(struct llrb_link* root, pf_llrb_compare comp);
void              llrb_debug_check_v(struct llrb_link* root, pf_llrb_compare_v comp_v, void* param);


#endif /* _LLRB_LINK_H_ */

