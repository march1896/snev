#include "reg_def.h"

#ifdef _ENABLE_PRINT_
#include "stdio.h"
#endif 

p_node findnode_via_id( p_nodelist pnl, int node_id );
p_node findnode_via_info( p_nodelist pnl, int node_info );

p_nfa nfa_new() {
	p_nfa pa;

	pa  = (p_nfa)t_alloc( sizeof( s_nfa ) );
	pa->pnl_f = pa->pnl_l = NULL;

	return pa;
}

void nfa_del( p_nfa pa ) {
	p_nodelist pnl, pnl_prev;
	p_node pn;

	if ( pa == NULL ) return;

	pnl = pa->pnl_f;
	while ( pnl != NULL ) {
		pn = pnl->element;
		node_del( pn );

		pnl_prev = pnl;
		pnl = pnl->next;

		t_free( pnl_prev );
	}

	t_free( pa );
	return;
}

void nfa_addnode( p_nfa pa, p_node pn ) {
	p_nodelist pnl;

	pnl = (p_nodelist)t_alloc( sizeof( s_nodelist ) );
	pnl->element = pn;
	pnl->next = NULL;

	if ( pa->pnl_f == NULL ) {
		pa->pnl_f = pa->pnl_l = pnl;
	}
	else {
		pa->pnl_l->next = pnl;
		pa->pnl_l = pnl;
	}
}

/* it's a hard work for me to do the copy work, becasue all the memory are 
   alloced from the heap, so during the copy I have to alloc all the nodes and edges
   and maintain the pointers between them, this function may seems complicated
   and a little less of efficiency  */
p_nfa nfa_copy( p_nfa source ) {
	p_nfa dest;
	p_nodelist pnl, pnl_new;
	p_node pn, pn_new;
	p_edgelist pel, pel_new;
	p_edge pe, pe_new;
	int node_id;
	
	dest = nfa_new();

	/* first, copy all the nodes, ignore the edges */
	pnl = source->pnl_f;
	while ( pnl != NULL ) {
		pn = pnl->element;

		pn_new = node_new( pn->id );
		pn_new->info = pn->info;
		nfa_addnode( dest, pn_new );

		pnl = pnl->next;
	}

	/* second, init the edges */
	pnl = source->pnl_f;
	pnl_new = dest->pnl_f;
	while ( pnl != NULL ) {
		/* for each node in nodelist */
		pel = pnl->element->pel_f;
		while ( pel != NULL ) {
			/* for each edge in edgelist */
			pe = pel->element;
			node_id = pe->dest->id;
			pn = findnode_via_id( dest->pnl_f, node_id );

			pe_new = edge_new( pe->weight, pn );

			node_addedge( pnl_new->element, pe_new );

			pel = pel->next;
		}
		pnl = pnl->next;
		pnl_new = pnl_new->next;
	}

	return dest;
}

p_node findnode_via_id( p_nodelist pnl, int node_id ) {
	while ( pnl != NULL ) {
		if ( pnl->element->id == node_id ) {
			return pnl->element;
		}
		pnl = pnl->next;
	}
	return NULL;
}

p_nfa nfa_concat( p_nfa first, p_nfa second ) { 
 	p_nfa res;
	p_nfa pa_first, pa_second;
	p_node pn_second_start, pn_first_accept;
	p_edge pe;

	pa_first = nfa_copy( first );
	pa_second = nfa_copy( second );

	pn_first_accept = findnode_via_info( pa_first->pnl_f, NODE_ACCEPT );
	pn_second_start = findnode_via_info( pa_second->pnl_f, NODE_START );
	pn_first_accept->info = NODE_NORMAL;
	pn_second_start->info = NODE_NORMAL;
	
	pe = edge_new( EPSILON, pn_second_start );
	node_addedge( pn_first_accept, pe );

	pa_first->pnl_l->next = pa_second->pnl_f;

	res = nfa_new();
	res->pnl_f = pa_first->pnl_f;
	res->pnl_l = pa_second->pnl_l;

	t_free( pa_first );
	t_free( pa_second );

	return res;
}

p_node findnode_via_info( p_nodelist pnl, int node_info ) {
	while ( pnl != NULL ) {
		if ( pnl->element->info == node_info ) {
			return pnl->element;
		}
		pnl = pnl->next;
	}
	return NULL;
}

