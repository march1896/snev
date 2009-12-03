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
Nfa* 	PopNfa( Context* con );
void  	PushNfa( Context* con, Nfa* nfa );
int 	NumNfaList( Context* con );
int 	NumNfaStack( Context* con );

State* MakeState( Context* con );
void AddState( Node* n, State* s );
void RemoveState( Context* con, State* s );

Node* AddNode( Context* con, State* statelist );
void RemoveNode( Context* con, Node* node );

Edge* AddEdge( Node* start, Node* end, Weight w );
//Edge* AddEdge( Context* con, State* start, State* end, Weight );
void RemoveEdge( Edge* e );

Nfa* AddNfa( Context* con, Node* starts, Node* accepts );
void RemoveNfa( Context* con, Nfa* nfa );

Nfa* AtomicMakeNfaBySingleC( Context* con, const char c );
Nfa* AtomicConcatenateNfa( Context* con, Nfa* first, Nfa* second );
Nfa* AtomicOrNfa( Context* con, Nfa* first, Nfa* second );
Nfa* AtomicClosureNfa( Context* con, Nfa* in );

Dfa* CompileNfaToDfa( Context* con );
// a dummy graph is always maintained
struct NfaStackElement {
	Nfa* 	nfa;
	NfaStackElement* 	next;
};

struct Context {
	int 	state_count;
	Node* 	nodelist;
	Nfa* 	nfalist;
	NfaStackElement* 	stacktop;
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
} // namespace
#endif
