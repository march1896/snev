#include "regexp_fa.h"
#include "heap2.h"
#include "stdio.h"

namespace regexp {

static int a = 0;
Context* MakeContext() {
	Context* ret = new Context();
	ret->state_count = 0;
	ret->nfanodelist = NULL;
	ret->nfalist = NULL;
	ret->nfa_stacktop = NULL;

	ret->dne_queue = NULL;
	ret->dfa = NULL;

	ret->symbollist = NULL;
}

void DestorySymbol( Context* con );
void DestoryContext( Context* con ) {
	// remove all nodes and edges
	Node *node = con->nfanodelist;
	Node *temp;

	node = con->nfanodelist;
	
	while ( node != NULL ) {
		temp = node;
		node = node->next;
		RemoveNode( con, temp );
	}

	// remove all nfas
	Nfa *prev_n, *next_n;
	prev_n = next_n = con->nfalist;
	while ( next_n != NULL ) {
		prev_n = next_n;
		next_n = next_n->next;

		RemoveNfa( con, prev_n );
	}

	NfaStackElement* nse = con->nfa_stacktop;
	NfaStackElement* t;

	while ( nse != NULL ) {
		t = nse;
		nse = nse->next;
		delete t;
	}

	DfaNodeElement* dne = con->dne_queue;
	DfaNodeElement* prev_dne;
	while ( dne != NULL ) {
		Node* node = dne->node;
		State* ss = node->statelist;
		State* _s;
		while ( ss != NULL ) {
			_s = ss;
			ss = ss->next;
			delete _s;
		}
		Edge* ee = node->edgelist;
		Edge* _e;
		while ( ee != NULL ) {
			_e = ee;
			ee = ee->next;
			delete _e;
		}
		delete node;
		prev_dne = dne;
		dne = dne->next;
		delete prev_dne;
	}
	delete con->dfa;

	DestorySymbol( con );
	delete con;
}

void PushNfa( Context* con, Nfa* nfa ) {
	NfaStackElement* nse = new NfaStackElement();
	nse->nfa = nfa;

	nse->next = con->nfa_stacktop;
	con->nfa_stacktop = nse;
}

Nfa* PopNfa( Context* con ) {
	NfaStackElement* nse = con->nfa_stacktop;
	if ( nse == NULL ) {
		return NULL;
	}
	else {
		con->nfa_stacktop = nse->next;
		Nfa* nfa = nse->nfa;
		delete nse;
		return nfa;
	}
}

void PushDfaNode( Context* con, Node* node ) {
	DfaNodeElement* dne = new DfaNodeElement();
	dne->node = node;
	dne->colored = false;
	dne->next = NULL;

	DfaNodeElement* t = con->dne_queue;
	if ( t == NULL ) {
		con->dne_queue = dne;
	}
	else {
		while ( t->next != NULL ) {
			t = t->next;
		}
		t->next = dne;
	}
}

Node* PopDfaNode( Context* con ) {
	DfaNodeElement* dne = con->dne_queue;

	if ( dne == NULL ) {
		return NULL;
	}
	else {
		con->dne_queue = dne->next;
		Node* ret = dne->node;
		delete dne;
		return ret;
	}
}

DfaNodeElement* FirstUncoloredNode( Context* con ) {
	DfaNodeElement* dne = con->dne_queue;

	while ( dne != NULL ) {
		if ( !dne->colored ) {
			return dne;
		}
		dne = dne->next;
	}
	return NULL;
}

static bool StateEqual( State* sl1, State* sl2 ) {
	// all states in sl1 can be found in sl2, vice vser
	State* s;
	s = sl1;
	bool found;
	while ( s != NULL ) {
		found = false;
		State* temp = sl2;

		while ( temp != NULL ) {
			if ( s->dummy == temp->dummy ) {
				found = true;
				break;
			}
			temp = temp->next;
		}
		if ( !found ) return false;
		s = s->next;
	}

	s = sl2;
	while ( s != NULL ) {
		found = false;
		State* temp = sl1;

		while ( temp != NULL ) {
			if ( s->dummy == temp->dummy ) {
				found = true;
				break;
			}
			temp = temp->next;
		}
		if ( !found ) return false;
		s = s->next;
	}
	return true;
}

Node* FindDfaNode( Context* con, Node* nn ) {
	DfaNodeElement* dne = con->dne_queue;

	while ( dne != NULL ) {
		State* sl1 = dne->node->statelist;
		State* sl2 = nn->statelist;

		/* test code
		State* temp = sl1;
		printf( "\n" );
		while ( temp != NULL ) {
			printf( "%d\t", temp->dummy );
			temp = temp->next;
		}
		temp = sl2;
		printf( "\n" );
		while ( temp != NULL ) {
			printf( " fuck %d\t", temp->dummy );
			temp = temp->next;
		}
		// test code end
		*/
		
		if ( StateEqual( sl1, sl2 ) ) {
			return dne->node;
		}

		dne = dne->next;
	}
	return NULL;
}

int NumNfaList( Context* con ) {
	Nfa* nfa = con->nfalist;
	int ret = 0;
	while ( nfa != NULL ) {
		nfa = nfa->next;
		ret ++;
	}
	return ret;
}

int NumNfaStack( Context* con ) {
	NfaStackElement* nse = con->nfa_stacktop;
	int ret = 0;
	while ( nse != NULL ) {
		nse = nse->next;
		ret ++;
	}
	return ret;
}

State* MakeState( Context* con ) {
	State* s = new State();
	s->dummy = con->state_count ++;
	s->next = NULL;
	return s;
}

State* MakeDfaState( Context* con, int count ) {
	State* s = new State();
	s->dummy = count;
	s->next = NULL;
	return s;
}

void AddState( Node* n, State* s ) {
	State* t = n->statelist;
	if ( t == NULL ) {
		n->statelist = s;
	}
	else {
		while ( t->next != NULL ) {
			if ( t == s ) return;
			t = t->next;
		}
		t->next = s;
	}
	return;
}

bool FindState( Node* n, State* s ) {
	State* t = n->statelist;
	while ( t != NULL ) {
		if ( t == s ) {
			return true;
		}
		t = t->next;
	}
	return false;
}

Node* AddNode( Context* con ) {
	Node* new_node = new Node();
	new_node->next = NULL;
	new_node->statelist = NULL;
	new_node->edgelist = NULL;

	// add this node into the context
	if ( con->nfanodelist == NULL ) {
		con->nfanodelist = new_node;
	}
	else {
		Node* l = con->nfanodelist;
		while ( l->next != NULL ) {
			l = l->next;
		}
		l->next = new_node;
	}
	return new_node;
}

/*
Node* DfaAddNode( Context* con ) {
	Node* new_node = new Node();
	new_node->next = NULL;
	new_node->statelist = NULL;
	new_node->edgelist = NULL;

	if ( con->dfanodelist == NULL ) {
		con->dfanodelist = new_node;
	}
	else {
		Node* l = con->dfanodelist;
		while ( l->next != NULL ) {
			l = l->next;
		}
		l->next = new_node;
	}
	return new_node;
}
*/

void RemoveNode( Context* con, Node* n ) {
	Edge 	*prev_edge, *next_edge;
	Node 	*prev_node, *next_node;
	prev_node = next_node = con->nfanodelist;
	
	while ( next_node != NULL ) {
		if ( next_node == n ) {
			// remove all state;
			State *prev, *next; 
			prev = next = next_node->statelist;

			while ( next != NULL ) {
				prev = next;
				next = next->next;
				delete prev;
			}
			n->statelist = NULL;

			// remove all edges out
			next_edge = n->edgelist;
			while ( next_edge != NULL ) {
				prev_edge = next_edge;
				next_edge = next_edge->next;
				delete prev_edge;
			}

			Node* temp = next_node;
			if ( next_node == con->nfanodelist ) {
				con->nfanodelist = next_node->next;
				prev_node = next_node = con->nfanodelist;
			}
			else {
				prev_node->next = next_node->next;
				next_node = next_node->next;
			}
			delete temp;
			n = NULL;
		}
		else {
			// remove all edges in
			prev_edge = next_edge = next_node->edgelist;

			while ( next_edge != NULL ) {
				if ( next_edge->dest == n ) {
					Edge* ttemp = next_edge;
					if ( next_edge == next_node->edgelist ) {
						next_edge = prev_edge = next_node->edgelist = next_edge->next;
					}
					else {
						prev_edge->next = next_edge->next;
						next_edge = next_edge->next;
					}
					delete ttemp;
				}
				else {
					prev_edge = next_edge;
					next_edge = next_edge->next;
				}
			}
			prev_node = next_node;
			next_node = next_node->next;
		}
	}
}

Edge* AddEdge( Node* from, Node* to, Weight w ) {
	Edge* 	new_edge = new Edge();
	new_edge->dest = to;
	new_edge->weight = w;
	new_edge->next = NULL;

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
	return new_edge;
}

void RemoveEdge( Edge* e ) {
	delete e;
}

Nfa* AddNfa( Context* con ) {
	Nfa* nfa = new Nfa();
	nfa->starts = NULL;
	nfa->accepts = NULL;
	nfa->next = NULL;

	if ( con->nfalist == NULL ) {
		con->nfalist = nfa;
	}
	else {
		Nfa* n = con->nfalist;
		while ( n->next != NULL ) {
			n = n->next;
		}
		n->next = nfa;
	}
	return nfa;
}

void RemoveNfa( Context* con, Nfa* nfa ) {
	Nfa* prev, *next;
	prev = next = con->nfalist;
	while ( next != NULL ) {
		if ( next == nfa ) {
			if ( next == con->nfalist ) {
				con->nfalist = next->next;
			}
			else {
				prev->next = next->next;
			}
			break;
		}
		prev = next;
		next = next->next;
	}
	delete nfa;
}

// atomic operations
Nfa* AtomicMakeNfaBySingleC( Context* con, const char c ) {
	AddSymbol( con, c );
	Nfa* ret = AddNfa( con );
	Node* start = AddNode( con );
	State* ss = MakeState( con );
	AddState( start, ss);
	Node* end = AddNode( con );
	ss = MakeState( con );
	AddState( end, ss);
	AddEdge( start, end, c );
	ret->starts = start;
	ret->accepts = end;
	return ret;
}

Nfa* AtomicConcatenateNfa( Context* con, Nfa* first, Nfa* second ) {
	Node* 	fa = first->accepts;
	Node* 	ss = second->starts;
	Edge* 	edge = ss->edgelist;

	while ( edge != NULL ) {
		AddEdge( fa, edge->dest, edge->weight );
		edge = edge->next;
	}
	RemoveNode( con, second->starts );
	first->accepts = second->accepts;
	RemoveNfa( con, second );
	return first;
}

Nfa* AtomicOrNfa( Context* con, Nfa* first, Nfa* second ) {
	Node* 	n1 = AddNode( con );
	AddState( n1, MakeState( con ) );
	AddEdge( n1, first->starts, EPSILON );
	AddEdge( n1, second->starts, EPSILON );

	Node* 	n2 = AddNode( con );
	AddState( n2, MakeState( con ) );
	AddEdge( first->accepts, n2, EPSILON );
	AddEdge( second->accepts, n2, EPSILON );

	Nfa* 	nfa = AddNfa( con );
	nfa->starts = n1;
	nfa->accepts = n2;

	RemoveNfa( con, first );
	RemoveNfa( con, second );
	return nfa;
}

Nfa* AtomicClosureNfa( Context* con, Nfa* in ) {
	Node* 	n1 = AddNode( con );
	AddState( n1, MakeState( con ) );
	AddEdge( n1, in->starts, EPSILON );
	Node* 	n2 = AddNode( con );
	AddState( n2, MakeState( con ) );
	AddEdge( in->accepts, n2, EPSILON );

	AddEdge( in->accepts, in->starts, EPSILON );
	AddEdge( n1, n2, EPSILON );

	in->starts = n1;
	in->accepts = n2;
	return in;
}

struct StateEdge {
	StateEdge* next;
	State* 	from;
	State* 	to;
	Weight 	w;
};
static StateEdge* sequeue = NULL;
static StateEdge* PushStateEdge( State* from, State* to, Weight w ) {
	StateEdge* se = new StateEdge();
	se->next = NULL;
	se->from = from;
	se->to = to;
	se->w = w;

	if ( sequeue == NULL ) {
		sequeue = se;
	}
	else {
		StateEdge* t = sequeue;
		while ( t->next != NULL ) {
			t = t->next;
		}
		t->next = se;
	}
	return se;
}

void InitStateEdge( Context* con ) {
	Node *n;
	n = con->nfanodelist;
	Edge* e;
	
	while ( n != NULL ) {
		e = n->edgelist;
		while ( e != NULL ) {
			PushStateEdge( n->statelist, e->dest->statelist, e->weight );
			e = e->next;
		}
		n = n->next;
	}
	return;
}

static StateEdge* FindStateEdge( const State* from, const State* to ) {
	StateEdge* t = sequeue;
	while ( t != NULL ) {
		if ( t->from == from && t->to == to ) {
			return t;
		}
	}
	return NULL;
}

static void DestoryStateEdge() {
	StateEdge *prev, *next;
	prev = next = sequeue;

	while ( next != NULL ) {
		prev = next;
		next = next->next;
		delete prev;
	}
	sequeue = NULL;
	return;
}

struct StateStackElement {
	StateStackElement* next;
	State* 	state;
};
static StateStackElement* ssetop = NULL;

void PushSSE( State* s ) {
	StateStackElement* sse = new StateStackElement();
	
	sse->state = s;
	sse->next = ssetop;

	ssetop = sse;
}

State* PopSSE() {
	StateStackElement* sse = ssetop;
	if ( ssetop != NULL ) {
		ssetop = ssetop->next;
		State* s = sse->state;
		delete sse;
		return s;
	}
	return NULL;
}

void DestorySSE() {
	StateStackElement* prev, *next;
	prev = next = ssetop;
	while ( next != NULL ) {
		prev = next;
		next = next->next;
		delete prev;
	}
	ssetop = NULL;

	return;
}

void EpsilonClosure( Context* con, Node *dfa_node, State* s ) {
	if ( s == NULL ) return;
	DestorySSE();
	while ( s != NULL ) {
		PushSSE( s );
		s = s->next;
	}

	while ( ssetop != NULL ) {
		State* st = PopSSE();

		StateEdge* se = sequeue;
		while ( se != NULL ) {
			if ( se->w == EPSILON && se->from->dummy == st->dummy && !FindState( dfa_node, se->to ) ) {
				AddState( dfa_node, MakeDfaState( con, se->to->dummy ) );
				PushSSE( se->to );
			}
			se = se->next;
		}
	}
	return;
}

const int CHARSIZE = 256;
void CompileNfaToDfa( Context* con ) {
	InitStateEdge( con );

	//Node* n = DfaAddNode( con );
	Node* n = new Node();
	n->statelist = NULL;
	n->edgelist = NULL;
	n->next = NULL;
	Node* t = con->nfa_stacktop->nfa->starts;
	AddState( n, MakeDfaState( con, t->statelist->dummy ) );
	EpsilonClosure( con, n, n->statelist );
	PushDfaNode( con, n );

	con->dfa = new Dfa();
	con->dfa->next = NULL;
	con->dfa->starts = n;
	con->dfa->accepts = NULL;
	
	DfaNodeElement* dne;

	while ( ( dne = FirstUncoloredNode( con ) ) != NULL ) {
		n = dne->node;
		// test code
		DfaNodeElement* _dne = con->dne_queue;
		printf( "printf dne queue\n" );
		while ( _dne != NULL ) {
			Node *_n = _dne->node;
			State* _ss = _n->statelist;
			while ( _ss != NULL ) {
				printf( "%d\t", _ss->dummy );
				_ss = _ss->next;
			}
			printf( " colored:%d\n", _dne->colored );
			_dne = _dne->next;
		}
		// test code end

		dne->colored = true;

		State* s = n->statelist;
		Symbol* sym = con->symbollist;
		Weight w;

		while ( sym != NULL ) {
			w = sym->symbol;
			if ( w == EPSILON ) continue;
			StateEdge* se = sequeue;

			Node *nn = new Node();
			nn->statelist = NULL;
			nn->edgelist = NULL;
			nn->next = NULL;

			while ( se != NULL ) {
				//printf( "%c", se->w );
				if ( se->w == w ) {
					// if find se->from in n->statelist;
					bool found = false;
					State* _s = n->statelist;
					while ( _s != NULL ) {
						if ( _s->dummy == se->from->dummy ) {
							found = true; 
							break;
						}
						_s = _s->next;
					}
					
					if ( found ) {
						AddState( nn, MakeDfaState( con, se->to->dummy ) );
					}
				}
				se = se->next;
			}

			EpsilonClosure( con, nn, nn->statelist );

			if ( nn->statelist == NULL ) {
				delete nn;
			}
			else {
				if ( FindDfaNode( con, nn ) == NULL ) {
					PushDfaNode( con, nn );
				}
				else {
					Node* temp = nn;
					nn = FindDfaNode( con, nn );
					State* prev, *next;
					prev = next = temp->statelist;
					while ( next != NULL ) {
						prev = next;
						next = next->next;
						delete prev;
					}
					delete temp;
				}

				Edge* ee = new Edge();
				ee->dest = nn;
				ee->weight = w;
				ee->next = n->edgelist;
				n->edgelist = ee;
			}

			sym = sym->next;
		}
	}

	dne = con->dne_queue;
	while ( dne != NULL ) {
		Node *_n = dne->node;
		State* _ss = _n->statelist;
		while ( _ss != NULL ) {
			if ( _ss->dummy == con->nfa_stacktop->nfa->accepts->statelist->dummy ) {
				_n->next = con->dfa->accepts;
				con->dfa->accepts = _n;
				break;
			}
			_ss = _ss->next;
		}
		dne = dne->next;
	}

	// test code
	DfaNodeElement* _dne = con->dne_queue;
	/*
	printf( "printf dne queue\n" );
	while ( _dne != NULL ) {
		Node *_n = _dne->node;
		State* _ss = _n->statelist;
		while ( _ss != NULL ) {
			printf( "%d\t", _ss->dummy );
			_ss = _ss->next;
		}
		printf( " colored:%d\n", _dne->colored );
		_dne = _dne->next;
	}
	*/
	// test code end

	DestoryStateEdge();
	//DestoryContext( con );
}

void AddSymbol( Context* con, Weight w ) {
	bool found = false;
	Symbol* sym = con->symbollist;
	Symbol* prev = sym;

	while ( sym != NULL ) {
		if ( sym->symbol == w ) {
			found = true;
		}
		prev = sym;
		sym = sym->next;
	}
	
	if ( !found ) {
		sym = new Symbol();
		sym->symbol = w;
		sym->next = NULL;
		if ( prev == NULL ) {
			con->symbollist = sym;
		}
		else {
			prev->next = sym;
		}
	}
}

void DestorySymbol( Context* con ) {
	Symbol *prev, *next;

	prev = next = con->symbollist;
	while ( next != NULL ) {
		prev = next;
		next = next->next;
		delete prev;
	}
	con->symbollist = NULL;

	return;
}

}
	
