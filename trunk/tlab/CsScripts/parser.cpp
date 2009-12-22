#include "parser.h"
#include "variable.h"
#include "function.h"

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
	if ( t_type == E_TOKEN_IDENTITY ) {
		// just for key word, because functions and variables have special prefix
		const char* str = tok.GetCharValue();
		if ( strcmp( str, "if" ) == 0 || strcmp( str, "elif" ) == 0 || strcmp( str, "else" ) == 0 ) {
			m_eType = E_FACTOR_KEYWORD;
		}
		else {
			// now we just support if-else gramma
			GetParser()->SetError( E_PAERR_KEYWORD_UNDEF );
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
			}
			else {
				m_eType = E_FACTOR_VARIABLE;
			}
		}
		else {
			// error
			GetParser()->SetError( E_PAERR_VARIABLE_DEF );
		}
	}
	else if ( t_type == E_TOKEN_AMPER ) {
		// '&function( para1, para2, para3 )' is a function
		lex->MoveNext();
		ptok = lex->GetNextTokenPointer();

		if ( ptok->GetType() == E_TOKEN_IDENTITY ) {
			Function* func = rt->GetFunction( std::string( ptok->GetCharValue() ) );

			if ( func == NULL ) {
				GetParser()->SetError( E_PAERR_FUNCTION_UNDEF );
			}

			lex->MoveNext();
			ptok = lex->GetNextTokenPointer();

			if ( ptok->GetType() != E_TOKEN_LPAR ) {
				// '('
				GetParser()->SetError( E_PAERR_FUNCTION_PAR );
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

				lex->MoveNext();
				ptok = lex->GetNextTokenPointer();

				if ( ptok->GetType() == E_TOKEN_RPAR ) {
					// ')'
					break;
				}
				else if ( ptok->GetType() == E_TOKEN_COMMA ) {
					lex->MoveNext();
					ptok = lex->GetNextTokenPointer();
				}
				else {
					GetParser()->SetError( E_PAERR_FUNCTION_PARAMS_OTHER );
				}
			}
		}
		else {
			GetParser()->SetError( E_PAERR_FUNCTION_DEF );
		}
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
	std::vector< Variable* >::iterator v_itr = m_VarStack.begin();
	for ( ; v_itr != m_VarStack.end(); v_itr ++ ) {
		delete (*v_itr);
	}
	m_VarStack.clear();
	
	m_SymStack.clear();
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
	fac->Parse();

	// deal with init input
	if ( fac->GetType() == E_FACTOR_VARIABLE || fac->GetType() == E_FACTOR_VARIABLE_TEMP ) {
		PushVar( *(fac->GetValue()) );
		m_bIsLastVariable = true;
	}
	else if ( fac->GetType() == E_FACTOR_SYMBOL ) {
		if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_LPAR ) {
			PushSym( E_TOKEN_LPAR );
			m_bIsLastVariable = false;
		}
		else {
			GetParser()->SetError( E_PAERR_EXP_START );
		}
	}
	else if ( fac->GetType() == E_FACTOR_VARIABLE_UNDEF ) {
		GetParser()->SetError( E_PAERR_EXP_VARUNDEF );
	}
	else {
		// error, expression can only start with variable or '('
		GetParser()->SetError( E_PAERR_EXP_START );
	}

	fac->Clear();
	lex->MoveNext();

	while ( true ) {
		fac->Parse();

		// precess error
		if ( fac->GetType() == E_FACTOR_VARIABLE_UNDEF ) {
			GetParser()->SetError( E_PAERR_EXP_VARUNDEF );
			break;
		}

		Variable var1, var2;
		Variable result;
		E_TOKEN_TYPE op;

		if ( fac->GetType() == E_FACTOR_VARIABLE || fac->GetType() == E_FACTOR_VARIABLE_TEMP ) {
			if ( m_bIsLastVariable  ) {
				// variable follows variable, the expression is end
				while ( true ) {
					var2 = PopVar();
					var1 = PopVar();

					op = PopSym(); 
					result = Calculate( var1, var2, op );

					PushVar( result );
				}

				if ( m_VarStack.size() != 1 || m_SymStack.size() != 0 ) {
					GetParser()->SetError( E_PAERR_EXP_UNCALCABLE );
				}
				break;
			}

			PushVar( *(fac->GetValue()) );
			m_bIsLastVariable;
		}
		else if ( fac->GetType() == E_FACTOR_SYMBOL ) {
			if ( !CheckLegalSymbol( lex->GetNextTokenPointer()->GetType() ) ) {
				// not valid symbol comes, assume the expression is end
				while ( true ) {
					var2 = PopVar();
					var1 = PopVar();

					op = PopSym(); 
					result = Calculate( var1, var2, op );

					PushVar( result );
				}
				if ( m_VarStack.size() != 1 || m_SymStack.size() != 0 ) {
					GetParser()->SetError( E_PAERR_EXP_UNCALCABLE );
				}
			}
			else if ( lex->GetNextTokenPointer()->GetType() == E_TOKEN_RPAR ) {
				while ( TopSym() != E_TOKEN_LPAR ) {
					var2 = PopVar();
					var1 = PopVar();

					op = PopSym(); 
					result = Calculate( var1, var2, op );

					PushVar( result );
				}
				PopSym();
			}
			else {
				op = TopSym();
				while ( SymLessThen( lex->GetNextTokenPointer()->GetType(), op ) ) {
					var2 = PopVar();
					var1 = PopVar();

					op = PopSym(); 
					result = Calculate( var1, var2, op );

					PushVar( result );
				}

				PushSym( lex->GetNextTokenPointer()->GetType() );
			}
		}
	}

	return;
}

