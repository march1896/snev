#include "parser.h"
#include "variable.h"
#include "function.h"
#include <cstdio>
#include <cstring>

#undef _DEBUG

/****************************************************************************
 Parser implementation
****************************************************************************/
const char* Parser::ErrorInfo[] = {
	"OK",
	"Undefined Keyword",
	"Strange variable format",
	"Function undefined",
	"Function parentheses do not match",
	"Function params error",
	"Strange function format",
	"Expression start error",
	"Expression used undefined variable",
	"Expression can not be parsed",
	"String can only support operator+",
	"Operator Undefined",
	"Two operands types are different",
	"Opreator && error",
	"opreator error",
	"Missing Operand before right brace",
	"Expression parathese do not match",
	"Expression before '='",
	"Statementlist must be quoted with braces",
	"Statementlist braces do not match",
	"Can not break, no while statement",
};
	
Parser::Parser() :
	m_eParErr( E_PAERR_OK )
{
	m_pLexer = new Lexer();
	m_pRuntime = new Runtime();
}

Parser::~Parser() {
	if ( m_pLexer ) {
		delete m_pLexer;
		m_pLexer = NULL;
	}

	if ( m_pRuntime ) {
		delete m_pRuntime;
		m_pRuntime = NULL;
	}
}

void Parser::Initialize( const char* filename ) {
	m_pLexer->BindFile( filename );
	m_pLexer->Initialize();
	m_pRuntime->Initialize();
}

bool Parser::SeekPos( const char* def, const char* name ) {
	// TODO:
	return true;
}

void Parser::PreParse() {
	// TODO:
	return;
}

static void print( const Variable* var ) {
	switch ( var->GetType() ) {
		case Variable::E_NULL:
			printf( "NULL\n" );
			break;
		case Variable::E_INT:
			printf( "%d\n", var->GetIntValue() );
			break;
		case Variable::E_FLOAT:
			printf( "%f\n", var->GetFloatValue() );
			break;
		case Variable::E_STRING:
			printf( "%s\n", var->GetStringValue().c_str() );
			break;
	}
	return;
}

void Parser::Parse() {
	//Factor* fac = new Factor( this );
	//fac->Parse();

	//printf( "%d", fac->GetType() );
	StatementList* sml = new StatementList( this );
	GetRuntime()->PushVarTable();
	sml->Parse();
	GetRuntime()->PopVarTable();

	if ( HasError() ) {
		printf( "error: %s\tline: %d\ttoken: %s\n", 
			GetErrorString(), GetLexer()->GetErrorLine(), GetLexer()->GetPrevTokenPointer()->GetCharValue() );
	}
	else {
		printf( "parse over\n" );
	}
	delete sml;

	/*
	Expression* expr = new Expression( this );
	expr->Parse();

	if ( !HasError() ) {
		Variable* var = expr->GetValue();
		printf( "\nresult:\t" );
		print( var );
		printf( "\n" );
	}
	else {
		printf( "expression error: %s\n", GetErrorString() );
	}

	expr->Clear();
	expr->Parse();
	if ( !HasError() ) {
		Variable* var = expr->GetValue();
		printf( "\nresult:\t" );
		print( var );
		printf( "\n" );
	}
	else {
		printf( "expression error: %s\n", GetErrorString() );
	}
	delete expr;
	*/

	return;
}

/****************************************************************************
 factor implementation
****************************************************************************/

void Factor::Clear() {
	if ( m_eType == E_FACTOR_VARIABLE_TEMP ) {
		if ( m_pVar != NULL ) {
			delete m_pVar;
			m_pVar = NULL;
		}
	}
}

Factor::~Factor() {
	Clear();
}

