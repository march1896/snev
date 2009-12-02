#ifndef _TJ_REGEXP_FA_
#define _TJ_REGEXP_FA_
namespace regexp {
#define EPSILON '\0'
typedef char Weight;
struct State;
struct Edge;
struct Node;
struct Context;
struct Dfa;
typedef Dfa Nfa;

Context* MakeContext();
void DestoryContext( Context* con );

State* AddState( Context* con );
void RemoveState( Context* con );

Node* AddNode( Context* con, State* statelist );
void RemoveNode( Context* con, Node* node );

Edge* AddEdge( Node* start, Node* end, Weight w );
//Edge* AddEdge( Context* con, State* start, State* end, Weight );
void RemoveEdge( Edge* e );

Nfa* AddNfa( Context* con, Node* starts, Node* accepts );
void RemoveNfa( Context* con, Nfa* nfa );

// a dummy graph is always maintained
struct Context {
	int 	state_count;
	Node* 	nodelist;
	Nfa* 	nfalist;
	State* 	statelist;
};

struct State {
	State*  next;
	int 	dummy; 
};

struct Edge {
	Edge* 	next;
	Node* 	dest;
	Weight 	weight;
};

struct Node {
	Node* 	next;
	State*  statelist;
	Edge* 	edgelist;
};

// dfa or nfa is just pick out some nodes as start node and accept node, but not 
// maintain node infomation, the data is stored in the context
struct Dfa {
	Dfa* 	next;
	Node* 	starts;
	Node* 	accepts;
};
}; // namespace
#endif
