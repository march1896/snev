#ifndef _BLOCK_LINK_H_
#define _BLOCK_LINK_H_

/* 
 * this file defines data structure and method of using 
 * double linked list(dll) to manage free block 
 */

/*
 * double linked list(dll) has several way to manage blocks, 
 * 1, sort free blocks in increasing order.
 * 3, keep free blocks in random order.
 *
 * for all of the managing method list above, we have two ways 
 * to find a proper block that can carry enough data.
 * 1, find free block first fit.
 * 2, find free block best fit.
 */
struct list_link;
int block_data_size_from_blick(struct list_link *link);

struct list_link *blink_inc_find_ff (struct list_link *phead, int req);
struct list_link *blink_inc_find_bf (struct list_link *phead, int req);
void   blink_inc_pop  (struct list_link **pphead, struct list_link *pbd);
void   blink_inc_push (struct list_link **pphead, struct list_link *pbd);

struct list_link *blink_rnd_find_ff (struct list_link *phead, int req);
struct list_link *blink_rnd_find_bf (struct list_link *phead, int req);
void   blink_rnd_pop  (struct list_link **pphead, struct list_link *pbd);
void   blink_rnd_push (struct list_link **pphead, struct list_link *pbd);

#define blink_find(ph, reg) blink_rnd_find_ff((ph), reg)
#define blink_push(pph, pbd) blink_rnd_push((pph), (pbd))
#define blink_pop(pph, pbd)  blink_rnd_pop((pph), (pbd))

#endif
