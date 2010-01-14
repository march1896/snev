#include "lexer.h"
#include "lfile.h"

//#define PC
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

using namespace CSSPT;

static inline bool IsAlpha( const char* p );
static inline bool IsPunct( const char* p );
static inline bool IsDigit( const char* p );

/**********************************************************
  below is the implementation of token
  *********************************************************/
const char* TokenTypeString[ E_TOKEN_END ] = {
	"TOKEN_NULL",
	"TOKEN_STRING",
	"TOKEN_STRING_SINGLEQUOTE",
	"TOKEN_INT",
	"TOKEN_FLOAT",
	"TOKEN_IDENTITY",
	"TOKEN_LPAR",
	"TOKEN_RPAR",
	"TOKEN_LSQB",
	"TOKEN_RSQB",
	"TOKEN_COLON",
	"TOKEN_COMMA",
	"TOKEN_SEMI",
	"TOKEN_PLUS",
	"TOKEN_PLUSEQUAL",
	"TOKEN_MINUS",
	"TOKEN_MINUSEQUAL",
	"TOKEN_STAR",
	"TOKEN_MUlTIPLYEQUAL",
	"TOKEN_SLASH",
	"TOKEN_BACKSLASH",
	"TOKEN_DIVIDEQUAL",
	"TOKEN_PIPE",
	"TOKEN_OR",
	"TOKEN_AMPER",
	"TOKEN_AND",
	"TOKEN_LESS",
	"TOKEN_GREATER",
	"TOKEN_EQUAL",
	"TOKEN_DOT",
	"TOKEN_PERCENT",
	"TOKEN_BACKQUOTE",
	"TOKEN_LBRACE",
	"TOKEN_RBRACE",
	"TOKEN_EQEQUAL",
	"TOKEN_NOTEQUAL",
	"TOKEN_LESSEQUAL",
	"TOKEN_GREATEREQUAL",
	"TOKEN_TILDE",
	"TOKEN_POUND",
	"TOKEN_EXCLAMATION",
	"TOKEN_DOLLAR",

	"TOKEN_ERROR",
	"TOKEN_COMMENT",
	"TOKEN_BLANKLINE",
	"TOKEN_NONE",
};

const int Token::N_TOKEN_CHAR = 100;

Token::Token():
	m_start( NULL ),
	m_end( NULL ),
	m_type( E_TOKEN_NONE )
{}

void Token::Flush() {
	if ( m_start > m_end ) {
		m_type = E_TOKEN_ERROR;
		m_dummy[0] = '\0';
		return;
	}
	int quate;
	if ( m_type == E_TOKEN_STRING || m_type == E_TOKEN_STRING_SINGLEQUOTE ) {
		quate = 1;
	}
	else {
		quate = 0;
	}

	for ( const char *p = m_start + quate; p < m_end - quate; p ++ ) {
		m_dummy[p - m_start - quate ] = *p;
	}
	m_dummy[ m_end - m_start - 2*quate ] = '\0';

	return;
}

void Token::Assign( const char* start, const char* end, E_TOKEN_TYPE e ) {
	m_start = start;
	m_end = end;
	m_type = e;
	Flush();
}

const char* Token::GetTokenTypeString() const {
	return TokenTypeString[ m_type ];
}

int Token::GetIntValue() const {
	if ( m_type != E_TOKEN_INT ) return 0;

	int ret = 0;
	const char *p = m_dummy;
	while ( *p != '\0' ) {
		ret *= 10;
		ret += *p - '0';
		
		p++;
	} 

	return ret;
}

float Token::GetFloatValue() const {
	if ( m_type != E_TOKEN_FLOAT ) return 0.0f;

	float ret = 0.0f;
	const char* p = m_dummy;
	// integer part
	while ( IsDigit( p ) ) {
		ret *= 10;
		ret += *p - '0';
		p ++;
	}

	// assert( *p == '.' );
	// decimal part
	float factor = 1.0f / 10.0f;
	p ++;
	while ( IsDigit( p ) ) {
		ret += ( *p -'0' ) * factor;
		factor /= 10;

		p ++;
	}

	if ( *p == 'e' || *p == 'E' ) {
		p ++;

		int exponent = 0;
		bool negative = false;

		if ( *p == '+' ) p ++;
		else if ( *p == '-' ) {
			p ++;
			negative = true;
		}

		while ( IsDigit( p ) ) {
			exponent *= 10;
			exponent += *p - '0';
			p ++;
		}

		if ( !negative ) {
			for ( int i = 0; i < exponent; i ++ ) {
				ret *= 10.0f;
			}
		}
		else {
			for ( int i = 0; i < exponent; i ++ ) {
				ret /= 10.0f;
			}
		}
	}

	return ret;
}