void Factor::Parse() {
	Lexer* lex = GetLexer();
	Runtime* rt = GetRuntime();
	Token tok;
	const Token* ptok;
	E_TOKEN_TYPE t_type;

	tok = lex->GetNextToken();
	t_type = tok.GetType();
	//printf( "factor token type: %s\n", tok.GetTokenTypeString() );
	if ( t_type == E_TOKEN_IDENTITY ) {
		// just for key word, because functions and variables have special prefix
		const char* str = tok.GetCharValue();
		if ( strcmp( str, "if" ) == 0 || strcmp( str, "elif" ) == 0 || strcmp( str, "else" ) == 0 ) {
			m_eType = E_FACTOR_KEYWORD;
		}
		else if ( strcmp( str, "while" ) == 0 || strcmp( str, "break" ) == 0 ) {
			m_eType = E_FACTOR_KEYWORD;
		}
		else {
			GetParser()->SetError( Parser::E_PAERR_KEYWORD_UNDEF );
		}
	}
	else if ( t_type == E_TOKEN_STRING || t_type == E_TOKEN_FLOAT || t_type == E_TOKEN_INT ) {
		// temp variable 
		m_eType = E_FACTOR_VARIABLE_TEMP;
		switch ( t_type ) {
			case E_TOKEN_STRING:
				m_pVar = new Variable( tok.GetCharValue() );
				break;
			case E_TOKEN_FLOAT:
				m_pVar = new Variable( tok.GetFloatValue() );
				break;
			case E_TOKEN_INT:
				m_pVar = new Variable( tok.GetIntValue() );
				break;
		}
	}
	else if ( t_type == E_TOKEN_DOLLAR ) {
		// '$variable' is a variable
		lex->MoveNext();
		ptok = lex->GetNextTokenPointer();

		if ( ptok->GetType() == E_TOKEN_IDENTITY ) {
			m_pVar = rt->GetVariable( ptok->GetCharValue() );

			if ( m_pVar == NULL ) {
				m_eType = E_FACTOR_VARIABLE_UNDEF;
				m_pVar = rt->AddVariable( ptok->GetCharValue(), Variable() );
			}
			else {
				m_eType = E_FACTOR_VARIABLE;
			}
		}
		else {
			// error
			GetParser()->SetError( Parser::E_PAERR_VARIABLE_DEF );
		}
	}
	else if ( t_type == E_TOKEN_AMPER ) {
		do {
			// '&function( para1, para2, para3 )' is a function
			lex->MoveNext();
			ptok = lex->GetNextTokenPointer();

			if ( ptok->GetType() == E_TOKEN_IDENTITY ) {
				Function* func = rt->GetFunction( std::string( ptok->GetCharValue() ) );

				if ( func == NULL ) {
					GetParser()->SetError( Parser::E_PAERR_FUNCTION_UNDEF );
					break;
				}

				lex->MoveNext();
				ptok = lex->GetNextTokenPointer();

				if ( ptok->GetType() != E_TOKEN_LPAR ) {
					// '('
					GetParser()->SetError( Parser::E_PAERR_FUNCTION_PAR );
					break;
				}

				lex->MoveNext();
				ptok = lex->GetNextTokenPointer();

				while ( true ) {
					Expression* express = new Expression( GetParser() );
					express->Parse();
					if ( GetParser()->HasError() ) {
						break;
					}
					else {
						func->AddParam( *(express->GetValue()) );
					}
					delete express;

					//lex->MoveNext();
					ptok = lex->GetNextTokenPointer();

					if ( ptok->GetType() == E_TOKEN_RPAR ) {
						// ')'
						//lex->MoveNext();
						break;
					}
					else if ( ptok->GetType() == E_TOKEN_COMMA ) {
						lex->MoveNext();
						ptok = lex->GetNextTokenPointer();
					}
					else {
						GetParser()->SetError( Parser::E_PAERR_FUNCTION_PARAMS_OTHER );
					}
				}
				func->Compute();
				func->ClearParam();
				m_pVar = new Variable( func->GetReturnValue() );
				m_eType = E_FACTOR_FUNCTION;
			}
			else {
				GetParser()->SetError( Parser::E_PAERR_FUNCTION_DEF );
			}
		} while ( 0 );
	}
	else if ( t_type >= E_TOKEN_IDENTITY && t_type < E_TOKEN_DOLLAR ) {
		m_eType = E_FACTOR_SYMBOL;
	}
}

/****************************************************************************
 expression implementation

parse: compute the arthmetic expression
****************************************************************************/

Expression::~Expression() {
	Clear();
}

void Expression::Clear() {
	std::vector< Variable* >::iterator v_itr = m_VarStack.begin();
	for ( ; v_itr != m_VarStack.end(); v_itr ++ ) {
		delete (*v_itr);
	}
	m_VarStack.clear();
	
	m_SymStack.clear();
	m_bIsLastVariable = false;
}

