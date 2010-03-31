#include "reg_def.h"
#include "heap2.h"
#include "stdio.h"
#include "assert.h"

static int g_nfa_id_index = 0;

void idlist_addid( p_idlist pil, int id ) {
	assert( pil != NULL && id < ID_SENTINEL );

	p_idlist prev, next;

	p_idlist pil_new;
	pil_new = (p_idlist)t_alloc( sizeof( s_idlist ) );
	pil_new->id = id;

	prev = pil;
	next = pil->next;
	while ( prev != NULL ) {
		if ( next == NULL ) {
			prev->next = pil_new;
			break;
		}
		else if ( next->id == id ) {
			/* return if this id is already in the list */
			break;
		}
		else if ( next->id < id ) {
			pil_new->next = next;
			prev->next = pil_new;
			break;
		}

		prev = next;
		next = next->next;
	}

	return;
}

p_idlist idlist_new() {
	p_idlist pil;
	
	pil = (p_idlist)t_alloc( sizeof( s_idlist ) );
	pil->id = ID_SENTINEL;
	pil->next = NULL;

	return pil;
}

p_idlist idlist_copy( p_idlist pil ) {
	p_idlist ret;
	p_idlist prev, pil_new; // iterator

	if ( pil == NULL ) return NULL;
	assert( pil->id == ID_SENTINEL );

	ret = (p_idlist)t_alloc( sizeof( s_idlist ) );
	ret->id = ID_SENTINEL;
	ret->next = NULL;

	prev = ret;
	pil = pil->next;

	while ( pil != NULL ) {
		pil_new = (p_idlist)t_alloc( sizeof( s_idlist ) );
		pil_new->id = pil->id;
		pil_new->next = NULL;

		prev->next = pil_new;
		prev = pil_new;

		pil = pil->next;
	}

	return ret;
}

void idlist_del( p_idlist pil ) {
	p_idlist prev;

	while ( pil != NULL ) {
		prev = pil;
		pil = pil->next;

		t_free( prev );
	}

	return;
}

p_idmap idmap_new() {
	p_idmap pim;

	pim = (p_idmap)t_alloc( sizeof( s_idmap ) );
	pim->dfa_id = ID_SENTINEL;
	pim->nfa_idlist = NULL;
	pim->next = NULL;

	return pim;
}

void idmap_del( p_idmap pim ) {
	p_idmap pim_itr, pim_prev;

	pim_itr = pim;
	while ( pim_itr != NULL ) {
		pim_prev = pim_itr;
		
		idlist_del( pim_itr->nfa_idlist );
		t_free( pim_prev );

		pim_itr = pim_itr->next;
	}

	return;
}

static int make_unique_dfaid();
int idmap_add( p_idmap pim, p_idlist pil ) {
	p_idmap pim_itr;
	p_idlist pil_copy;
	int id;

	id = idmap_find( pim, pil_copy );
	if ( id != ID_NOTEXSIT ) return id;
	
	pim_itr = (p_idmap)t_alloc( sizeof( s_idmap ) );
	id = pim_itr->dfa_id = make_unique_dfaid();
	pim_itr->nfa_idlist = pil_copy;

	pim_itr->next = pim->next;
	pim->next = pim_itr;

	return id;
}

int make_unique_dfaid() {
	return g_nfa_id_index ++;
}

int idlist_compare( p_idlist first, p_idlist second ) {
	if ( first == NULL ) return -1;

	while ( first != NULL ) {
		if ( second == NULL ) return 1;
		else if ( first->id < second->id ) return -1;
		else if ( first->id > second->id ) return 1;
		
		first = first->next;
		second = second->next;
	}

	if ( second == NULL ) return 0;
	else return -1;
}

int idmap_find( p_idmap pim, p_idlist pil ) {
	p_idmap pim_itr;
	p_idlist pil_itr;

	if ( pim == NULL ) return ID_NOTEXSIT;

	pim_itr = pim->next;
	while ( pim_itr != NULL ) {
		pil_itr = pim_itr->nfa_idlist;

		if ( idlist_compare( pil_itr, pil ) == 0 ) {
			return pim_itr->dfa_id;
		}

		pim_itr = pim_itr->next;
	}
	
	return ID_NOTEXSIT;
}

