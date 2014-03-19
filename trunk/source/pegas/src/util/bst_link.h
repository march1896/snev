#ifndef _COMMOM_BINARY_TREE_LINK_H_
#define _COMMOM_BINARY_TREE_LINK_H_

struct bst_link {
	struct bst_link* left;    /* point to the left child, NULL if no left child */
	struct bst_link* right;   /* point to the right child, NULL if no right child */
};

/* 
 * the below functions serve as base interfaces that could be reused by other 
 * trees which expand the base binary search tree to some more powerful trees,
 * such as red-black tree, AVL tree or splay tree
 */
extern inline struct bst_link* bst_min(struct bst_link* root);
extern inline struct bst_link* bst_max(struct bst_link* root);

extern inline struct bst_link* bst_sub_predecessor(struct bst_link* link);
extern inline struct bst_link* bst_sub_successor(struct bst_link* link);

extern inline struct bst_link* bst_rotate_left(struct bst_link* link);
extern inline struct bst_link* bst_rotate_right(struct bst_link* link);

/*
 * the below functions are the basic interfaces that a tree should offer. insert/search/remove.
 * alghough we usually dont use the bst directly, because of performance problem.
 */

/* return -1, if lhs < rhs
 * return 0 , if lhs = rhs
 * return 1 , if lhs > rhs */
typedef int (*pf_bst_compare)(const struct bst_link* lhs, const struct bst_link* rhs);
typedef int (*pf_bst_compare_v)(const struct bst_link* lhs, const struct bst_link* rhs, void* param);

/* return -1, the left subtree will be searched.
 * return 0 , the current link will be returned.
 * return 1 , the right subtree will be searched. */
typedef int (*pf_bst_direct) (const struct bst_link* link, void* param);

struct bst_link* bst_insert  (struct bst_link* root, struct bst_link* n_link, pf_bst_compare comp);
struct bst_link* bst_search  (struct bst_link* root, void*  param,            pf_bst_direct dir);
struct bst_link* bst_remove  (struct bst_link* root, struct bst_link* n_link, pf_bst_compare comp);

/* implement it on requirements, is this a right development method */
struct bst_link* bst_insert_v(struct bst_link* root, struct bst_link* n_link, pf_bst_compare_v comp, void* param);
struct bst_link* bst_remove_v(struct bst_link* root, struct bst_link* n_link, pf_bst_compare_v comp, void* param);

struct bstp_link {
	struct bstp_link* left;
	struct bstp_link* right;

	struct bstp_link* parent;  /* point to parent, NULL if current is root */
};

extern inline struct bstp_link* bstp_predecessor(struct bstp_link* root);
extern inline struct bstp_link* bstp_successor(struct bstp_link* root);

extern inline struct bstp_link* bstp_rotate_left(struct bstp_link* link);
extern inline struct bstp_link* bstp_rotate_right(struct bstp_link* link);

#endif /* _COMMOM_BINARY_TREE_LINK_H_ */