bool Expression::CheckLegalSymbol( E_TOKEN_TYPE e ) {
	switch ( e ) {
		case E_TOKEN_PLUS:
		case E_TOKEN_MINUS:
		case E_TOKEN_STAR:
		case E_TOKEN_SLASH:
		case E_TOKEN_AND:
		case E_TOKEN_OR:
		case E_TOKEN_LESS:
		case E_TOKEN_LESSEQUAL:
		case E_TOKEN_GREATER:
		case E_TOKEN_GREATEREQUAL:
		case E_TOKEN_EQEQUAL:
		case E_TOKEN_LPAR:
		case E_TOKEN_RPAR:
			return true;
			break;
		default:
			return false;
	}
	return false;
}

void Expression::Parse() {
	Factor* fac = new Factor( GetParser() );
	Lexer* lex = GetLexer();
	Runtime* rt = GetRuntime();
	m_bIsSingleVariable = false;

	fac->Parse();

	if ( GetParser()->HasError() ) {
		delete fac;
		return;
	}

	// deal with the first input
	if ( fac->GetType() == E_FACTOR_VARIABLE || fac->GetType() == E_FACTOR_VARIABLE_TEMP ) {
		PushVar( *(fac->GetValue()) );
		m_bIsLastVariable = true;
		if ( fac->GetType() == E_FACTOR_VARIABLE ) {
			m_bIsSingleVariable = true;
		}
	}
	else if ( fac->GetType() == E_FACTOR_SYMBOL ) {
		if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_LPAR ) {
			PushSym( E_TOKEN_LPAR );
			m_bIsLastVariable = false;
		}
		else {
			GetParser()->SetError( Parser::E_PAERR_EXP_START );
		}
	}
	else if ( fac->GetType() == E_FACTOR_VARIABLE_UNDEF ) {
		PushVar( *(fac->GetValue()) );
		m_bIsLastVariable = true;
		// GetParser()->SetError( Parser::E_PAERR_EXP_VARUNDEF );
		m_bIsSingleVariable = true;
	}
	else if ( fac->GetType() == E_FACTOR_FUNCTION ) {
		PushVar( *(fac->GetValue() ) );
		m_bIsLastVariable = true;
	}
	else {
		// error, expression can only start with variable or '('
		GetParser()->SetError( Parser::E_PAERR_EXP_START );
		delete fac;
		return;
	}

	fac->Clear();
	lex->MoveNext();
	
	Variable var1, var2;
	Variable result;
	E_TOKEN_TYPE op;
