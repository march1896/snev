#ifndef _CS_LEXER_
#define _CS_LEXER_

namespace CSSPT {
	
enum E_TOKEN_TYPE {
	E_TOKEN_NULL = 0,
	E_TOKEN_STRING,
	E_TOKEN_STRING_SINGLEQUOTE,
	E_TOKEN_INT,
	E_TOKEN_FLOAT,
	E_TOKEN_IDENTITY,
	E_TOKEN_LPAR,
	E_TOKEN_RPAR,
	E_TOKEN_LSQB,
	E_TOKEN_RSQB,
	E_TOKEN_COLON,
	E_TOKEN_COMMA,
	E_TOKEN_SEMI,
	E_TOKEN_PLUS,
	E_TOKEN_PLUSEQUAL,
	E_TOKEN_MINUS,
	E_TOKEN_MINUSEQUAL,
	E_TOKEN_STAR,
	E_TOKEN_MUlTIPLYEQUAL,
	E_TOKEN_SLASH,
	E_TOKEN_BACKSLASH,
	E_TOKEN_DIVIDEQUAL,
	E_TOKEN_PIPE,
	E_TOKEN_OR,
	E_TOKEN_AMPER,
	E_TOKEN_AND,
	E_TOKEN_LESS,
	E_TOKEN_GREATER,
	E_TOKEN_EQUAL,
	E_TOKEN_DOT,
	E_TOKEN_PERCENT,
	E_TOKEN_BACKQUOTE,
	E_TOKEN_LBRACE,
	E_TOKEN_RBRACE,
	E_TOKEN_EQEQUAL,
	E_TOKEN_NOTEQUAL,
	E_TOKEN_LESSEQUAL,
	E_TOKEN_GREATEREQUAL,
	E_TOKEN_TILDE,
	E_TOKEN_POUND,
	E_TOKEN_EXCLAMATION,
	E_TOKEN_DOLLAR,

	E_TOKEN_ERROR,

	E_TOKEN_COMMENT,
	E_TOKEN_BLANKLINE,
	E_TOKEN_NONE,
	E_TOKEN_END
};

class Token;
class LFile;
class Lexer;

class Token {
public:
	friend class 	Lexer;
public:
					Token();
	E_TOKEN_TYPE 	GetType() const { return m_type; }
	const char* 	GetTokenTypeString() const; 
	const char* 	GetCharValue() const { return m_dummy; }
	int 			GetIntValue() const;
	float 			GetFloatValue() const;

	const Token& 	operator=( const Token& tok );
private:
	void 			Assign( const char* start, const char* end, E_TOKEN_TYPE e );
	void 			Flush();

private:
	static const int N_TOKEN_CHAR;

	const char* 	m_start;
	const char* 	m_end;
	E_TOKEN_TYPE 	m_type;

	// I want to declare 'char m_dummy[ N_TOKEN_CHAR ]', but the compiler complains
	char 			m_dummy[ 100 ];
};


class Lexer {
public:
	enum E_ERROR_TYPE {
		E_ERROR_NONE = 0,
		E_ERROR_INITFAILED,
		E_ERROR_MEMORY,
		E_ERROR_STRING,
		E_ERROR_MISSINGSYMBOL,
		E_ERROR_FLOAT,
		E_ERROR_STACKOVERFLOW,
		E_ERROR_END,
	};

private:
	static const char *ErrorInfo[ E_ERROR_END ];
public:
					Lexer();
					Lexer( const char* filename );
					~Lexer();

	void 			BindFile( const char* filename );
	// get next token pointer
	const Token* 	GetNextTokenPointer() const { return m_next; }
	// get current token pointer
	const Token* 	GetPrevTokenPointer() const { return m_prev; }
	// get next token
	Token 			GetNextToken() const { return (*m_next); };
	// get prev token
	Token 			GetPrevToken() const { return (*m_prev); };
	// move to the next token, flush the current token, and calculate the next token
	void 			MoveNext();
	// after this initialization, the current token will be E_TOKEN_NONE, and the next token will be the first token of the file 
	void 			Initialize();

	// record the position of current lexer
	void 			PushPos();
	// pop the last position recorded
	void 			PopPos();
	// goto the position in the pos stack
	void 			GotoTopPos();
	bool 			PosStackEmpty() const;

	E_ERROR_TYPE 	GetErrorType() const { return m_errtype; } 			
	const char* 		GetErrorInfo() const { return ErrorInfo[ m_errtype ]; }
	int 			GetErrorLine() const { return m_linenumber; }
private:
	
private:
	static const int N_FILE_NAME;
	static const int N_POSSTACK_SIZE;
	
	LFile* 			m_pfile;
	const char* 	m_linebuff;
	int 			m_linenumber;

	Token 			*m_prev, *m_next;
	E_ERROR_TYPE 	m_errtype;

	int* 		m_positionstack;
	int* 		m_linenumberstack;
	int 		m_posidx;
	bool 			m_bInited;
};

}

#endif // _CS_LEXER_
