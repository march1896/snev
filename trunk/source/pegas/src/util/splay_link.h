#ifndef _SPLAY_LINK_H_
#define _SPLAY_LINK_H_

#include <util/cominc.h>

struct splaylink {
	struct splaylink*  left;
	struct splaylink*  right;
	struct splaylink*  parent;
};

typedef int (*pf_splay_compare)  (const struct splaylink* l, const struct splaylink* r);
typedef int (*pf_splay_compare_v)(const struct splaylink* l, const struct splaylink* r, void* param);
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
struct splaylink* splay_insert(struct splaylink* root, struct splaylink* target, pf_splay_compare comp);

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
struct splaylink* splay_insert_s(struct splaylink* root, struct splaylink* target, pf_splay_compare comp, struct splaylink** dup);

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
struct splaylink* splay_remove(struct splaylink* root, struct splaylink* target, pf_splay_compare comp);

/* the verbose version of insert/remove */
struct splaylink* splay_insert_v(struct splaylink* root, struct splaylink* target, pf_splay_compare_v comp, void* param);
struct splaylink* splay_insert_sv(struct splaylink* root, struct splaylink* target, pf_splay_compare_v comp, void* param, struct splaylink** dup);
struct splaylink* splay_remove_v(struct splaylink* root, struct splaylink* target, pf_splay_compare_v comp, void* param);

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
typedef int (*pf_splay_direct)(const struct splaylink* cur, void* param);

struct splaylink* splay_search(struct splaylink* root, pf_splay_direct direct, void* param);
struct splaylink* splay_dynamic_search(struct splaylink** p_root, pf_splay_direct direct, void* param);

struct splaylink* splay_min        (struct splaylink* root);
struct splaylink* splay_max        (struct splaylink* root);
struct splaylink* splay_predesessor(const struct splaylink* link, bool only_sub);
struct splaylink* splay_successor  (const struct splaylink* link, bool only_sub);

void              splay_debug_check(struct splaylink* root, pf_splay_compare comp);
void              splay_debug_check_v(struct splaylink* root, pf_splay_compare_v comp_v, void* param);


#endif /* _SPLAY_LINK_H_ */

