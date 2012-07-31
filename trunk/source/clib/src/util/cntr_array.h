#ifndef _CNTR_ARRAY_H_
#define _CNTR_ARRAY_H_

/*
 * This file defines st::vector like dynamic array container 
 */

#include <cntr_iterator.h>

typedef struct cntr_array_t {
	int size;
	int capacity;
	unsigned flags;

	void* data;
} carray;

/*
 * linear container specific interface.
 */

void carray_init(carray* pca);

void carray_deinit(carray* pca);

void carray_clean(carray* pca);

int carray_size(carray* pca);

void* carray_front(carray* pcl);

void* carray_back (carray* pcl);

void  carray_add_front(carray* pcl, void* object);

void  carray_add_back (carray* pcl, void* object);

void* carray_remove_front(carray* pcl);

void* carray_remove_back (carray* pcl);

struct citer;
void  carray_citer_begin(carray* pcl, citer* itr);

void  carray_citer_end  (carray* pcl, citer* itr);
/* linear container specific interface end.  */


void  carray_add      (carray* pcl, void* object);
/**
 * @brief 
 *
 * @param pca
 * @param n_size
 */
void carray_assign_capacity(carray* pca, int n_size);

#endif /* _CNTR_ARRAY_H_ */
