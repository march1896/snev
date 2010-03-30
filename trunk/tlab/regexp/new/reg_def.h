#ifndef _REG_DEF_
#define _REG_DEF_

struct s_node, s_nodelist, s_edge, s_edgelist;

typedef struct __s_node {
	int 		id;
	p_edge_list el;
} s_node, *p_node;

void node_new( p_node pn );
void node_del( p_node pn );
void node_addedge( p_node pn, p_edge pe )

typedef struct __s_nodelist {
	p_node 		element;
	p_node_list next;
} s_nodelist, *p_nodelist;

void nodelist_new( p_nodelist pnl );
void nodelist_del( p_nodelist pnl );
void nodelist_add( p_nodelist pnl, p_node pn );

typedef struct __s_edge {
	int 		id;
	p_node 		dest;
} s_edge, *p_edge;

void edge_new( p_edge pn );
void edge_del( p_edge pn );

typedef struct __s_edge_list {
	p_edge 		element;
	p_edge_list next;
} s_edgelist, *p_edgelist;

void edgelist_new( p_edgelist pnl );
void edgelist_del( p_edgelist pnl );
void edgelist_add( p_edgelist pnl, p_node pn );

/*
const int NUM_NODE = 1000;
const int NUM_EDGE = 3000;
typedef struct __s_graph {
	p_nodelist 	starts;
	p_nodelist 	accepts;

	p_nodelist 	nodes;

	int 		node_id_index;
} s_graph, *p_graph;

void graph_new( p_graph pg );
void graph_del( p_graph pg );
int  graph_get_nodeid( p_graph pg );
*/

typedef struct __s_nfa {
	p_node 		start;
	p_node 		accept;

	p_nodelist 	nodes;

	int 		node_id_index;
} s_nfa, *p_nfa;

void nfa_new( p_nfa pa );
void nfa_del( p_nfa pa );
int  nfa_get_nodeid( p_nfa pa );

#endif // _REG_DEF_
