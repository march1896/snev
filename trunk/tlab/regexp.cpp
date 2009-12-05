#include "regexp.h"
#include "regexp_fa.h"
#include "heap2.h"
#include "stdio.h"
namespace regexp {
enum SYMBOL {
	STAR = 0,
	CONCAT = 1,
	OR,
	LBRACE,
	RBRACE,
	NONE,
};
int SymbolWeight[ NONE + 1 ] = { 1, 2, 2, 3, 3, 4 };

struct SymbolStackElement {
	SYMBOL symbol;
	SymbolStackElement* next;
};
static SymbolStackElement* symbolstacktop = NULL;

static void PushSymbol( SYMBOL s ) {
	SymbolStackElement* sse = new SymbolStackElement();
	sse->symbol = s;
	sse->next = symbolstacktop;
	symbolstacktop = sse;
}

static SYMBOL PopSymbol() {
	if ( symbolstacktop == NULL ) {
		return NONE;
	}
	else {
		SYMBOL ret = symbolstacktop->symbol;
		SymbolStackElement* sse = symbolstacktop;
		symbolstacktop = symbolstacktop->next;
		delete sse;
		return ret;
	}
}

static SYMBOL TopSymbol() {
	if ( symbolstacktop == NULL ) {
		return NONE;
	}
	else {
		return symbolstacktop->symbol;
	}
}

static int SymbolStackSize() {
	SymbolStackElement* sse = symbolstacktop;
	int ret = 0;
	while ( sse != NULL ) {
		sse = sse->next;
		ret ++;
	}
	return ret;
}

static bool SymbolGE( SYMBOL lhs, SYMBOL rhs ) {
	return SymbolWeight[ lhs ] >= SymbolWeight[ rhs ];
}

static void ChangeStack( Context* con ) {
	printf( "Change Stack\n" );
	SYMBOL s = PopSymbol();
	Nfa* n1, *n2, *nt;
	
	if ( s == CONCAT ) {
		n2 = PopNfa( con );
		n1 = PopNfa( con );
		nt = AtomicConcatenateNfa( con, n1, n2 );
		PushNfa( con, nt );
	}
	else if ( s == OR ) {
		n2 = PopNfa( con );
		n1 = PopNfa( con );
		nt = AtomicOrNfa( con, n1, n2 );
		PushNfa( con, nt );
	}
	else if ( s == STAR ) {
		// impossible
	}
}

static bool isletter( const char c ) {
	return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z';
}

void OutputContext( Context* con ) {
	State* s;
	Edge* e;
	Node* n;
	n = con->nfanodelist;
	while ( n != NULL ) {
		printf( "Node\n" );
		s = n->statelist;
		while ( s != NULL ) {
			printf( "\tstate: %d\n", s->dummy );
			s = s->next;
		}

		e = n->edgelist;
		while ( e != NULL ) {
			printf ("\tedge: %d %c\n", e->dest->statelist->dummy, e->weight );
			e = e->next;
		}

		printf( "\n" );
		n = n->next;
	}
	printf( "Start: %d\n", con->nfa_stacktop->nfa->starts->statelist->dummy );
	printf( "Accepts: %d\n", con->nfa_stacktop->nfa->accepts->statelist->dummy );
	return;
}

int ComputR( Node* n ) {
	State* s = n->statelist;
	int ret = 0;
	
	while ( s != NULL ) {
		//printf( "%d\t", s->dummy );
		ret += s->dummy;
		s = s->next;
	}
	return ( (ret*17337) % 100 );
}
void OutputDfa( Context* con ) {
	DfaNodeElement* dne = con->dne_queue;
	while ( dne != NULL ) {
		Node* node = dne->node;
		printf( "Node %d\n", ComputR( dne->node ) );
		Edge* e = node->edgelist;
		printf( "Edges\n" );
		while ( e != NULL ) {
			printf( "\t%c\t%d\n", e->weight, ComputR( e->dest ) );
			e = e->next;
		}
		dne = dne->next;
	}

	printf( "Starts: %d\n", ComputR( con->dfa->starts ) );
	Node* n = con->dfa->accepts;
	printf( "Accepts: " );
	while ( n != NULL ) {
		printf( "%d\t", ComputR( n ) );
		n = n->next;
	}
	printf( "\n" );
}

Context* RegexpCompile( const char* Pattern ) {
	Context* con = MakeContext();
	const char *p = Pattern;
	
	while ( *p != '\0' ) {
		printf( "deal with %c\n", *p );
		printf( "nfa size: %d\t%d\n", NumNfaList( con ), NumNfaStack( con ) );
		printf( "symbol stack size: %d\n", SymbolStackSize() );
		if ( isletter( *p ) ) {
			Nfa* nfa = AtomicMakeNfaBySingleC( con, *p );
			PushNfa( con, nfa );

			while ( SymbolGE( CONCAT, TopSymbol() )  ) {
				ChangeStack( con );
			}
			PushSymbol( CONCAT );
		}
		else if ( *p == '|' ) {
			if ( TopSymbol() == CONCAT ) {
				PopSymbol();
			}
			else {
				printf("error\n");
			}

			while ( SymbolGE( OR, TopSymbol() ) ) {
				ChangeStack( con );
			}

			PushSymbol( OR );
		}
		else if ( *p == '*' ) {
			if ( TopSymbol() == CONCAT ) {
				PopSymbol();
			}
			else {
				printf("error\n");
			}

			Nfa* nfa = PopNfa( con );
			nfa = AtomicClosureNfa( con, nfa );
			PushNfa( con, nfa );

			PushSymbol( CONCAT );
		}
		else if ( *p == '(' ) {
			PushSymbol( LBRACE );
		}
		else if ( *p == ')' ) {
			if ( TopSymbol() == CONCAT ) {
				PopSymbol();
			}
			else {
				printf("error\n");
			}

			while ( SymbolGE( RBRACE, TopSymbol() ) ) {
				if ( TopSymbol() == LBRACE ) {
					PopSymbol();
					break;
				}
				ChangeStack( con );
			}

			PushSymbol( CONCAT );
		}
		p ++;
	}

	while ( TopSymbol() != NONE ) {
		PopSymbol();
	}
	return con;
}

bool 	RegexpAcceptable( const char* Text, const Context* con ) {
	const char *p = Text;
	Node* n = con->dfa->starts;
	bool found;
	while ( *p != '\0' ) {
		Edge* e = n->edgelist;
		
		found = false;
		while ( e != NULL ) {
			if ( e->weight == *p ) {
				found = true;
				break;
			}
			e = e->next;
		}
		if ( found ) {
			n = e->dest;
			p ++;
		}
		else {
			return false;
		}
	}
	Node* nt = con->dfa->accepts;
	while ( nt != NULL ) {
		if ( nt == n ) return true;
		nt = nt->next;
	}
	return false;
}
}
