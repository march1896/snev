#ifndef _CS_PARSER_
#define _CS_PARSER_
#include "Variable.h"
#include "lexer.h"
#include "runtime.h"

enum E_PAERR_TYPE {
	E_PAERR_OK,
	E_PAERR_KEYWORD_UNDEF,
	E_PAERR_VARIABLE_DEF,
	E_PAERR_FUNCTION_UNDEF,
	E_PAERR_FUNCTION_PAR,
	E_PAERR_FUNCTION_PARAMS_OTHER,
	E_PAERR_FUNCTION_DEF,
	E_PAERR_EXP_START,
	E_PAERR_EXP_VARUNDEF,
	E_PAERR_EXP_UNCALCABLE,
	E_PAERR_EXP_STRINGOPERATION,
	E_PAERR_EXP_OPUNDEF,
	E_PAERR_EXP_OPTPYEDIFF,
	E_PAERR_EXP_OPAND,
	E_PAERR_EXP_OPOTHER,
	E_PAERR_END,
};

class Parser {
public:
								Parser();
								~Parser();
	void 						Initialize( const char* filename );
	bool 						SeekPos( const char* def, const char* name );
	void 						PreParse();
	void 						Parse();

	Lexer* 						GetLexer() const { return m_pLexer; };
	Runtime* 					GetRuntime() const { return m_pRuntime; };

	void 						SetError( E_PAERR_TYPE err ) { m_eParErr = err; }
	bool 						HasError() { return m_eParErr == E_PAERR_OK; }
private:
	Lexer* 						m_pLexer;
	Runtime* 					m_pRuntime;
	E_PAERR_TYPE 				m_eParErr;
};

class Parsable {
public:
								Parsable( Parser* par ): m_pParser( par ) {}
	virtual void 				Parse() = 0;
	virtual Variable* 			GetValue() = 0;
	Lexer* 						GetLexer() const { return m_pParser->GetLexer(); };
	Runtime* 					GetRuntime() const { return m_pParser->GetRuntime(); };
	Parser* 					GetParser() const { return m_pParser; }
protected:
	Parser* 					m_pParser;
private:
								Parsable();
};

enum E_FACTOR_TYPE {
	E_FACTOR_NONE = 0,
	E_FACTOR_KEYWORD,
	E_FACTOR_SYMBOL,
	E_FACTOR_FUNCTION,
	E_FACTOR_VARIABLE,
	E_FACTOR_VARIABLE_TEMP,
	E_FACTOR_VARIABLE_UNDEF,
	E_FACTOR_END
};

class Factor: public Parsable {
public:
								Factor( Parser* par ): Parsable( par ) {}
								~Factor();
	void 						Parse();
	void 						Clear();
	Variable* 					GetValue() { return m_pVar; }
	E_FACTOR_TYPE 				GetType() const { return m_eType; };
protected:
	E_FACTOR_TYPE 				m_eType;
	Variable* 					m_pVar;
};


class Expression : public Parsable {
public:
								Expression( Parser* par ) : Parsable( par ) { InitSymWeight(); }
								~Expression();
	void 						Parse();
	Variable* 					GetValue();

private:
	void 						PushVar( const Variable& var );
	Variable 					PopVar();
	Variable 					TopVar() const;
	void 						PushSym( E_TOKEN_TYPE sym );
	E_TOKEN_TYPE 				PopSym();
	E_TOKEN_TYPE 				TopSym() const;

	Variable 					Calculate( const Variable &var1, const Variable &var2, E_TOKEN_TYPE op );

	bool 						SymLessThen( E_TOKEN_TYPE left, E_TOKEN_TYPE right );
	bool 						CheckLegalSymbol( E_TOKEN_TYPE e );

	void 						InitSymWeight();
private:
	std::vector< Variable* > 	m_VarStack;
	std::vector< E_TOKEN_TYPE >	m_SymStack;
	bool 						m_bIsLastVariable;
	static int 					SymWeight[ E_TOKEN_END ];
};

/*
class Statement : public Parsable {
public:
	void 						Parse();
	Variable* 					GetValue();
private:
};


class Parser: public Parsable {
public:
								Parser( Lexer* lex, Runtime* rt );
								~Parser();
	void 						Parse();
	Variable* 					GetValue();
private:
								Parser();
};
*/
#endif //_CS_PARSER_H_
