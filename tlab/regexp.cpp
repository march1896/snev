#include "heap2.h"
#include "regexp.h"
namespace regexp {
#define EPSILON '\0'

typedef char Weight;
struct State {
	int 	dummy;
	State*  next;
};
struct Node;
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

// a dummy graph is always maintained
struct Context {
	int 	state_count;
	Node* 	nodelist;
};

Context* MakeContext() {
	Context* ret = new Context();
	ret->state_count = 0;
	ret->nodelist = NULL;
}

void DestoryContext( Context* con ) {
	Node *node = con->nodelist;
	Node *temp;
	Edge *prev, *next;

	while ( node != NULL ) {
		prev = next = node->edgelist;
		while ( next != NULL ) {
			prev = next;
			next = next->next;
			delete prev;
		}
		temp = node;
		node = node->next;
		delete node;
	}
	delete con;
}

// dfa or nfa is just pick out some nodes as start node and accept node, but not 
// maintain node infomation, the data is stored in the context
struct Dfa {
	Node* 	starts;
	Node* 	accepts;
	Context* 	context;
};
//#define Nfa Dfa
typedef Dfa Nfa;

State* MakeState( Context *con ) {
	State* s = new State();
	s->dummy = con->state_count ++;
	return s;
}

Node* AddNode( State* sl, Context* con ) {
	Node* new_node = new Node();
	new_node->next = NULL;
	new_node->statelist = sl;
	new_node->edgelist = NULL;

	// add this node into the context
	if ( con->nodelist == NULL ) {
		con->nodelist = new_node;
	}
	else {
		Node* l = con->nodelist;
		while ( l->next != NULL ) {
			l = l->next;
		}
		l->next = new_node;
	}
}

void RemoveNode( Node* n, Context* con ) {
	Edge 	*prev_edge, *next_edge;
	Node 	*prev_node, *next_node;
	prev_node = next_node = con->nodelist;

	bool found = false;
	while ( next_node != NULL ) {
		if ( next_node == n ) {
			// remove all edges out
			next_edge = n->edgelist;
			while ( next_edge != NULL ) {
				prev_edge = next_edge;
				next_edge = next_edge->next;
				delete prev_edge;
			}
			found = true;
		}
		else {
			// remove all edges in
			prev_edge = next_edge = next_node->edgelist;

			while ( next_edge != NULL ) {
				if ( next_edge->dest == n ) {
					Edge* temp = next_edge;

					if ( prev_edge == next_node->edgelist ) {
						next_node->edgelist = next_edge->next;
					}
					else {
						prev_edge->next = next_edge->next;
					}
					prev_edge = next_edge;
					next_edge = next_edge->next;
					delete temp;
				}
				else {
					prev_edge = next_edge;
					next_edge = next_edge->next;
				}
			}
		}

		prev_node = next_node;
		next_node = next_node->next;
	}
	
	// delete the node self
	if ( found ) {
		delete n;
	}
}

void AddEdge( Node* from, Node* to, Weight w ) {
	Edge* 	new_edge = new Edge();
	new_edge->dest = to;
	new_edge->weight = w;

	Edge* 	ep = from->edgelist;
	if ( ep == NULL ) {
		from->edgelist = new_edge;
	}
	else {
		while ( ep->next != NULL ) {
			ep = ep->next;
		}
		ep->next = new_edge;
	}
}

void RemoveEdge( Edge* e ) {
	delete e;
}


// atomic operations
Nfa* AtomicMakeNfaBySingleC( const char c, Context* con ) {
	Nfa* ret = new Nfa();
	State* ss = MakeState( con );
	Node* start = AddNode( ss, con );
	ss = MakeState( con );
	Node* end = AddNode( ss, con );
	AddEdge( start, end, c );
	ret->starts = start;
	ret->accepts = end;
	ret->context = con;
	return ret;
}

Nfa* AtomicConcatenateNfa( Nfa* first, Nfa* second, Context* con ) {
	Node* 	fa = first->accepts;
	Node* 	ss = second->starts;
	Edge* 	edge = ss->edgelist;

	while ( edge != NULL ) {
		AddEdge( fa, edge->dest, edge->weight );
		edge = edge->next;
	}
	RemoveNode( second->starts, con );
	first->accepts = second->accepts;
	delete second;
	return first;
}

Nfa* AtomicAndNfa( Nfa* first, Nfa* second, Context* con ) {
	Node* 	n1 = AddNode( MakeState( con ), con );
	AddEdge( n1, first->starts, EPSILON );
	AddEdge( n1, second->starts, EPSILON );

	Node* 	n2 = AddNode( MakeState( con ), con );
	AddEdge( first->accepts, n2, EPSILON );
	AddEdge( second->accepts, n2, EPSILON );

	Nfa* 	nfa = new Nfa();
	nfa->starts = n1;
	nfa->accepts = n2;
	nfa->context = con;

	delete first;
	delete second;
	return nfa;
}

Nfa* AtomicClosureNfa( Nfa* in, Context* con ) {
	Node* 	n1 = AddNode( MakeState( con ), con );
	AddEdge( n1, in->starts, EPSILON );
	Node* 	n2 = AddNode( MakeState( con ), con );
	AddEdge( in->accepts, n2, EPSILON );

	AddEdge( in->accepts, in->starts, EPSILON );

	in->starts = n1;
	in->accepts = n2;
	return in;
}

Dfa* GenerateDFAfromNFA( Nfa* nfa ) {

}

enum SYMBOL {
	NONE,
	LBRACE,
	RBRACE,
	STAR,
	AND,
};
#define STACK_SIZE 1000
static Nfa* StackNfa[ STACK_SIZE ];
static int SNCount = 0;
static SYMBOL StackSym[ STACK_SIZE ];
static int SSCount = 0;
void PushNfa( Nfa* nfa ) {
	if ( SNCount + 1 >= STACK_SIZE ) {
		//
		return;
	}
	StackNfa[ SNCount ++ ] = nfa;
}
Nfa* PopNfa() {
	if ( SNCount - 1 < 0 ) { 
		return NULL;
	}
	return StackNfa[ SNCount -- ];
}
void PushSymbol( SYMBOL s ) {
	if ( SSCount + 1 >= STACK_SIZE ) {
		return;
	}
	StackSym[ SSCount ++ ] = s;
}
SYMBOL PopSymbol( SYMBOL s ) {
	if ( SSCount - 1 < 0 ) {
		return NONE;
	}
	return StackSym[ SSCount -- ];
}

bool isletter( const char c ) {
	return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}
Nfa* BuildNfa( const char* start, const char *end, Context* con ) {
	if ( isletter( *start ) ) {

	}
	else if ( *start == '|' ) {
	}
	else if ( *start == '*' ) {
	}
	else if ( *start == '(' ) {
	}
	else if ( *start == ')' ) {
	}
}

Nfa* BuildNfa( const char* Pattern ) {
	if ( Pattern == NULL ) return NULL;

	int len = strlen( Pattern );
	Context con = new Context();
	return BuildNfa( Pattern, Pattern + len - 1, con );
}

Dfa* BuildDFA( const char* Pattern ) {
}
} // namespace regexp

#define _REGEXP_TEST_
#ifdef _REGEXP_TEST_
int main() {
	using namespace regexp;
	return 0;
}
#endif