#define PROCESS_STACK \
	if ( m_VarStack.size() < 2 ) break; \
	var2 = PopVar(); \
	var1 = PopVar(); \
	op = PopSym();  \
	result = Calculate( var1, var2, op ); \
	PushVar( result ); \
	op = TopSym();


	while ( true ) {
		// pre judge if the expression is end
		const Token* ptok = lex->GetNextTokenPointer();
		if ( ptok->GetType() == E_TOKEN_END ) {
			break;
		}
		if ( ptok->GetType() == E_TOKEN_LBRACE || ptok->GetType() == E_TOKEN_RBRACE ) {
			while ( true ) {
				PROCESS_STACK;
			}
			if ( m_VarStack.size() != 1 || m_SymStack.size() != 0 ) {
				GetParser()->SetError( Parser::E_PAERR_EXP_UNCALCABLE );
			}
			break;
		}
		//printf( "%s\n", lex->GetNextTokenPointer()->GetTokenTypeString() );
		
		if ( m_bIsLastVariable ) {
			if ( ptok->GetType() == E_TOKEN_DOLLAR || ptok->GetType() == E_TOKEN_AMPER || ptok->GetType() == E_TOKEN_IDENTITY ) {
				while ( true ) {
					PROCESS_STACK;
				}
				#ifdef _DEBUG
				printf( "VarStack Size: %d\nSymStack Size: %d\n", m_VarStack.size(), m_SymStack.size() );
				#endif

				if ( m_VarStack.size() != 1 || m_SymStack.size() != 0 ) {
					GetParser()->SetError( Parser::E_PAERR_EXP_UNCALCABLE );
				}
				break;
			}
		}


		fac->Parse();
		// precess error
		if ( fac->GetType() == E_FACTOR_VARIABLE_UNDEF ) {
			GetParser()->SetError( Parser::E_PAERR_EXP_VARUNDEF );
			break;
		}

		if ( fac->GetType() == E_FACTOR_VARIABLE || fac->GetType() == E_FACTOR_VARIABLE_TEMP ) {
			if ( m_bIsLastVariable  ) {
				// variable follows variable, the expression is end
				while ( true ) {
					PROCESS_STACK;
				}
				#ifdef _DEBUG
				printf( "VarStack Size: %d\nSymStack Size: %d\n", m_VarStack.size(), m_SymStack.size() );
				#endif

				if ( m_VarStack.size() != 1 || m_SymStack.size() != 0 ) {
					GetParser()->SetError( Parser::E_PAERR_EXP_UNCALCABLE );
				}
				break;
			}

			Variable* vvv = fac->GetValue();
			PushVar( *(fac->GetValue()) );
			m_bIsLastVariable = true;
		}
		else if ( fac->GetType() == E_FACTOR_SYMBOL ) {
			if ( !CheckLegalSymbol( lex->GetNextTokenPointer()->GetType() ) ) {
				// not valid symbol comes, assume the expression is end
				while ( true ) {
					PROCESS_STACK;
				}

				#ifdef _DEBUG
				printf( "VarStack Size: %d\nSymStack Size: %d\n", m_VarStack.size(), m_SymStack.size() );
				#endif

				if ( m_VarStack.size() != 1 || m_SymStack.size() != 0 ) {
					GetParser()->SetError( Parser::E_PAERR_EXP_UNCALCABLE );
				}
				break;
			}
			else if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_LPAR ) {
				if ( m_bIsLastVariable ) {
					// left parenthese follows operand, assume expression is end
					while ( true ) {
						PROCESS_STACK;
					}
					#ifdef _DEBUG
					printf( "VarStack Size: %d\nSymStack Size: %d\n", m_VarStack.size(), m_SymStack.size() );
					#endif

					if ( m_VarStack.size() != 1 || m_SymStack.size() != 0 ) {
						GetParser()->SetError( Parser::E_PAERR_EXP_UNCALCABLE );
					}
					break;
				}
				else {
					PushSym( lex->GetNextTokenPointer()->GetType() );
				}
			}
			else if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_RPAR ) {
				if ( !m_bIsLastVariable ) {
					// right parenthese follows a oparator 
					GetParser()->SetError( Parser::E_PAERR_EXP_PARERROR );
					break;
				}

				if ( TopSym() == E_TOKEN_NONE ) {
					break;
				}
				{
					while ( TopSym() != E_TOKEN_LPAR ) {
						PROCESS_STACK;
					}
					if ( TopSym() == E_TOKEN_LPAR ) {
						PopSym();
					}
					else {
						GetParser()->SetError( Parser::E_PAERR_EXP_PAREUNMATCH );
					}
					
					#ifdef _DEBUG
					printf( "VarStack Size: %d\nSymStack Size: %d\n", m_VarStack.size(), m_SymStack.size() );
					#endif

					/*
					if ( m_VarStack.size() != 1 || m_SymStack.size() != 0 ) {
						GetParser()->SetError( Parser::E_PAERR_EXP_UNCALCABLE );
					}
					*/
					//lex->MoveNext();
					//break;
				}
			}
			else if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_EQUAL ) {
				if ( m_bIsSingleVariable ) {
					break;
				}
				else {
					// '=' left should not be a expression 
					GetParser()->SetError( Parser::E_PAERR_ASSIGNMENT_LEFT );
				}
			}
			else {
				// normal symbols
				op = TopSym();
				while ( SymLessThen( lex->GetNextTokenPointer()->GetType(), op ) ) {
					PROCESS_STACK;
				}

				PushSym( lex->GetNextTokenPointer()->GetType() );
				m_bIsLastVariable = false;
				m_bIsSingleVariable = false;
			}
		}

		fac->Clear();
		lex->MoveNext();
#undef PROCESS_STACK
	}
	#ifdef _DEBUG
	printf( "expression end, next token : %s\n", lex->GetNextTokenPointer()->GetTokenTypeString() );
	#endif
	delete fac;

	return;
}

int Expression::SymWeight[ E_TOKEN_END ] = {};
void Expression::InitSymWeight() {
	Expression::SymWeight[ E_TOKEN_PLUS ] = 3;
	Expression::SymWeight[ E_TOKEN_MINUS ] = 3;
	Expression::SymWeight[ E_TOKEN_STAR ] = 4;
	Expression::SymWeight[ E_TOKEN_SLASH ] = 4;
	Expression::SymWeight[ E_TOKEN_AND ] = 1;
	Expression::SymWeight[ E_TOKEN_OR ] = 1;
	Expression::SymWeight[ E_TOKEN_LESS ] = 2;
	Expression::SymWeight[ E_TOKEN_LESSEQUAL ] = 2;
	Expression::SymWeight[ E_TOKEN_GREATER ] = 2;
	Expression::SymWeight[ E_TOKEN_GREATEREQUAL ] = 2;
	Expression::SymWeight[ E_TOKEN_EQEQUAL ] = 2;
	Expression::SymWeight[ E_TOKEN_LPAR ] = 5;
	Expression::SymWeight[ E_TOKEN_RPAR ] = 0;
}

