#ifndef _REG_DEF_
#define _REG_DEF_

#define _ENABLE_PRINT_

#include "heap2.h"
#define t_alloc Allocate
#define t_free Free

#define EPSILON 256
typedef struct __s_edge {
	int 			weight;
	struct __s_node* 	dest;
} s_edge, *p_edge;

#define NODE_NORMAL 0
#define NODE_START 1
#define NODE_ACCEPT 2
typedef struct __s_node {
	int 			id; 	// the identity of the node, it's a unique interger
	int 			info; 	// the infomation of the node, like it's START_NODE or ACCEPT_NODE
	struct __s_edgelist* 	pel_f; 	// pointer to the first node in the edgelist
	struct __s_edgelist* 	pel_l; 	// pointer to the last node in the edgelist
} s_node, *p_node;

/** 
	edgelist is a list maintain in a certain node, when we create a new edge and add it to a
	node, we should add it to the nodelist
 */
typedef struct __s_edgelist {
	struct __s_edge* 	element;
	struct __s_edgelist* 	next;
} s_edgelist, *p_edgelist;

/** 
	nodelist is a list maintain in a certain graph, when we create a new node and add it to a
	graph, we should add it to the nodelist
 */
typedef struct __s_nodelist {
	struct __s_node* 	element;
	struct __s_nodelist* 	next;
} s_nodelist, *p_nodelist;

typedef struct __s_nfa {
	struct __s_nodelist* 	pnl_f; 	// pointer to the fisrt node in nodelist
	struct __s_nodelist* 	pnl_l; 	// pointer to the last node in nodelist
} s_nfa, *p_nfa;

p_edge 	edge_new( int w, p_node pdn );
void 	edge_del( p_edge pn );

int  	node_make_id(); 		// return a unique id for node
p_node 	node_new( int node_id );
void 	node_del( p_node pn );
void 	node_addedge( p_node pn, p_edge pe );
p_edge 	node_findedge( p_node dest, p_node source );


p_nfa 	nfa_new();
p_nfa 	nfa_copy( p_nfa source );
void 	nfa_del( p_nfa pa );
void 	nfa_addnode( p_nfa pa, p_node pn );

/** concat two nfa to to dest */
p_nfa nfa_concat( p_nfa source, p_nfa second );
/** */
p_nfa nfa_branch( p_nfa first, p_nfa second );
/** */
p_nfa nfa_closure( p_nfa source );

p_nfa nfa_make_from_stringconcat( const char* str );

p_nfa nfa_make_from_stringbranch( const char* str );

#ifdef _ENABLE_PRINT_
void nfa_print( p_nfa pa );
#endif
#endif // _REG_DEF_
