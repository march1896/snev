#ifndef _CNTR_ARRAY_H_
#define _CNTR_ARRAY_H_

/*
 * This file defines st::vector like dynamic array container 
 */

#include <cntr_iterator.h>
#define default_init_capacity 64
#define default_expand_size 64

typedef unknown carray;

carray carray_create();
carray carray_create_v(int init_capacity, int expand_size);

void  carray_destroy    (carray ca);
void  carray_clear      (carray ca);
int   carray_size       (carray ca);
void  carray_add        (carray ca, void* object);
void  carray_remove     (carray ca, citer begin, citer end);
bool  carray_find       (carray ca, void* object, citer itr);
void  carray_citer_begin(carray ca, citer itr);
void  carray_citer_end  (carray ca, citer itr);

void* carray_front      (carray ca);
void* carray_back       (carray ca);
void  carray_add_front  (carray ca, void* object);
void  carray_add_back   (carray ca, void* object);
void* carray_remove_front(carray ca);
void* carray_remove_back (carray ca);


/**
 * @brief 
 *
 * @param pca
 * @param n_size
 */
void carray_assign_capacity(carray* pca, int n_size);

#endif /* _CNTR_ARRAY_H_ */
