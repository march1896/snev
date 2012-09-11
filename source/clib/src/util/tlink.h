#ifndef _RED_BLACK_TREE_LINK_H_
#define _RED_BLACK_TREE_LINK_H_

#include <cominc.h>

/* this is just a copy of 234 tree algorithm, using red-black color representation, 
 * and it does not maintain any type of data, nor the alloc/free of a node, the duty
 * is transferred to the client who is using it */

struct tlink {
	struct tlink *left;
	struct tlink *right;
	struct tlink *parent;

	unsigned     color;
};

/**
 * @brief 
 *
 * @param r
 * @param n
 *
 * @return 
 */
struct tlink *tlink_insert(struct tlink *r, struct tlink *n);

/**
 * @brief 
 *
 * @param r
 * @param n
 *
 * @return 
 */
struct tlink *tlink_remove(struct tlink *r, struct tlink *n);

/**
 * @brief 
 *
 * @param r
 * @param n
 *
 * @return 
 */
struct tlink *tlink_search(struct tlink *r, struct tlink *n);

void   tlink_init(struct tlink *n);

typedef bool (*pf_tlink_compare)(const struct tlink *l, const struct tlink *r);
void tlink_set_compare(pf_tlink_compare pc);


#endif /* _RED_BLACK_TREE_LINK_H_ */