bool Expression::SymLessThen( E_TOKEN_TYPE sym1, E_TOKEN_TYPE sym2 ) {
	//printf( "symbweight %d, %d\n", SymWeight[ sym1 ], SymWeight[ sym2 ] );
	if ( sym2 == E_TOKEN_LPAR ) return false;
	return SymWeight[ sym1 ] < SymWeight[ sym2 ];
}

Variable Expression::Calculate( const Variable &var1, const Variable &var2, E_TOKEN_TYPE op ) {
	if ( !CheckLegalSymbol( op ) || op == E_TOKEN_LPAR || op == E_TOKEN_RPAR ) {
		GetParser()->SetError( Parser::E_PAERR_EXP_OPUNDEF );
		// return NULL value
		return Variable();
	}

	if ( var1.GetType() - var2.GetType() > 1 || var1.GetType() - var2.GetType() < -1 ) {
		// printf ("types %d %d\n", var1.GetType(), var2.GetType() );
		GetParser()->SetError( Parser::E_PAERR_EXP_OPTYPEDIFF );
		return Variable();
	}
	else if ( var1.GetType() == Variable::E_NULL || var2.GetType() == Variable::E_NULL ) {
		GetParser()->SetError( Parser::E_PAERR_EXP_OPUNDEF );
		return Variable();
	} 
	else if ( var1.GetType() == Variable::E_STRING && op != E_TOKEN_PLUS ) {
		GetParser()->SetError( Parser::E_PAERR_EXP_STRINGOPERATION );
		return Variable();
	}

	bool ret;
	switch ( op ) {
		case E_TOKEN_PLUS:
			return var1 + var2;
			break;
		case E_TOKEN_MINUS:
			return var1 - var2;
			break;
		case E_TOKEN_STAR:
			return var1 * var2;
			break;
		case E_TOKEN_SLASH:
			return var1 / var2;
			break;
		case E_TOKEN_AND:
			ret = true;
			if ( var1.GetType() == Variable::E_INT ) 			ret = ret && ( var1.GetIntValue() != 0 );
			else if ( var1.GetType() == Variable::E_FLOAT ) 	ret = ret && ( var1.GetFloatValue() != 0.0 );
			else {
				GetParser()->SetError( Parser::E_PAERR_EXP_OPAND );
				return Variable();
			}

			if ( var2.GetType() == Variable::E_INT ) 			ret = ret && ( var2.GetIntValue() != 0 );
			else if ( var2.GetType() == Variable::E_FLOAT ) 	ret = ret && ( var2.GetFloatValue() != 0.0 );
			else {
				GetParser()->SetError( Parser::E_PAERR_EXP_OPAND );
				return Variable();
			}

			if ( ret ) {
				return Variable( 1 );
			}
			else {
				return Variable( 0 );
			}
			break;
		case E_TOKEN_OR:
			ret = false;
			if ( var1.GetType() == Variable::E_INT ) 			ret = ret || ( var1.GetIntValue() != 0 );
			else if ( var1.GetType() == Variable::E_FLOAT ) 	ret = ret || ( var1.GetFloatValue() != 0.0 );
			else {
				GetParser()->SetError( Parser::E_PAERR_EXP_OPAND );
				return Variable();
			}

			//printf( "var2's type and value: %d %d\n", var2.GetType(), var2.GetIntValue() );
			if ( var2.GetType() == Variable::E_INT ) 			ret = ret || ( var2.GetIntValue() != 0 );
			else if ( var2.GetType() == Variable::E_FLOAT ) 	ret = ret || ( var2.GetFloatValue() != 0.0 );
			else {
				GetParser()->SetError( Parser::E_PAERR_EXP_OPAND );
				return Variable();
			}

			if ( ret ) {
				return Variable( 1 );
			}
			else {
				return Variable( 0 );
			}
			break;
		case E_TOKEN_LESS:
			if ( var1 < var2 ) return Variable( 1 );
			else return Variable( 0 );
			break;
		case E_TOKEN_LESSEQUAL:
			if ( var1 <= var2 ) return Variable( 1 );
			else return Variable( 0 );
			break;
		case E_TOKEN_GREATER:
			if ( var1 > var2 ) return Variable( 1 );
			else return Variable( 0 );
			break;
		case E_TOKEN_GREATEREQUAL:
			if ( var1 >= var2 ) return Variable( 1 );
			else return Variable( 0 );
		case E_TOKEN_EQEQUAL:
			if ( var1 == var2 ) return Variable( 1 );
			else return Variable( 0 );
		default:
			GetParser()->SetError( Parser::E_PAERR_EXP_OPOTHER );
			return Variable();
	}
}

