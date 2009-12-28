#ifndef _CS_PARSER_
#define _CS_PARSER_
#include "variable.h"
#include "lexer.h"
#include "runtime.h"

class Parser {
public:
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
		E_PAERR_EXP_OPTYPEDIFF,
		E_PAERR_EXP_OPAND,
		E_PAERR_EXP_OPOTHER,
		E_PAERR_EXP_PARERROR,
		E_PAERR_EXP_PAREUNMATCH,
		E_PAERR_ASSIGNMENT_LEFT,
		E_PAERR_SML_START,
		E_PAERR_SML_BRACEUNMATCH,
		E_PAERR_CANNOTBREAK,

		E_PAERR_END,
	};

private:
	static const char* 				ErrorInfo[ E_PAERR_END ];

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
	bool 						HasError() { return m_eParErr != E_PAERR_OK; }
	const char * 				GetErrorString() { return ErrorInfo[ m_eParErr ]; };
	E_PAERR_TYPE 				GetErrorType() { return m_eParErr; }
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
	virtual void 				Clear() = 0;

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
	void 						Clear();

	// check if the expression can be the left part of an assignment
	bool 						IsSingleVariable() const { return m_bIsSingleVariable; }
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
	bool 						m_bIsSingleVariable;
	static int 					SymWeight[ E_TOKEN_END ];
};

enum E_STATEMENT_TYPE {
	E_STATEMENT_NONE,
	E_STATEMENT_EXPRESSION,
	E_STATEMENT_ASSIGNMENT,
	E_STATEMENT_IFELSE,
	E_STATEMENT_WHILE,
	E_STATEMENT_BREAK,
	E_STATEMENT_COMMENT,
	E_STATEMENT_END,
};

class Statement : public Parsable {
public:
								Statement( Parser* par );
								~Statement();

	void 						Parse();
	Variable* 					GetValue() { return m_pVar; }
	E_STATEMENT_TYPE 			GetType() const { return m_eType; }

	void 						Clear();
	// three types of statement, 
	// first, Assignment "expr = expr1 = expr2"
	// second, "if-elif-else" statement, if expr { statement } elif expr { statement } elif expr { statement } else { statement }
	// third, "while expr { statement }"
private:
	// to check the expression's return value is true or not, use in 'if expr' or 'while expr'
	bool 						IsVariableTrue( const Variable* var );

private:
	E_STATEMENT_TYPE 			m_eType;
	Variable* 					m_pVar;
};

class StatementList : public Parsable {
public:
								StatementList( Parser* par );
								~StatementList();

	void 						Parse();
	Variable* 					GetValue() { return m_pVar; }
	void 						Clear();
	// just go throuth the text without parse
	void 						GoThrough();
	bool 						IsBreaked() const;
private:
	Variable* 					m_pVar;
	bool 						m_bIsBreaked;
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
