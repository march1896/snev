#include "reg_def.h"
#include "heap2.h"
#include "stdio.h"
#include "assert.h"

static int g_dfa_id_index = 0;
p_ndmap ndmap_new() {
	p_ndmap pim;

	pim = (p_ndmap)t_alloc( sizeof( s_ndmap ) );
	pim->pf = NULL;
	pim->pl = NULL;

	return pim;
}

void nodelist_del( p_nodelist pnl );
/* the map only delete the nodelist and itself, but not the nodes */
void ndmap_del( p_ndmap pim ) {
	p_ndmap_ele pme_itr, pme_prev;
	p_nodelist pnl_itr, pnl_prev;

	pme_itr = pim->pf;
	while ( pme_itr != NULL ) {
		pnl_itr = pme_itr->nfa_pnl;

		/*
		while ( pnl_itr != NULL ) {
			pnl_prev = pnl_itr;
			pnl_itr = pnl_itr->next;
			t_free( pnl_prev );
		}*/
		nodelist_del( pnl_itr );

		pme_prev = pme_itr;
		pme_itr = pme_itr->next;
		t_free( pme_prev );
	}
	
	t_free( pim );

	return;
}

static int make_unique_dfaid();
static p_nodelist nodelist_copy( p_nodelist pnl );
void ndmap_add( p_ndmap pim, p_node pn, p_nodelist pnl ) {
	p_ndmap_ele pme_itr;

	if ( pim == NULL ) return ;

	pme_itr = (p_ndmap_ele)t_alloc( sizeof( s_ndmap_ele ) );
	pme_itr->dfa_pn = pn;
	pme_itr->nfa_pnl = nodelist_copy( pnl );
	pme_itr->next = NULL;

	if ( pim->pf == NULL ) {
		pim->pf = pim->pl = pme_itr;
	}
	else {
		pim->pl->next = pme_itr;
		pim->pl = pme_itr;
	}

	return ;
}

int make_unique_dfaid() {
	return g_dfa_id_index ++;
}

int nodelist_compare( p_nodelist first, p_nodelist second ) {
	if ( first == NULL ) return -1;

	while ( first != NULL ) {
		if ( second == NULL ) return 1;
		else if ( first->element->id < second->element->id ) return -1;
		else if ( first->element->id > second->element->id ) return 1;
		
		first = first->next;
		second = second->next;
	}

	if ( second == NULL ) return 0;
	else return -1;
}

p_nodelist ndmap_find_via_dfanode( p_ndmap pim, p_node pn ) {
	p_ndmap_ele pme;
	if ( pim == NULL ) return NULL;

	pme = pim->pf;
	while ( pme != NULL ) {
		if ( pme->dfa_pn->id == pn->id ) 
			return nodelist_copy( pme->nfa_pnl ); 
		pme = pme->next;
	}

	return NULL;
}

p_nodelist nodelist_copy( p_nodelist pnl ) {
	p_nodelist ret, itr, itr_new;
	
	if ( pnl == NULL ) return NULL;
	ret = (p_nodelist)t_alloc( sizeof( s_nodelist ) );
	ret->element = pnl->element;
	ret->next = NULL;
	itr = ret;

	pnl = pnl->next;
	while ( pnl != NULL ) {
		itr_new = (p_nodelist)t_alloc( sizeof( s_nodelist ) );
		itr_new->element = pnl->element;
		itr_new->next = NULL;

		itr->next = itr_new;
		itr = itr_new;

		pnl = pnl->next;
	}

	return ret;
}

void nodelist_del( p_nodelist pnl ) {
	p_nodelist prev;

	while ( pnl != NULL ) {
		prev = pnl;
		pnl = pnl->next;

		t_free( prev );
	}

	return;
}

p_node ndmap_find_via_nfanodelist( p_ndmap pim, p_nodelist pnl ) {
	p_ndmap_ele pme;
	if ( pim == NULL ) return NULL;

	pme = pim->pf;
	while ( pme != NULL ) {
		if ( nodelist_compare( pme->nfa_pnl, pnl ) == 0 )
			return pme->dfa_pn;
		pme = pme->next;
	}

	return NULL;
}

static p_nodelist node_epsilon_closure( p_node pn );
static p_nodelist nodelist_epsilon_closure( p_nodelist pnl );