Variable* Expression::GetValue() {
	if ( m_VarStack.size() == 1 ) {
		return m_VarStack.front();
	}
	return NULL;
}

void Expression::PushVar( const Variable& var ) {
	m_VarStack.push_back( new Variable( var ) );
}

Variable Expression::PopVar() {
	if ( m_VarStack.empty() ) {
		return Variable();
	}
	Variable* var = m_VarStack.back();
	m_VarStack.pop_back();
	Variable ret( *var );
	delete var;
	return ret;
}

Variable Expression::TopVar() const {
	if ( m_VarStack.empty() ) {
		return Variable();
	}
	return Variable( *(m_VarStack.back() ) );
}

void Expression::PushSym( E_TOKEN_TYPE sym ) {
	m_SymStack.push_back( sym );
}

E_TOKEN_TYPE Expression::PopSym() {
	if ( m_SymStack.empty() ) {
		return E_TOKEN_NONE;
	}
	E_TOKEN_TYPE ret = m_SymStack.back();
	m_SymStack.pop_back();
	return ret;
}

E_TOKEN_TYPE Expression::TopSym() const {
	if ( m_SymStack.empty() ) {
		return E_TOKEN_NONE;
	}
	return m_SymStack.back();
}

/****************************************************************************
 Statement implementation
****************************************************************************/
Statement::Statement( Parser* par ) :
	Parsable( par ),
	m_eType( E_STATEMENT_NONE )
{
	m_pVar = new Variable();
}

Statement::~Statement() {
	if ( m_pVar ) {
		delete m_pVar;
		m_pVar = NULL;
	}
}

void Statement::Clear() {
	m_eType = E_STATEMENT_NONE;
	(*m_pVar) = Variable();
}

bool Statement::IsVariableTrue( const Variable* var ) {
	switch ( var->GetType() ) {
		case Variable::E_NULL:
			return false;
			break;
		case Variable::E_INT:
			if ( var->GetIntValue() != 0 ) { 
				return true;
			}
			else {
				return false;
			}
			break;
		case Variable::E_FLOAT:
			if ( var->GetFloatValue() != 0.0 ) {
				// TODO: use gentle float compare
				return true;
			}
			else {
				return false;
			}
			break;
		case Variable::E_STRING:
			if ( strcmp( var->GetStringValue().c_str(), "" ) == 0 ) {
				return false;
			}
			else {
				return true;
			}

			break;
	}
	return false;
}

