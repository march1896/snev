#ifndef _TOKEN_H_
#define _TOKEN_H_
#include <stdio.h>

enum token_type
{
	T_NULL,
	T_COMMENT,
	T_BLANKLINE,
	T_STRING,
	T_INT,
	T_FLOAT,
	T_IDENTITY,
	T_LPAR,
	T_RPAR,
	T_LSQB,
	T_RSQB,
	T_COLON,
	T_COMMA,
	T_SEMI,
	T_PLUS,
	T_MINUS,
	T_STAR,
	T_SLASH,
	T_VBAR,
	T_OR,
	T_AMPER,
	T_AND,
	T_LESS,
	T_GREATER,
	T_EQUAL,
	T_DOT,
	T_PERCENT,
	T_BACKQUOTE,
	T_LBRACE,
	T_RBRACE,
	T_EQEQUAL,
	T_NOTEQUAL,
	T_LESSEQUAL,
	T_GREATEREQUAL,
	T_TILDE,
	T_POUND,
	T_EXCLAMATION,
	T_DOLLAR,
	T_END
};

struct token_stat
{
	char *line;
	char *cur, *inp ;
	int eol;
	FILE* fp;
};

class token
{
public:
	token(char const *filename );
	~token();
	
	bool get_token_name(char *str);

	int get_next( char* str );
	int get_n_chars( char *str, int n );  // this is not a good method, but I dont know how to avoid it
	bool has_error();
	bool get_error(char *str);
	int get_line_num();
private:
	int next_token(); // return -1 if at the end of the file or recognisable token found
	int get_token( char *str );

	token_stat *tok;
	char *m_oFilename;
	char *start;
	char *end;

	int token_type;
	int eof;
	int error_line;
	int error_code;
	char *m_oError;
};

#endif