p_nodelist nodelist_find_edge( p_nodelist pnl, int w );
#define WT_SIZE 256
p_dfa dfa_convert_from_nfa( p_nfa pna ) {
	int i;
	p_dfa pda;
	p_ndmap pim;
	p_nodelist pnl, pnl_first, pnl_last, pnl_x, pnl_y, pnl_z, pnl_new;
	p_node pn, pn_new, pn_nfa_start, pn_nfa_accept, pn_x;
	p_edge pe_x;
	int dfa_id;
	int *w_table; // weight table

	if ( pna == NULL ) return NULL;

	pim = ndmap_new();
	pda = dfa_new();

	/* first find the start and accept node in nfa */
	pnl = pna->pnl_f;
	pn_nfa_start = pn_nfa_accept = NULL;
	while ( pnl != NULL ) {
		if ( pnl->element->info == NODE_START ) {
			pn_nfa_start = pnl->element;
		}
		else if ( pnl->element->info == NODE_ACCEPT ) {
			pn_nfa_accept = pnl->element;
		}
		pnl = pnl->next;
	}

	pnl = node_epsilon_closure( pn_nfa_start );
	dfa_id = make_unique_dfaid();

	/* add the start node to dfa */
	pn_new = node_new( dfa_id );
	pn_new->info = NODE_START;
	pnl_z = pnl;
	while ( pnl_z != NULL ) {
		if ( pnl_z->element->info == NODE_ACCEPT ) {
			pn_new->info |= NODE_ACCEPT;
		}
		pnl_z = pnl_z->next;
	}
	dfa_addnode( pda, pn_new );

	ndmap_add( pim, pn_new, pnl );
	nodelist_del( pnl );
	/* now pnl_f should be associated with a right value */
	pnl_first = pnl_last = pda->pnl_f;


	while ( pnl_first != NULL ) {
		pn = pnl_first->element;
	
		pnl = ndmap_find_via_dfanode( pim, pn );

		//nodelist_print( pnl );

		for (i = 0; i < WT_SIZE; i++ ) {
			pnl_y = nodelist_find_edge( pnl, i );

			/* there exsit a i'weighted edge */
			if ( pnl_y != NULL ) {
				pnl_x = nodelist_epsilon_closure( pnl_y );
				nodelist_del( pnl_y );

				pn_x = ndmap_find_via_nfanodelist( pim, pnl_x );
				if ( pn_x == NULL ) {
					// find a new node
					pn_x = node_new( make_unique_dfaid() );
					ndmap_add( pim, pn_x, pnl_x );

					pnl_z = pnl_x;
					while ( pnl_z != NULL ) {
						if ( pnl_z->element->info == NODE_ACCEPT ) {
							pn_x->info |= NODE_ACCEPT;
						}
						pnl_z = pnl_z->next;
					}

					pnl_new = (p_nodelist)t_alloc( sizeof( s_nodelist) );
					pnl_new->element = pn_x;
					pnl_new->next = NULL;

					pnl_last->next = pnl_new;
					pnl_last = pnl_new;
				}

				pe_x = edge_new( i, pn_x );
				node_addedge( pn, pe_x );

				nodelist_del( pnl_x );
			}
		}

		nodelist_del( pnl );

		pnl_first = pnl_first->next;
	}

	ndmap_del( pim );

	return pda;
}

p_nodelist nodelist_find_edge( p_nodelist pnl, int w ) {
	p_nodelist ret, pnl_new, last;
	p_node pn;
	p_edgelist pel;
	p_edge pe;

	if ( pnl == NULL ) return NULL;

	ret = last = NULL;

	while ( pnl != NULL ) {
		pn = pnl->element;
		pel = pn->pel_f;

		while ( pel != NULL ) {
			pe = pel->element;

			if ( pe->weight == w ) {
				/* find a edge */
				//printf( "%c %d\n", pe->weight, pe->dest->id );

				/* check if the node is already in list */
				pnl_new = ret;
				while ( pnl_new != NULL ) {
					if ( pnl_new->element == pe->dest ) { 
						break;
					}
					pnl_new = pnl_new->next;
				}

				/* find a new node, push into list */
				if ( pnl_new == NULL ) {
					pnl_new = (p_nodelist)t_alloc( sizeof( s_nodelist ) );
					pnl_new->element = pe->dest;
					pnl_new->next = NULL;

					if ( ret == NULL ) {
						ret = last = pnl_new;
					}
					else {
						last->next = pnl_new;
						last = pnl_new;
					}
				}
			}
			pel = pel->next;
		}
		pnl = pnl->next;
	}

	return ret;
}

p_nodelist node_epsilon_closure( p_node pn ) {
	p_nodelist ret, temp;

	if ( pn == NULL ) return NULL;

	temp = (p_nodelist)t_alloc( sizeof( s_nodelist ) );
	temp->element = pn;
	temp->next = NULL;

	ret = nodelist_epsilon_closure( temp );
	nodelist_del( temp );

	return ret;
}

p_nodelist nodelist_epsilon_closure( p_nodelist pnl ) {
	p_nodelist ret, first, last, pnl_x;
	p_node pn, pn_x;
	p_edgelist pel;
	p_edge pe;

	if ( pnl == NULL ) return NULL;

	ret = nodelist_copy( pnl );
	first = last = ret;

	while ( last->next != NULL ) last = last->next;

	while ( first != NULL ) {
		pn = first->element;
		pel = pn->pel_f;
	
		while ( pel != NULL ) {
			if ( pel->element->weight == EPSILON ) {
				pn_x = pel->element->dest;

				pnl_x = ret;
				while ( pnl_x != NULL ) {
					if ( pnl_x->element == pn_x ) 
						break;
					pnl_x = pnl_x->next; 
				}

				if ( pnl_x == NULL ) {
					/* find a new node */
					pnl_x = (p_nodelist)t_alloc( sizeof( s_nodelist ) );
					pnl_x->element = pn_x;
					pnl_x->next = NULL;

					last->next = pnl_x;
					last = pnl_x;
				}
			}

			pel = pel->next;
		}

		first = first->next;
	}

	return ret;
}

void dfa_print( p_dfa pa ) {
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

void dfa_print_table( p_dfa dfa ) {
}

void nodelist_print( p_nodelist pnl ) {
	printf( "nodelist: " );

	while ( pnl != NULL ) {
		printf( "%d ", pnl->element->id );
		pnl = pnl->next;
	}

	printf( "\n" );
	return;
}

void dfa_addnode( p_dfa pa, p_node pn ) {
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

p_dfa dfa_new() {
	p_dfa pa;

	pa  = (p_dfa)t_alloc( sizeof( s_dfa ) );
	pa->pnl_f = pa->pnl_l = NULL;

	return pa;
}

void dfa_del( p_dfa pa ) {
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
