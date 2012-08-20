#ifndef _234TREE_H_
#define _234TREE_H_

#include <cominc.h>

struct _234_node {
	int dim;
	struct _234_node *parent;
	union {
		struct {
			void *first;
			void *second;
			void *third;
		};
		void *keys[3];
	};
	union {
		struct {
			struct _234_node* left;
			struct _234_node* midleft;
			struct _234_node* midright;
			struct _234_node* right;
		};
		struct _234_node* children[4];
	};
};

/**
 * @brief find an element in 234 tree 
 *
 * @param n root of the tree
 * @param key key to find
 * @param index index of keys if found, meaningless if not found
 *
 * @return the node contains the key, NULL if not found
 */
struct _234_node *_234_node_find  (struct _234_node *n, void *key, int *index);

/**
 * @brief add an element into 234 tree
 *
 * @param n root of the tree
 * @param key new key to add
 *
 * @return the new root of the tree
 */
struct _234_node *_234_node_add   (struct _234_node *n, void *key);

/**
 * @brief delete an element from 234 tree
 *
 * @param n root of the tree
 * @param index index of the keys to delete
 *
 * @return the new root of the tree
 */
struct _234_node *_234_node_delete(struct _234_node *n, int index);

struct _234_node *_234_node_predecessor(struct _234_node *n, int index, int *n_index);

struct _234_node *_234_node_successor(struct _234_node *n, int index, int *n_index);

void _234_node_check(struct _234_node *root);

typedef int (*pf_compare_function)(const void *lhs, const void *rhs);
/**
 * @brief the tree use and static global compare function to compare all keys stored
 *  in the tree, make sure the compare function is set before perform a add/find/delete 
 *  operation
 *
 * @param pf_compare_function compare function that compares keys
 */
extern inline void _234_node_set_comp(pf_compare_function);

#endif /* _234TREE_H_ */
