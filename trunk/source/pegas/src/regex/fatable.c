#include "fa.h"
#include "fatable.h"
#include "stdio.h"

void dfa_to_table( const p_dfa pa, int** weight_list, int *weight_size, int** state_table, int *state_size ) {
	p_nodelist pnl;
	p_node pn;
	p_edgelist pel;
	p_edge pe;
	int weight_map[256];
	int i, j;

	for (i = 0; i < 256; i ++) weight_map[i] = 0;
	j = 1;
	*state_size = 0;

	pnl = pa->pnl_f;
	while (pnl != NULL) {
		pn = pnl->element;
		pel = pn->pel_f;
		while (pel != NULL) {
			pe = pel->element;
			weight_map[pe->weight] = 1; 
			pel = pel->next;
		}
		pnl = pnl->next;
		(*state_size) ++;
	}

	for (i = 0, j = 1; i < 256; i ++) {
		if (weight_map[i] == 1) weight_map[i] = j++;
	}

	*weight_size = j;
	*weight_list = (int*)t_alloc( sizeof(int)*(*weight_size) );
	(*weight_list)[0] = 0;
	for (i = 0; i < 256; i ++) {
		if (weight_map[i] > 0) {
			(*weight_list)[weight_map[i]] = i;
		}
	}

	*state_table = (int*)t_alloc( sizeof(int) * ((*state_size) * (*weight_size) ) );

	for (i = 0; i < (*state_size)*(*weight_size); i ++) (*state_table)[i] = 0;

	i = 0;
	pnl = pa->pnl_f;
	while (pnl != NULL) {
		pn = pnl->element;

		(*state_table)[i * (*weight_size) + 0] = pn->id;
		pel = pn->pel_f;
		while (pel != NULL) {
			pe = pel->element;
			(*state_table)[i * (*weight_size) + weight_map[pe->weight]] = pe->dest->id;
			pel = pel->next;
		}

		i++;
		pnl = pnl->next;
	}

	/* dump the id */
	/*
	id_map = (int*)t_alloc( sizeof(int)*state_size );
	i = 0;
	pnl = pa->pnl_f;
	while (pnl != NULL) {
		id_map[i++] = pnl->element->id;
		pnl = pnl->next;
	}

	t_free( id_map );
	*/
}

void table_print( const p_dfa pa ) {
	int *w, *st;
	int w_size, st_size;
	int i, j;

	dfa_to_table( pa, &w, &w_size, &st, &st_size );

	printf( "\n\\----------------print table formated------------------\n" );
	for (i = 0; i < w_size; i ++) {
		printf( "%4d", w[i] );
	}
	printf( "\n" );

	for (i = 0; i < st_size; i ++) {
		for (j = 0; j < w_size; j ++) {
			printf( "%4d", st[i * w_size +j] );
		}
		printf( "\n" );
	}
	printf( "\n\\-------------------end of print-----------------------\n" );

	t_free( w );
	t_free( st );
}