void Statement::Parse() {
	Lexer* lex = GetLexer();
	Runtime* rt = GetRuntime();

	Token tok;
	const Token* ptok;

	Expression* 	expr = new Expression( GetParser() );
	Factor* 		fac = new Factor( GetParser() );
	StatementList* 	sml = new StatementList( GetParser() );
	Statement* 		stm = new Statement( GetParser() );

	ptok = lex->GetNextTokenPointer();

	E_TOKEN_TYPE e_ttype = ptok->GetType();
	if ( e_ttype == E_TOKEN_LBRACE ) {
		// TODO: this is a code part, includes many statement
		sml->Clear();
		rt->PushVarTable();
		sml->Parse();
		rt->PopVarTable();

		if ( sml->IsBreaked() ) {
			m_eType = E_STATEMENT_BREAK;
		}
	}
	else if ( e_ttype == E_TOKEN_IDENTITY ) {
		// 'if-elif-else' or 'while' statement
		fac->Parse();
		if ( fac->GetType() != E_FACTOR_KEYWORD ) {
			GetParser()->SetError( Parser::E_PAERR_KEYWORD_UNDEF );
		}
		else {
			// process if-elif-else and while-break statement
			
			if ( strcmp( lex->GetNextTokenPointer()->GetCharValue(), "if" ) == 0 ) {
				// assume if-else statement
				// NB. after fac->Parse(), lex->next stops at the factor's last token, 
				// after expr->Parse(), stm->Parse(), sml->Parse() , lex->next stops at the token follows the expression
				m_eType = E_STATEMENT_IFELSE;
				bool IsProcessed = false;

				lex->MoveNext();

				expr->Clear();
				expr->Parse();

				if ( !GetParser()->HasError() ) {
					Variable* var = expr->GetValue();
					sml->Clear();

					if ( IsVariableTrue( var ) ) {
						rt->PushVarTable();
						sml->Parse();
						rt->PopVarTable();
						if ( sml->IsBreaked() ) {
							m_eType = E_STATEMENT_BREAK;
						}
						IsProcessed = true;
					}
					else {
						sml->GoThrough();
					}
				
					while ( true ) {
						// deal with 'elif'
						if ( GetParser()->HasError() ) {
							break;
						}

						if ( strcmp( lex->GetNextTokenPointer()->GetCharValue(), "elif" ) == 0 ) {
							lex->MoveNext();

							expr->Clear();
							expr->Parse();

							if ( !GetParser()->HasError() ) {
								sml->Clear();

								if ( !IsProcessed ) {
									// not process batch
									Variable* var = expr->GetValue();
									
									if ( IsVariableTrue( var ) ) {
										rt->PushVarTable();
										sml->Parse();
										rt->PopVarTable();
										if ( sml->IsBreaked() ) {
											m_eType = E_STATEMENT_BREAK;
										}
										IsProcessed = true;
									}
									else {
										sml->GoThrough();
									}
								}
								else {
									sml->GoThrough();
								}
							}
							else {
								// parse error
								break;
							}
						}
						else if ( strcmp( lex->GetNextTokenPointer()->GetCharValue(), "else" ) == 0 ) {
							lex->MoveNext();

							if ( !GetParser()->HasError() ) {
								sml->Clear();

								if ( !IsProcessed ) {
									rt->PushVarTable();
									sml->Parse();
									rt->PopVarTable();
									if ( sml->IsBreaked() ) {
										m_eType = E_STATEMENT_BREAK;
									}
									IsProcessed = true;
								}
								else {
									sml->GoThrough();
								}
							}
							else {
								// parse error
								break;
							}
							break;
						}
						else {
							// if the token is neither 'elif' or 'else', if-elif-else statement is over
							break;
						}
					}
				}
			}
			else if ( strcmp( lex->GetNextTokenPointer()->GetCharValue(), "while" ) == 0 ) {
				// is hard to process 'while' statement, because we should record the expression after keyword'while'
				// the better idea is to record the position use lexer
				m_eType = E_STATEMENT_WHILE;

				lex->PushPos();
				lex->MoveNext();

				rt->PushVarTable();
				while ( true ) {
					expr->Clear();
					expr->Parse();

					if ( GetParser()->HasError() ) {
						// process error
						break;
					}

					Variable* var = expr->GetValue();
					sml->Clear();

					if ( IsVariableTrue( var ) ) {
						sml->Parse();
					}
					else {
						sml->GoThrough();
						break;
					}

					if ( GetParser()->HasError() ) {
						break;
					}

					if ( sml->IsBreaked() ) {
						// check if there exist 'break' in the statement list
						break;
					}

					lex->GotoTopPos();
					// now the next token is 'while'
					lex->MoveNext();
				}
				rt->PopVarTable();

				lex->PopPos();
			}
			else if ( strcmp( lex->GetNextTokenPointer()->GetCharValue(), "break" ) == 0 ) {
				// 'while-break' statement
				if ( lex->PosStackEmpty() ) {
					// error here, no while statment to break
					GetParser()->SetError( Parser::E_PAERR_CANNOTBREAK );
				}
				else {
					lex->MoveNext();
					m_eType = E_STATEMENT_BREAK;
				}
			}
		}
	}
	else if ( e_ttype == E_TOKEN_DOLLAR ) {
		// maybe assignment
		expr->Parse();
		if ( expr->IsSingleVariable() ) {
			// assume this is a assignment
			std::string varName = std::string( lex->GetPrevTokenPointer()->GetCharValue() );

			Variable* pvar = rt->GetVariable( varName );
			if ( pvar == NULL ) {
				pvar = rt->AddVariable( varName, Variable() );
			}
			(*m_pVar) = (*pvar);

			if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_EQUAL ) {
				lex->MoveNext();

				stm->Clear();
				stm->Parse();

				(*pvar) = *( stm->GetValue() );
				(*m_pVar) = (*pvar);
				m_eType = E_STATEMENT_ASSIGNMENT;
			}
		}
		else {
			// expression
			Variable var;
			if ( expr->GetValue() != NULL ) {
				var = *( expr->GetValue() );
			}
			
			(*m_pVar) = var;
			m_eType = E_STATEMENT_EXPRESSION;
		}
		
	}
	else {
		// expression
		expr->Parse();
		Variable var;
		if ( expr->GetValue() != NULL ) {
			var = *( expr->GetValue() );
		}
		
		(*m_pVar) = var;
		m_eType = E_STATEMENT_EXPRESSION;
	}

