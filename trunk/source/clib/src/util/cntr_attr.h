#ifndef _CNTR_ATTR_H_
#define _CNTR_ATTR_H_

/*
 * This attribute serves as type information in oo, it determines what kind of operations could 
 * operate on an object.
 */

typedef unsigned int cntr_attr;

#define CNTR_ATTR_BASE       (1<<0)
#define CNTR_ATTR_LINEAR     (1<<1)
#define CNTR_ATTR_ARRAY      (1<<2)
#define CNTR_ATTR_LIST       (1<<3)

#endif /* _CNTR_ATTR_H_ */
