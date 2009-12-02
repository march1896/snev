#include "regexp_fa.h"

namespace regexp {

Context* MakeContext() {
	Context* ret = new Context();
	ret->state_count = 0;
	ret->nodelist = NULL;
	ret->nfalist = NULL;
	ret->stateliat = NULL;
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

	// remove all state
	State *prev_s, *next_s;
	prev_s = next_s = con->statelist;
	while ( next_s != NULL ) {
		prev_s = next_s;
		next_s = next_s->next;
		
		RemoveState( con, prev_s );
	}

	// remove all nfas
	Nfa *prev_n, *next_n;
	prev_n = next_n = con->nfalist;
	while ( next_n != NULL ) {
		prev_n = next_n;
		next_n = next_n->next;

		RemoveNfa( con, prev_n );
	}

	delete con;
}

State* AddState( Context *con ) {
	State* s = new State();
	s->dummy = con->state_count ++;
	s->next = NULL;
	
	if ( con->statelist == NULL ) {
		con->statelist = s;
	}
	else {
		State* temp = con->statelist;
		while ( temp->next != NULL ) {
			temp = temp->next;
		}
		temp->next = s;
	}
	return s;
}

void RemoveState( Context* con, State* s ) {
	State *prev, *next;

	prev = next = con->statelist;

	while ( next != NULL ) {
		 if ( s == next ) {
			 if ( prev == s->statelist ) {
				 s->statelist = next->next;
			 }
			 else {
				 prev->next = next->next;
			 }
			 delete next;
			 return;
		 }
		 prev = next;
		 next = next->next;
	}
	
	//assert( false );
}

Node* AddNode( Context* con, State* sl ) {
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

void RemoveNode( Context* con, Node* n ) {
	Edge 	*prev_edge, *next_edge;
	Node 	*prev_node, *next_node;
	prev_node = next_node = con->nodelist;

	while ( next_node != NULL ) {
		if ( next_node == n ) {
			// remove all edges out
			next_edge = n->edgelist;
			while ( next_edge != NULL ) {
				prev_edge = next_edge;
				next_edge = next_edge->next;
				delete prev_edge;
			}
			if ( prev_node == con->nodelist ) {
				prev_node = con->nodelist = next->next;
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
					if ( prev_edge == next_node->edgelist ) {
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

Nfa* AddNfa( Context* con ) {
	
}

void RemoveNfa( Context* con, Nfa* nfa ) {
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
	
}; // namespace