const Token& Token::operator=( const Token& tok ) {
	m_start = tok.m_start;
	m_end = tok.m_end;
	m_type = tok.m_type;

	Flush();
	return *this;
}

/*****************************************************
  below is the implementation of lexer
  ***************************************************/
const int Lexer::N_FILE_NAME = 100;
const int Lexer::N_POSSTACK_SIZE = 100;

const char* Lexer::ErrorInfo[ E_ERROR_END ] = {
	"OK",
	"Initialize lexer failed, no file binded",
	"Not enough memory",
	"Quotes do not match",
	"Some symbols are out of my consideration",
	"Strange float number",
	"Lexer's positions stack over flow",
};

Lexer::Lexer():
	m_linenumber( -1 ),
	m_pfile( NULL ),
	m_prev( NULL ),
	m_next( NULL ),
	m_linebuff( NULL ),
	m_positionstack( NULL ),
	m_linenumberstack( NULL ),
	m_posidx( 0 ),
	m_bInited( false ),
	m_errtype( E_ERROR_NONE )
{
}

Lexer::Lexer( const char *filename):
	m_linenumber( -1 ),
	m_prev( NULL ),
	m_next( NULL ),
	m_linebuff( NULL ),
	m_positionstack( NULL ),
	m_linenumberstack( NULL ),
	m_posidx( 0 ),
	m_bInited( false ),
	m_errtype( E_ERROR_NONE )
{
	int len = strlen( filename );
	m_pfile = new LFile( filename );
}

Lexer::~Lexer() {
#define RELEASEPOINTER( pointer ) \
	if ( pointer ) { \
		delete pointer; \
		pointer = NULL; \
	}
	RELEASEPOINTER( m_pfile );
	RELEASEPOINTER( m_prev );
	RELEASEPOINTER( m_next );
#undef RELEASEPOINTER
#define RELEASEPOINTER( pointer ) \
	if ( pointer ) { \
		delete[] pointer; \
		pointer = NULL; \
	}
	RELEASEPOINTER( m_positionstack );
	RELEASEPOINTER( m_linenumberstack );
#undef RELEASEPOINTER
}

void Lexer::BindFile( const char *filename ) {
	if ( m_pfile ) {
		delete m_pfile;
	}
	m_pfile = new LFile( filename );
}

void Lexer::Initialize() {
	if ( !m_bInited ) {
		m_bInited = true;
		m_prev = new Token();
		m_next = new Token();
		m_positionstack = new int[ N_POSSTACK_SIZE ];
		m_linenumberstack = new int[ N_POSSTACK_SIZE ];
	}
	
	if ( m_prev == NULL || m_next == NULL ) {
		m_errtype = E_ERROR_MEMORY;
		return;
	}

	if ( m_pfile == NULL ) {
		m_errtype = E_ERROR_INITFAILED;
		m_next->Assign( NULL, NULL, E_TOKEN_ERROR );
		return;
	}

	m_linebuff = m_pfile->GetNextLine();
	m_linenumber = 1;

	m_next->Assign( m_linebuff, m_linebuff, E_TOKEN_NONE );
	MoveNext();

	m_posidx = 0;
}

bool IsAlpha( const char* p ) {
	return isalpha( *p );
}

bool IsPunct( const char* p ) {
	return ispunct( *p );
}

bool IsDigit( const char* p ) {
	return isdigit( *p );
}

static inline bool IsBlack( const char* p ) {
	// 13 is for linux
	// '\r' is for WII platform
	if ( *p == ' ' || *p == '\t' || *p == 10 || *p == '\n' || *p == 13 || *p == '\r' ) return true;
	else return false;
}

