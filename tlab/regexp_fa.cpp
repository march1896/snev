#include "regexp_fa.h"
#include "heap2.h"
#include "stdio.h"

namespace regexp {

Context* MakeContext() {
	Context* ret = new Context();
	ret->state_count = 0;
	ret->nodelist = NULL;
	ret->nfalist = NULL;
	ret->stacktop = NULL;
}

void DestoryContext( Context* con ) {
	// remove all nodes and edges
	Node *node = con->nodelist;
	Node *temp;
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

	NfaStackElement* nse = con->stacktop;
	NfaStackElement* t;

	while ( nse != NULL ) {
		t = nse;
		nse = nse->next;
		delete t;
	}

	delete con;
}

void PushNfa( Context* con, Nfa* nfa ) {
	NfaStackElement* nse = new NfaStackElement();
	nse->nfa = nfa;

	nse->next = con->stacktop;
	con->stacktop = nse;
}

Nfa* PopNfa( Context* con ) {
	NfaStackElement* nse = con->stacktop;
	if ( nse == NULL ) {
		return NULL;
	}
	else {
		con->stacktop = nse->next;
		Nfa* nfa = nse->nfa;
		delete nse;
		return nfa;
	}
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
	NfaStackElement* nse = con->stacktop;
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

void AddState( Node* n, State* s ) {
	State* t = n->statelist;
	if ( t == NULL ) {
		n->statelist = s;
	}
	else {
		while ( t->next != NULL ) {
			t = t->next;
		}
		t->next = s;
	}
}

Node* AddNode( Context* con ) {
	Node* new_node = new Node();
	new_node->next = NULL;
	new_node->statelist = NULL;
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
	return new_node;
}

void RemoveNode( Context* con, Node* n ) {
	Edge 	*prev_edge, *next_edge;
	Node 	*prev_node, *next_node;
	prev_node = next_node = con->nodelist;
	// remove all state;
	State *prev, *next; 
	prev = next = n->statelist;

	while ( next != NULL ) {
		prev = next;
		next = next->next;
		delete prev;
	}

	// remove all edges out
	next_edge = n->edgelist;
	while ( next_edge != NULL ) {
		prev_edge = next_edge;
		next_edge = next_edge->next;
		delete prev_edge;
	}

	while ( next_node != NULL ) {
		if ( next_node == n ) {
			if ( next_node == con->nodelist ) {
				prev_node = con->nodelist = next_node->next;
			}
			else {
				prev_node->next = next_node->next;
			}
			Node* temp = next_node;
			next_node = next_node->next;
			delete temp;
			continue;
		}
		else {
			// remove all edges in
			prev_edge = next_edge = next_node->edgelist;

			while ( next_edge != NULL ) {
				if ( next_edge->dest == n ) {
					Edge* temp = next_edge;
					if ( next_edge == next_node->edgelist ) {
						prev_edge = next_node->edgelist = next_edge->next;
					}
					else {
						prev_edge->next = next_edge->next;
					}
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

Context* CompileNfaToDfa( Context* con ) {
	Context* dfa_c = MakeContext();
	Node* n_n = con->starts;
	Node* d_n = AddNode( dfa_c );

	DestoryContext( con );
}

}
	
