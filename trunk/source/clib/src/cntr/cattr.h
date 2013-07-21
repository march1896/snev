#ifndef _CNTR_ATTR_H_
#define _CNTR_ATTR_H_

/*
 * This attribute serves as type information in oo, it determines what kind of operations could 
 * operate on an object.
 */

typedef unsigned int cattr;

#define CNTR_ATTR_BASE       (1<<0)
#define CNTR_ATTR_LINEAR     (1<<1)
#define CNTR_ATTR_ARRAY      (1<<2)
#define CNTR_ATTR_LIST       (1<<3)
#define CNTR_ATTR_TREE       (1<<4)

#define CITER_ATTR_BASE      (1<<0)
/* continuous */
#define CITER_ATTR_CONT      (1<<1)
#define CITER_ATTR_LINK      (1<<2)

#define CNTR_BASE_NO         0XF0FA28CE
#define CNTR_LINEAR_NO       0X629425DD
#define CNTR_TREE_NO         0X32C6840B
#define CNTR_HASH_NO         0X392A63EF

#endif /* _CNTR_ATTR_H_ */