void Lexer::MoveNext() {
	(*m_prev) = (*m_next);
	const char* p = m_next->m_end;

	// find the first useful char 
	while ( true ) {
		if ( !IsBlack( p ) ) {
			break;
		}
		while ( IsBlack( p ) ) {
			if ( *p == '\n' ) {
				p = m_linebuff = m_pfile->GetNextLine();
				m_linenumber ++;

				if ( p == NULL ) {
					// ok, no token anymore, at the end of the file
					m_next->Assign( NULL, NULL, E_TOKEN_END );
					return;
				}
				continue;
			}
			p ++;
		}
	}

	const char* q = p;

	if ( IsAlpha( p ) || *p == '_' ) {
		// identity
		q = p;
		do { 
			q ++;
		} while ( IsDigit( q ) || IsAlpha( q ) || *q == '_' );
		m_next->Assign( p, q, E_TOKEN_IDENTITY );
		return;
	}
	else if ( IsDigit( p ) ) {
		q = p;
		// integer part
		do {
			q ++;
		} while ( IsDigit( q ) );

		// decimal part
		if ( *q == '.' ) {
			if ( !IsDigit( q + 1 ) ) {
				m_next->Assign( NULL, NULL, E_TOKEN_ERROR );
				m_errtype = E_ERROR_FLOAT;
				return;
			}

			do {
				q ++;
			} while ( IsDigit( q ) );

			if ( *q == 'e' || *q == 'E' ) {
				q ++;
				if ( *q == '+' || *q == '-' ) {
					q ++;
				}

				if ( !IsDigit( q ) ) {
					m_next->Assign( p, q, E_TOKEN_FLOAT );
					m_errtype = E_ERROR_FLOAT;
					return;
				}

				while ( IsDigit( q ) ) {
					q ++;
				}
			}
			m_next->Assign( p, q, E_TOKEN_FLOAT );
			return;
		}
		else {
			// no decimal part
			m_next->Assign( p, q, E_TOKEN_INT );
			return;
		}
	}
	else if ( IsPunct( p ) ) {
		/*
		if ( *p == '/' && *(p+1) == '/' ) { 
			// '//' form comment
			q = p + 2;
			while ( *q != '\n' ) {
				q ++;
			}

			m_next->Assign( p, q, E_TOKEN_COMMENT );
			return;
		}
		else if ( *p == '/' && *(p+1) == '*' ) {
			// '/*' form comment
			q = p + 2;
			do {
				if ( *q == '\n' ) {
					q = m_linebuff = m_pfile->GetNextLine();
					m_linenumber ++;

					if ( q == NULL ) {
						// ok, no token anymore, at the end of the file
						m_next->Assign( NULL, NULL, E_TOKEN_END );
						return;
					}
					continue;
				}
				else if ( *q == '*' && *(q+1) == '/' ) {
					break;
				}
				q ++;
			} while ( true );

			m_next->Assign( p, q+2, E_TOKEN_COMMENT );
			return;
		}*/
		if ( *p == '"' ) {
			// next token is a string
			q = p + 1;
			while ( *q != '"' ) {
				if ( *q == '\n' ) {
					m_errtype = E_ERROR_STRING;
					m_next->Assign( NULL, NULL, E_TOKEN_ERROR );
					return;
				}
				else if ( *q == '\\' ) {
					q ++;
				}
				q ++;
			}

			m_next->Assign( p, q+1, E_TOKEN_STRING );
			return;
		}
		else if ( *p == '\'' ) {
			// single quoted string
			q = p + 1;
			while ( *q != '\'' ) {
				if ( *q == '\n' ) {
					m_errtype = E_ERROR_STRING;
					m_next->Assign( NULL, NULL, E_TOKEN_ERROR );
					return;
				}
				else if ( *q == '\\' ) {
					q ++;
				}
				q ++;
			}

			m_next->Assign( p, q+1, E_TOKEN_STRING_SINGLEQUOTE );
			return;
		}

		q = p + 1;
#define CODE_PART( first, second, first_type, second_type ) \
		case first: \
			if ( *q == second ) { \
				m_next->Assign( p, q+1, E_TOKEN_##second_type ); \
				return; \
			} \
			else { \
				m_next->Assign( p, q, E_TOKEN_##first_type ); \
				return; \
			} \
			break

		// use the above macro to deal with +=, -=, *=, /=, ==
		switch ( *p ) {
			case '=':
				if ( *q == '=' ) {
					m_next->Assign( p, q+1, E_TOKEN_EQEQUAL );
					return;
				}
				else {
					m_next->Assign( p, q, E_TOKEN_EQUAL );
					return;
				}
				break;
			CODE_PART( '+', '=', PLUS, PLUSEQUAL );
			CODE_PART( '-', '=', MINUS, MINUSEQUAL );
			CODE_PART( '*', '=', STAR, MUlTIPLYEQUAL );
			CODE_PART( '/', '=', SLASH, DIVIDEQUAL );
			CODE_PART( '<', '=', LESS, LESSEQUAL );
			CODE_PART( '>', '=', GREATER, GREATEREQUAL );
			CODE_PART( '!', '=', EXCLAMATION, NOTEQUAL ); 
			CODE_PART( '&', '&', AMPER, AND );
			CODE_PART( '|', '|', PIPE, OR );
#undef CODE_PART
#define CODE_PART( symbol, type ) \
		case symbol: \
			m_next->Assign( p, q, E_TOKEN_##type ); \
			break 

			CODE_PART( '\\', BACKSLASH );
			CODE_PART( '(', LPAR );
			CODE_PART( ')', RPAR );
			CODE_PART( '[', LSQB );
			CODE_PART( ']', RSQB );
			CODE_PART( '{', LBRACE );
			CODE_PART( '}', RBRACE );
			CODE_PART( ';', SEMI );
			CODE_PART( ',', COMMA );
			CODE_PART( '.', DOT );
			CODE_PART( ':', COLON );
			CODE_PART( '%', PERCENT );
			CODE_PART( '~', TILDE );
			CODE_PART( '#', POUND );
			CODE_PART( '$', DOLLAR );
#undef CODE_PART
			default:
				m_next->Assign( NULL, NULL, E_TOKEN_ERROR );
				m_errtype = E_ERROR_MISSINGSYMBOL;
				break;
		}
	}
	else {
		// Error, some symbols out of control
		m_next->Assign( NULL, NULL, E_TOKEN_ERROR );
		m_errtype = E_ERROR_MISSINGSYMBOL;
	}
	return;
}

void Lexer::PushPos() {
	if ( m_positionstack == NULL || m_linenumberstack == NULL ) {
		m_errtype = E_ERROR_MEMORY;
		return;
	}
	if ( m_posidx >= N_POSSTACK_SIZE ) {
		m_errtype = E_ERROR_STACKOVERFLOW;
		return;
	}

	/*
	int pos = m_pfile->Tell();
	// because we use the getline() function, so the file position is already at 
	// the end of the line, we should get the accurate pos
	// something wrong here, we can tell the pos and seek back,
	// but we don't know the the accurate number of the pos
	const char *p = m_next->m_start;
	if ( p != NULL ) {
		const char* p_end = &m_linebuff[ strlen( m_linebuff ) - 1 ];
		while ( p <= p_end ) {
			p ++;
			pos --;
		}
	}
	*/
	int pos = 0;
	const char *p = m_next->m_start;
	if ( p != NULL ) {
		const char* p_start = m_linebuff;
		while ( p_start < p ) {
			pos ++;
			p_start ++;
		}
	}

	m_positionstack[ m_posidx ] = pos;
	m_linenumberstack[ m_posidx ] = m_linenumber;
	m_posidx ++;
}

void Lexer::PopPos() {
	if ( m_positionstack == NULL || m_linenumberstack == NULL ) {
		m_errtype = E_ERROR_MEMORY;
		return;
	}
	if ( m_posidx < 1 ) {
		m_errtype = E_ERROR_STACKOVERFLOW;
		return;
	}

	m_posidx--;
	//m_pfile->Seek( m_positionstack[ m_posidx ] );
	//m_linenumber = m_linenumberstack[ m_posidx ];
}

void Lexer::GotoTopPos() {
	if ( m_positionstack == NULL || m_linenumberstack == NULL ) {
		m_errtype = E_ERROR_MEMORY;
		return;
	}
	if ( m_posidx < 1 ) {
		m_errtype = E_ERROR_STACKOVERFLOW;
		return;
	}

	// recover the position
	//m_pfile->Seek( m_positionstack[ m_posidx-1 ] );
	m_pfile->Seek( 0 );
	m_linenumber = m_linenumberstack[ m_posidx-1 ];
	int x = m_linenumber;
	while ( x -- ) {
		m_linebuff = m_pfile->GetNextLine();
	}
	const char* p = m_linebuff;
	for ( int i = 0; i < m_positionstack[ m_posidx-1 ]; i ++ ) {
		p ++;
	}

	// here we should recover the two token pointer
	//m_linebuff = m_pfile->GetNextLine();
	(*m_prev) = (*m_next);
	//m_next->Assign( m_linebuff, m_linebuff, E_TOKEN_NONE );
	m_next->Assign( p, p, E_TOKEN_NONE );
	MoveNext();
}

bool Lexer::PosStackEmpty() const {
	return m_posidx == 0;
}

/********************************************************************
  below is the test code for lexer
  *******************************************************************/

//#define _CS_LEXER_TEST_
#ifdef _CS_LEXER_TEST_
int main( int argc, char* argv[] ) {
	if ( argc <= 1 ) return 0;

	const char* filename = argv[1];

	Lexer* lex = new Lexer();
	lex->BindFile( filename );
	lex->Initialize();

	const Token* tok = lex->GetNextTokenPointer();
	
	while ( tok->GetType() != E_TOKEN_END ) {
		if ( tok->GetType() == E_TOKEN_ERROR ) {
			printf( "ERROR %s, %d\n", lex->GetErrorInfo(), lex->GetErrorLine() );
			break;
		}
		printf( "%s:\t\t%s", tok->GetTokenTypeString(), tok->GetCharValue() );
		if ( tok->GetType() == E_TOKEN_INT ) {
			printf( "\t\t%d\n", tok->GetIntValue() );
		}
		else if ( tok->GetType() == E_TOKEN_FLOAT ) {
			printf( "\t\t%f\n", tok->GetFloatValue() );
		}
		else {
			printf( "\n" );
		}
		
		lex->MoveNext();
		tok = lex->GetNextTokenPointer();
	}
	delete lex;
	getchar();
}
#endif //_CS_LEXER_TEST_
