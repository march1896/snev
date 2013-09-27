#ifndef _SPLAY_LINK_H_
#define _SPLAY_LINK_H_

#include <cominc.h>

struct splay_link {
	struct splay_link*  left;
	struct splay_link*  right;
	struct splay_link*  parent;
};

typedef int (*pf_splay_compare)  (const struct splay_link* l, const struct splay_link* r);
typedef int (*pf_splay_compare_v)(const struct splay_link* l, const struct splay_link* r, void* param);
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
struct splay_link* splay_insert(struct splay_link* root, struct splay_link* target, pf_splay_compare comp);

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
struct splay_link* splay_insert_s(struct splay_link* root, struct splay_link* target, pf_splay_compare comp, struct splay_link** dup);

/**
 * @brief remove a link from a tree.
 * 		if target is not in the tree, the behavior is not defined.
 * 		if target is in the tree, it will be removed.
 *
 * @param root the original tree root.
 * @param target the link to be removed.
 * @param comp tree compare function.
 *
 * @return the new tree root after remove, NULL if no element in the tree.
 */
struct splay_link* splay_remove(struct splay_link* root, struct splay_link* target, pf_splay_compare comp);

/* the verbose version of insert/remove */
struct splay_link* splay_insert_v(struct splay_link* root, struct splay_link* target, pf_splay_compare_v comp, void* param);
struct splay_link* splay_insert_sv(struct splay_link* root, struct splay_link* target, pf_splay_compare_v comp, void* param, struct splay_link** dup);
struct splay_link* splay_remove_v(struct splay_link* root, struct splay_link* target, pf_splay_compare_v comp, void* param);

/**
 * @brief splay_search callback, given the current link, this function will decide which child will be searched next.
 * 		if return value < 0, the left child tree will be searched.
 * 		if return value == 0, the current link will be returned.
 * 		if return value > 0, the right child tree will be searched.
 *
 * @param the current tree link.
 * @param param custom parameter which is used for deciding which side to search next.
 *
 * @return see @brief
 */
typedef int (*pf_splay_direct)(const struct splay_link* cur, void* param);

struct splay_link* splay_search(struct splay_link* root, pf_splay_direct direct, void* param);
struct splay_link* splay_dynamic_search(struct splay_link** p_root, pf_splay_direct direct, void* param);

struct splay_link* splay_min        (struct splay_link* root);
struct splay_link* splay_max        (struct splay_link* root);
struct splay_link* splay_predesessor(const struct splay_link* link, bool only_sub);
struct splay_link* splay_successor  (const struct splay_link* link, bool only_sub);

/* TODO: why this interface is here */
void              splay_swap_link  (struct splay_link** pa, struct splay_link** pb);
void              splay_debug_check(struct splay_link* root, pf_splay_compare comp);
void              splay_debug_check_v(struct splay_link* root, pf_splay_compare_v comp_v, void* param);


#endif /* _SPLAY_LINK_H_ */

