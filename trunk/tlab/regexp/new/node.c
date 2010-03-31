#include "reg_def.h"

static int g_node_index = 0;

p_edge edge_new( int w, p_node pdn ) {
	p_edge pe;

	pe = (p_edge)t_alloc( sizeof( s_edge ) );
	pe->weight = w;
	pe->dest = pdn;
	
	return pe;
}

void edge_del( p_edge pn ) {
	t_free( pn );
}

int  node_make_id() {
	return g_node_index ++;
}

p_node node_new( int id ) {
	p_node pn;

	pn  = (p_node)t_alloc( sizeof( s_node ) );
	pn->id = id;
	pn->info = NODE_NORMAL;
	pn->pel_f = pn->pel_l = NULL;

	return pn;
}

void node_del( p_node pn ) {
	p_edgelist pel, pel_prev;
	p_edge pe;

	if ( pn == NULL ) return;
	
	pel = pn->pel_f;
	while ( pel != NULL ) {
		pe = pel->element;
		edge_del( pe );

		pel_prev = pel;
		pel = pel->next;

		t_free( pel_prev );
	}

	t_free( pn );

	return;
}

void node_addedge( p_node pn, p_edge pe ) {
	p_edgelist pel;

	pel = (p_edgelist)t_alloc( sizeof( s_edgelist ) );
	pel->element = pe;
	pel->next = NULL;

	if ( pn->pel_f == NULL ) {
		pn->pel_f = pn->pel_l = pel;
	}
	else {
		pn->pel_l->next = pel;
		pn->pel_l = pel;
	}
}

p_edge node_findedge( p_node dest, p_node source ) {
	p_edgelist pel;

	pel = source->pel_f;
	while ( pel != NULL ) {
		if ( pel->element->dest == dest ) {
			return pel->element;
		}
		pel = pel->next;
	}

	return NULL;
}

