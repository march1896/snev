#ifndef _CNTR_TREE_H_
#define _CNTR_TREE_H_

/*
 * store multi instance in the tree if they equals
 */
#define SET_MULTI_INS           (1 << 0)

/*
 * store single instance in the tree if they equals,
 * when add a new object that equals to an exist object, 
 * replace it with the new object.
 * if this bit is not set, when equals, the newly added object 
 * will be ignored.
 */
#define SET_EQUAL_REPLACE       (1 << 1)

#endif /* _CNTR_TREE_H_ */
