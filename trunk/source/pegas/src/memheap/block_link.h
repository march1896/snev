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
struct listlink;
int block_data_size_from_blick(struct listlink *link);

struct listlink *blink_inc_find_ff (struct listlink *phead, int req);
struct listlink *blink_inc_find_bf (struct listlink *phead, int req);
void   blink_inc_pop  (struct listlink *psent, struct listlink *pbd);
void   blink_inc_push (struct listlink *psent, struct listlink *pbd);

struct listlink *blink_rnd_find_ff (struct listlink *phead, int req);
struct listlink *blink_rnd_find_bf (struct listlink *phead, int req);
void   blink_rnd_pop  (struct listlink *psent, struct listlink *pbd);
void   blink_rnd_push (struct listlink *psent, struct listlink *pbd);

#define blink_find(ph, reg) blink_rnd_find_ff((ph), reg)
#define blink_push(ph, pbd) blink_rnd_push((ph), (pbd))
#define blink_pop(ph, pbd)  blink_rnd_pop((ph), (pbd))

#endif
