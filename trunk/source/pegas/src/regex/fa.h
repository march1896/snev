#ifndef _REG_FA_
#define _REG_FA_

#define _ENABLE_PRINT_

#include <util/cominc.h>
#include <memheap/heap_global.h>

#define t_alloc malloc
#define t_free free

#define EPSILON 256
typedef struct __s_edge {
	int              weight;
	struct __s_node* dest;
} s_edge, *p_edge;

#define NODE_NORMAL 0
#define NODE_START 1
#define NODE_ACCEPT 2
typedef struct __s_node {
	int 			id; 	// the identity of the node, it's a unique 
	int 			info; 	// the information of the node, like it's START_NODE or ACCEPT_NODE
	struct __s_edgelist* 	pel_f; 	// pointer to the first node in the edge list
	struct __s_edgelist* 	pel_l; 	// pointer to the last node in the edge list
} s_node, *p_node;

/** 
	edge list is a list maintain in a certain node, when we create a new edge and add it to a
	node, we should add it to the node list
 */
typedef struct __s_edgelist {
	struct __s_edge* 	element;
	struct __s_edgelist* 	next;
} s_edgelist, *p_edgelist;

/** 
	node list is a list maintain in a certain graph, when we create a new node and add it to a
	graph, we should add it to the node list
 */
typedef struct __s_nodelist {
	struct __s_node* 	element;
	struct __s_nodelist* 	next;
} s_nodelist, *p_nodelist;

typedef struct __s_nfa {
	struct __s_nodelist* 	pnl_f; 	// pointer to the first node in node list
	struct __s_nodelist* 	pnl_l; 	// pointer to the last node in node list
} s_nfa, *p_nfa;

typedef struct __s_dfa {
	struct __s_nodelist* 	pnl_f;
	struct __s_nodelist* 	pnl_l;
} s_dfa, *p_dfa;

#define ID_SENTINEL 100000
#define ID_NOTEXSIT -1
typedef struct __s_ndmap_ele {
	struct __s_node* 	dfa_pn;
	struct __s_nodelist* nfa_pnl;

	struct __s_ndmap_ele* next;
} s_ndmap_ele, *p_ndmap_ele;

typedef struct __s_ndmap {
	struct __s_ndmap_ele* pf;
	struct __s_ndmap_ele* pl;
} s_ndmap, *p_ndmap;

p_edge 	edge_new( int w, p_node pdn );
void 	edge_del( p_edge pn );

int  	node_make_id(); 		// return a unique id for node
p_node 	node_new( int node_id );
void 	node_del( p_node pn );
void 	node_addedge( p_node pn, p_edge pe );
p_edge 	node_findedge( p_node dest, p_node source );


p_nfa 	nfa_new();
p_nfa 	nfa_copy( p_nfa source );
p_nfa 	nfa_copy_with_different_id( p_nfa source );
void 	nfa_del( p_nfa pa );
void 	nfa_addnode( p_nfa pa, p_node pn );

/** concat two nfa to to dest */
p_nfa 	nfa_concat( p_nfa source, p_nfa second );
p_nfa 	nfa_branch( p_nfa first, p_nfa second );
p_nfa 	nfa_closure( p_nfa source );
p_nfa 	nfa_multiple( p_nfa source, int times );
p_nfa 	nfa_make_from_stringconcat( const char* str );
p_nfa 	nfa_make_from_stringbranch( const char* str );

/* map an id list to a single dfa id */
p_ndmap ndmap_new();
void 	ndmap_add( p_ndmap pim, p_node pn, p_nodelist pnl );
p_nodelist ndmap_find_via_dfanode( p_ndmap pim, p_node pn );
p_node 	ndmap_find_var_nfanodelist( p_ndmap pim, p_nodelist pnl );
void 	ndmap_del( p_ndmap pim );

/* */
p_dfa 	dfa_new();
void 	dfa_del( p_dfa pa );
void 	dfa_addnode( p_dfa pa, p_node pn );
void  	dfa_del( p_dfa pda );
p_dfa 	dfa_convert_from_nfa( p_nfa nfa );

#ifdef _ENABLE_PRINT_
void 	nfa_print( p_nfa pa );
void 	dfa_print( p_dfa dfa );
void 	dfa_print_table( p_dfa dfa );
void 	nodelist_print( p_nodelist pnl );
#endif
#endif // _REG_FA_