#ifdef _DEBUG
	printf( "Statement end %d, next token %s\n", GetType(), lex->GetNextTokenPointer()->GetTokenTypeString() );
#endif
	delete stm;
	delete sml;
	delete expr;
	delete fac;
}

/****************************************************************************
 StatementList implementation
****************************************************************************/
StatementList::StatementList( Parser* par ) : 
	Parsable( par ),
	m_bIsBreaked( false )
{
	m_pVar = new Variable();
}

StatementList::~StatementList() {
	if ( m_pVar ) {
		delete m_pVar;
		m_pVar = NULL;
	}
}

void StatementList::Clear() {
	m_bIsBreaked = false;
	// nothing to do
}

void StatementList::Parse() {
	// statement list must start with '{' and end with '}'
	Lexer* lex = GetLexer();
	Runtime* rt = GetRuntime();

	Token tok;
	const Token* ptok;

	Statement* stm = new Statement( GetParser() );

	ptok = lex->GetNextTokenPointer();

	E_TOKEN_TYPE e_ttype = ptok->GetType();

	if ( e_ttype == E_TOKEN_LBRACE ) {
		lex->MoveNext();

		while ( true ) {
			if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_RBRACE ) {
				// come across '}', the just return
				lex->MoveNext();
				break;
			}
			else if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_END ) {
				GetParser()->SetError( Parser::E_PAERR_SML_BRACEUNMATCH );
				break;
			}

			stm->Clear();
			stm->Parse();

			if ( stm->GetType() == E_STATEMENT_BREAK ) {
				// go through the codes below
				int brace_count = 0;
				do {
					if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_END ){
						GetParser()->SetError( Parser::E_PAERR_SML_BRACEUNMATCH );
						break;
					}
					else if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_RBRACE && brace_count == 0 ) {
						lex->MoveNext();
						break;
					}
					else if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_LBRACE ) {
						brace_count ++;
					}
					else if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_RBRACE ) {
						brace_count --;
					}
					lex->MoveNext();
				} while ( true );
				m_bIsBreaked = true;
				break;
			}

			if ( GetParser()->HasError() ) {
				break;
			}
		}
	}
	else {
		GetParser()->SetError( Parser::E_PAERR_SML_START );
	}
	
	delete stm;
	
	return;
}

void StatementList::GoThrough() {
	// just find the matched '}'

	Lexer* lex = GetLexer();
	Runtime* rt = GetRuntime();

	if ( lex->GetNextTokenPointer()->GetType() != E_TOKEN_LBRACE ) {
		GetParser()->SetError( Parser::E_PAERR_SML_START );
	}
	else {
		int brace_count = 0;
		
		do {
			lex->MoveNext();
			if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_END ){
				GetParser()->SetError( Parser::E_PAERR_SML_BRACEUNMATCH );
				break;
			}
			else if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_RBRACE && brace_count == 0 ) {
				lex->MoveNext();
				break;
			}
			else if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_LBRACE ) {
				brace_count ++;
			}
			else if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_RBRACE ) {
				brace_count --;
			}
		} while ( true );
	}

	return;
}

bool StatementList::IsBreaked() const {
	return m_bIsBreaked;
}

#define _CS_PARSER_TEST_
#ifdef _CS_PARSER_TEST_
int main( int argc, char* argv[] ) {
	if ( argc <= 1 ) return 0;

	//const char* filename = "d:\\tangel\\svn\\snev\\tlab\\CsScripts\\test_scripts\\ifelse.cs";
	const char* filename = argv[1];
	Parser* par = new Parser();
	par->Initialize( filename );

	par->Parse();

	getchar();
}
#endif //_CS_PARSER_TEST_