p_nfa nfa_branch( p_nfa first, p_nfa second ) {
 	p_nfa res;
	p_nfa pa_first, pa_second;
	p_node pn_second_start, pn_first_accept;
	p_node pn_second_accept, pn_first_start;
	p_node pn;
	p_edge pe;

	/* copy the nfas */
	pa_first = nfa_copy( first );
	pa_second = nfa_copy( second );
	
	/* find the start and accept nodes in first nfa, and second nfa */
	pn_first_start = findnode_via_info( pa_first->pnl_f, NODE_START );
	pn_first_accept = findnode_via_info( pa_first->pnl_f, NODE_ACCEPT );
	pn_second_start = findnode_via_info( pa_second->pnl_f, NODE_START );
	pn_second_accept = findnode_via_info( pa_second->pnl_f, NODE_ACCEPT );
	pn_first_start->info = pn_first_accept->info = NODE_NORMAL;
	pn_second_start->info = pn_second_accept->info = NODE_NORMAL;

	res = nfa_new();

	/* make EPSILON edge from the new node to the startnodes of the two */
	pn = node_new( node_make_id() );
	pn->info = NODE_START;
	pe = edge_new( EPSILON, pn_first_start );
	node_addedge( pn, pe );
	pe = edge_new( EPSILON, pn_second_start );
	node_addedge( pn, pe );
	nfa_addnode( res, pn );

	pn = node_new( node_make_id() );
	pn->info = NODE_ACCEPT;
	pe = edge_new( EPSILON, pn );
	node_addedge( pn_first_accept, pe );
	pe = edge_new( EPSILON, pn );
	node_addedge( pn_second_accept, pe );
	nfa_addnode( res, pn );

	pa_first->pnl_l->next = pa_second->pnl_f;

	res->pnl_l->next = pa_first->pnl_f;
	res->pnl_l = pa_second->pnl_l;

	/* just release the nfa but not the content */
	t_free( pa_first );
	t_free( pa_second );

	return res;
}

p_nfa nfa_closure( p_nfa source ) {
 	p_nfa res;
	p_nfa pa_copy;
	p_node pn_start, pn_accept;
	p_node pn_x, pn_y;
	p_edge pe;

	/* copy the nfas */
	pa_copy = nfa_copy( source );
	
	/* find the start and accept nodes in first nfa, and second nfa */
	pn_start = findnode_via_info( pa_copy->pnl_f, NODE_START );
	pn_accept = findnode_via_info( pa_copy->pnl_f, NODE_ACCEPT );
	pn_start->info = pn_accept->info = NODE_NORMAL;

	pe = edge_new( EPSILON, pn_start );
	node_addedge( pn_accept, pe );

	res = nfa_new();
	/* make EPSILON edge from the new node to the startnodes of the two */
	pn_x = node_new( node_make_id() );
	pn_x->info = NODE_START;
	pe = edge_new( EPSILON, pn_start );
	node_addedge( pn_x, pe );
	nfa_addnode( res, pn_x );

	pn_y = node_new( node_make_id() );
	pn_y->info = NODE_ACCEPT;
	pe = edge_new( EPSILON, pn_y );
	node_addedge( pn_accept, pe );
	nfa_addnode( res, pn_y );

	pe = edge_new( EPSILON, pn_y );
	node_addedge( pn_x, pe );

	res->pnl_l->next = pa_copy->pnl_f;
	res->pnl_l = pa_copy->pnl_l;

	/* just release the nfa but not the content */
	t_free( pa_copy );

	return res;
}

p_nfa nfa_make_from_stringconcat( const char* str ) {
	p_nfa pa;
	p_node pn_prev, pn_next;
	p_edge pe;

	if ( str == NULL ) return NULL;

	printf( "make from string concat\n" );
	pa = nfa_new();
	pn_prev = node_new( node_make_id() );
	pn_prev->info = NODE_START;
	nfa_addnode( pa, pn_prev );

	if ( (*str) == 0 ) {
		/* empty string indicate epsilon */
		printf( "Only Epsilon\n" );
		pn_next = node_new( node_make_id() );
		pn_next->info = NODE_ACCEPT;
		pe = edge_new( EPSILON, pn_next );
		node_addedge( pn_prev, pe );
		nfa_addnode( pa, pn_next );

		return pa;
	}

	while ( (*str) != 0 ) {
		pn_next = node_new( node_make_id() );
		pe = edge_new( (int)(*str), pn_next );
		node_addedge( pn_prev, pe );
		nfa_addnode( pa, pn_next );

		pn_prev = pn_next;
		str ++;
	}

	pn_prev->info = NODE_ACCEPT;
	return pa;
}

p_nfa nfa_make_from_stringbranch( const char* str ) {
	p_nfa pa;
	p_node pn_prev, pn_next;
	p_edge pe;

	if ( str == NULL ) return NULL;
	
	pa = nfa_new();
	pn_prev = node_new( node_make_id() );
	pn_prev->info = NODE_START;
	pn_next = node_new( node_make_id() );
	pn_next->info = NODE_ACCEPT;

	nfa_addnode( pa, pn_prev );
	nfa_addnode( pa, pn_next );

	if ( (*str) == 0 ) {
		/* empty string indicate epsilon */
		printf( "Only Epsilon\n" );
		pe = edge_new( EPSILON, pn_next );
		node_addedge( pn_prev, pe );

		return pa;
	}

	while ( (*str) != 0 ) {
		pe = edge_new( (int)(*str), pn_next );
		node_addedge( pn_prev, pe );

		str ++;
	}

	return pa;
}

#ifdef _ENABLE_PRINT_
void nfa_print( p_nfa pa ) {
	p_nodelist pnl;
	p_node pn;
	p_edgelist pel;
	p_edge pe;

	pnl = pa->pnl_f;
	while ( pnl != NULL ) {
		pn = pnl->element;
		printf( "node: %d %d\n", pn->id, pn->info );
		pel = pn->pel_f;

		while ( pel != NULL ) {
			pe = pel->element;
			
			printf( "    edge: %c %d\n", (char)pe->weight, pe->dest->id );
			pel = pel->next;
		}

		pnl = pnl->next;
	}

	return;
}
#endif
