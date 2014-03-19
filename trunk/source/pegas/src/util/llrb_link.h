#ifndef _LLRB_LINK_H_
#define _LLRB_LINK_H_

#include <util/cominc.h>

/**
 * @brief LLRB 2-3 tree, a direct copy from Sedgewick's paper, 
 * 		http://www.cs.princeton.edu/~rs/talks/LLRB/LLRB.pdf
 */
struct llrblink {
	struct llrblink*  left;
	struct llrblink*  right;
	struct llrblink*  parent;

	unsigned           color;
};

typedef int (*pf_llrb_compare)  (const struct llrblink* l, const struct llrblink* r);
typedef int (*pf_llrb_compare_v)(const struct llrblink* l, const struct llrblink* r, void* param);

/**
 * @brief insert target into a tree.
 * 		if comp(tree_node, target) == 0, it will directly compare the address of tree_node and target to insert.
 * 		if tree_node == target, this function will change nothing, just return the original tree root.
 *
 * @param root the original root.
 * @param target the new link to insert.
 * @param comp tree compare function.
 *
 * @return the new tree root after insertion.
 */
struct llrblink* llrb_insert(struct llrblink* root, struct llrblink* target, pf_llrb_compare comp);

/**
 * @brief insert a single instance into the tree.
 * 		if comp(tree_node, target) == 0, the tree will not changed, *dup will be set to tree_node.
 * 		if comp(tree_node, target) != 0, target will be insert into the tree, *dup will be NULL.
 *
 * @param root the original tree root.
 * @param target the new link to insert.
 * @param comp tree compare function.
 * @param dup represent the target is in the tree or not.
 *
 * @return the new tree root after insertion.
 */
struct llrblink* llrb_insert_s(struct llrblink* root, struct llrblink* target, pf_llrb_compare comp, struct llrblink** dup);

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
struct llrblink* llrb_remove(struct llrblink* root, struct llrblink* target, pf_llrb_compare comp);

/* the verbose version of insert/remove */
struct llrblink* llrb_insert_v(struct llrblink* root, struct llrblink* target, pf_llrb_compare_v comp, void* param);
struct llrblink* llrb_insert_sv(struct llrblink* root, struct llrblink* target, pf_llrb_compare_v comp, void* param, struct llrblink** dup);
struct llrblink* llrb_remove_v(struct llrblink* root, struct llrblink* target, pf_llrb_compare_v comp, void* param);

/**
 * @brief llrb_search callback, given the current link, this function will decide which child will be searched next.
 * 		if return value < 0, the left child tree will be searched.
 * 		if return value == 0, the current link will be returned.
 * 		if return value > 0, the right child tree will be searched.
 *
 * @param the current tree link.
 * @param param custom parameter which is used for deciding which side to search next.
 *
 * @return see @brief
 */
typedef int (*pf_llrb_direct)(const struct llrblink* cur, void* param);

struct llrblink* llrb_search(struct llrblink* root, pf_llrb_direct direct, void* param);

struct llrblink* llrb_min        (struct llrblink* root);
struct llrblink* llrb_max        (struct llrblink* root);
struct llrblink* llrb_predesessor(const struct llrblink* link, bool only_sub);
struct llrblink* llrb_successor  (const struct llrblink* link, bool only_sub);

void              llrb_debug_check(struct llrblink* root, pf_llrb_compare comp);
void              llrb_debug_check_v(struct llrblink* root, pf_llrb_compare_v comp_v, void* param);


#endif /* _LLRB_LINK_H_ */