int Expression::SymWeight[ E_TOKEN_END ] = {};
void Expression::InitSymWeight() {
	Expression::SymWeight[ E_TOKEN_PLUS ] = 3;
	Expression::SymWeight[ E_TOKEN_MINUS ] = 3;
	Expression::SymWeight[ E_TOKEN_STAR ] = 4;
	Expression::SymWeight[ E_TOKEN_SLASH ] = 4;
	Expression::SymWeight[ E_TOKEN_AND ] = 2;
	Expression::SymWeight[ E_TOKEN_OR ] = 2;
	Expression::SymWeight[ E_TOKEN_LESS ] = 1;
	Expression::SymWeight[ E_TOKEN_LESSEQUAL ] = 1;
	Expression::SymWeight[ E_TOKEN_GREATER ] = 1;
	Expression::SymWeight[ E_TOKEN_GREATEREQUAL ] = 1;
	Expression::SymWeight[ E_TOKEN_LPAR ] = 5;
	Expression::SymWeight[ E_TOKEN_RPAR ] = 0;
}

bool Expression::SymLessThen( E_TOKEN_TYPE sym1, E_TOKEN_TYPE sym2 ) {
	return SymWeight[ sym1 ] < SymWeight[ sym2 ];
}

Variable Expression::Calculate( const Variable &var1, const Variable &var2, E_TOKEN_TYPE op ) {
	if ( !CheckLegalSymbol( op ) || op == E_TOKEN_LPAR || op == E_TOKEN_RPAR ) {
		GetParser()->SetError( E_PAERR_EXP_OPUNDEF );
		// return NULL value
		return Variable();
	}

	if ( var1.GetType() != var2.GetType() ) {
		GetParser()->SetError( E_PAERR_EXP_OPTPYEDIFF );
		return Variable();
	}
	else if ( var1.GetType() == Variable::E_NULL || var2.GetType() == Variable::E_NULL ) {
		GetParser()->SetError( E_PAERR_EXP_OPUNDEF );
		return Variable();
	} 
	else if ( var1.GetType() == Variable::E_STRING && op != E_TOKEN_PLUS ) {
		GetParser()->SetError( E_PAERR_EXP_STRINGOPERATION );
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
			if ( var1.GetType() == Variable::E_INT ) 			ret &= ( var1.GetIntValue() != 0 );
			else if ( var1.GetType() == Variable::E_FLOAT ) 	ret &= ( var1.GetFloatValue() != 0.0 );
			else {
				GetParser()->SetError( E_PAERR_EXP_OPAND );
				return Variable();
			}

			if ( var2.GetType() == Variable::E_INT ) 			ret &= ( var1.GetIntValue() != 0 );
			else if ( var2.GetType() == Variable::E_FLOAT ) 	ret &= ( var1.GetFloatValue() != 0.0 );
			else {
				GetParser()->SetError( E_PAERR_EXP_OPAND );
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
			if ( var1.GetType() == Variable::E_INT ) 			ret |= ( var1.GetIntValue() != 0 );
			else if ( var1.GetType() == Variable::E_FLOAT ) 	ret |= ( var1.GetFloatValue() != 0.0 );
			else {
				GetParser()->SetError( E_PAERR_EXP_OPAND );
				return Variable();
			}

			if ( var2.GetType() == Variable::E_INT ) 			ret |= ( var1.GetIntValue() != 0 );
			else if ( var2.GetType() == Variable::E_FLOAT ) 	ret |= ( var1.GetFloatValue() != 0.0 );
			else {
				GetParser()->SetError( E_PAERR_EXP_OPAND );
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
		default:
			GetParser()->SetError( E_PAERR_EXP_OPOTHER );
			return Variable();
	}
}

Variable* Expression::GetValue() {
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
