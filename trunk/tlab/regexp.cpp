#include "heap2.h"
#include "regexp.h"
#include "regexp_fa.h"
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
	n = con->nodelist;
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
	printf( "Start: %d\n", con->stacktop->nfa->starts->statelist->dummy );
	printf( "Accepts: %d\n", con->stacktop->nfa->accepts->statelist->dummy );
	return;
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
	return con;
}

}
