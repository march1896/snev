// token.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "token.h"


char *token_type_string[]=
{
	"NULL",
	"Comment",
	"Blank line", 
	"String", 
	"Integer",
	"Float",
	"Identity", 
	"LPAR",
	"RPAR",
	"LSQB",
	"RSQB",
	"COLON",
	"COMMA",
	"SEMI",
	"PLUS",
	"MINUS",
	"STAR",
	"SLASH",
	"VBAR",
	"OR",
	"AMPER",
	"AND",
	"LESS",
	"GREATER",
	"EQUAL",
	"DOT",
	"PERCENT",
	"BACKQUOTE",
	"LBRACE",
	"RBRACE",
	"EQEQUAL",
	"NOTEQUAL",
	"LESSEQUAL",
	"GREATEREQUAL",
	"TILDE",
	"POUND",
	"EXCLAMATION",
	"DOLLAR",
	"End of type"
};

char *error_string[]=
{
	"OK",
	"arguments missing",
	"could not open file",
	"undifined type found",
	"quotes don't match"
};

const int BUF_SIZE = 32768;
int error_code=0;

int next_char( token_stat* tok )
{
	if ( *tok->cur != '\n' )
		return *tok->cur++;
	else 
	{
		tok->eol = 1;
		return ' ';
	}	
}


int check( token_stat *tok, int &error_code )
{
	char *p;
	bool negative = false;

	if ( *tok->cur == *(tok->cur+1) && *tok->cur == '/' )
	{
		return T_COMMENT;
	}
	
	if ( *tok->cur == *tok->inp && *tok->cur == '\n' )
		return T_BLANKLINE;
	
	if ( *tok->cur == '"' )
	{
		p = tok->cur + 1;
		while ( *p != '"' && *p != '\n' )
			p ++;
		if ( *p == '\n' ) 
		{
			error_code = 4;
			return -1;
		}else 
		{
			tok->inp = p + 1;
			return T_STRING;
		}
	}

	if ( *tok->cur == '\'' )
	{
		p = tok->cur + 1;
		while ( *p != '\'' && *p != '\n' )
			p ++;
		if ( *p == '\n' ) 
		{
			error_code = 4;
			return -1;
		}else 
		{
			tok->inp = p + 1;
			return T_STRING;
		}
	}

	if ( ispunct( *tok->cur ) )
	{
		p = tok->cur;
		if ( *p == '=' )
		{
			
		}
		switch ( *p ){
			case '=':
				if ( *(p+1) == '=' )
				{
					tok->inp = tok->cur + 2;
					return T_EQEQUAL;
				}else 
				{
					tok->inp = tok->cur + 1;
					return T_EQUAL;
				}
			break;
			case '+':
				if ( *(p+1) == '=' )
				{
					tok->inp = tok->cur + 2;
					return T_PLUS;
				}else 
				{
					tok->inp = tok->cur + 1;
					return T_PLUS;
				}
			break;
			case '-':
				if ( *(p+1) == '=' )
				{
					tok->inp = tok->cur + 2;
					return T_MINUS;
				}else if ( isdigit( *(p+1) ) )
				{
					negative = true;
					break;
				}else
				{
					tok->inp = tok->cur + 1;
					return T_MINUS;
				}
			break;
			case '*':
				if ( *(p+1) == '=' )
				{
					tok->inp = tok->cur + 2;
					return T_STAR;
				}else 
				{
					tok->inp = tok->cur + 1;
					return T_STAR;
				}
			break;
			case '/':
				if ( *(p+1) == '=' )
				{
					tok->inp = tok->cur + 2;
					return T_SLASH;
				}else
				{
					tok->inp = tok->cur + 1;
					return T_SLASH;
				}
			break;
			case '>':
				if ( *(p+1) == '=' )
				{
					tok->inp = tok->cur + 2;
					return T_GREATER;
				}else 
				{
					tok->inp = tok->cur + 1;
					return T_GREATEREQUAL;
				}
			break;
			case '<':
				if ( *(p+1) == '=' )
				{
					tok->inp = tok->cur + 2;
					return T_LESS;
				}else 
				{
					tok->inp = tok->cur + 1;
					return T_LESSEQUAL;
				}
			break;
			case '!':
				if ( *(p+1) == '=' )
				{
					tok->inp = tok->cur + 2;
					return T_EXCLAMATION;
				}else 
				{
					tok->inp = tok->cur + 1;
					return T_NOTEQUAL;
				}
			break;
			case '&':
				if ( *(p+1) == '&' )
				{
					tok->inp = tok->cur + 2;
					return T_AND;
				}else 
				{
					tok->inp = tok->cur + 1;
					return T_AMPER;
				}
			break;
			case '|':
				if ( *(p+1) == '|' )
				{
					tok->inp = tok->cur + 2;
					return T_OR;
				}else 
				{
					tok->inp = tok->cur + 1;
					return T_VBAR;
				}
			break;
			case '(':
				tok->inp = tok->cur + 1;
				return T_LPAR;
			case ')':
				tok->inp = tok->cur + 1;
				return T_RPAR;
			case '[':
				tok->inp = tok->cur + 1;
				return T_LSQB;
			case ']':
				tok->inp = tok->cur + 1;
				return T_RSQB;
			case '{':
				tok->inp = tok->cur + 1;
				return T_LBRACE;
			case '}':
				tok->inp = tok->cur + 1;
				return T_RBRACE;
			case ';':
				tok->inp = tok->cur + 1;
				return T_SEMI;
			case ',':
				tok->inp = tok->cur + 1;
				return T_COMMA;
			case '.':
				tok->inp = tok->cur + 1;
				return T_DOT;
			case ':':
				tok->inp = tok->cur + 1;
				return T_COLON;
			case '%':
				tok->inp = tok->cur + 1;
				return T_PERCENT;
			case '~':
				tok->inp = tok->cur + 1;
				return T_TILDE;
			case '#':
				tok->inp = tok->cur + 1;
				return T_POUND;
			case '$':
				tok->inp = tok->cur + 1;
				return T_DOLLAR;
		}
		
	}

	if ( negative || isdigit( *tok->cur ) )
	{
		p = tok->cur;
		do {
			p ++;
		}while ( isdigit( *p ) );

		if ( *p == '.' ){
			if ( !isdigit( *(p+1) ) )
			{
				error_code = 3;
				return -1;
			}
			do{
				p++;
			}while ( isdigit( *p ) );
			if ( *p == 'e' || *p == 'E' ){
				if ( *(p+1) == '+' || *(p+1) == '-' )
					p++;
				do {
					p ++;
				}while ( isdigit( *p ) );
			}
			tok->inp = p;
			return T_FLOAT;
		}
		tok->inp = p;
		return T_INT;
	}

	if ( isalpha( *tok->cur ) )
	{
		p = tok->cur;
		do{
			p ++;
		}while ( isalpha( *p ) || isdigit( *p ) || *p == '_' );
		
		tok->inp = p;
		return T_IDENTITY;
	}

	error_code = 3;
	return -1;
}

// return -1 if there is no token in the file
int get_next_token( token_stat *tok, char** start, char** end, int &error_line,int &error_code )
{
	if ( NULL == tok->cur || tok->eol )
	{
		if ( !fgets( tok->line, BUF_SIZE , tok->fp ) )
			return -1;
		else error_line ++;
		tok->inp = tok->cur = tok->line;
		tok->eol = 0;
	}
	
	while ( *tok->cur == *tok->inp && (*tok->cur == ' ' || *tok->cur == '\t') ){
		tok->cur ++;
		tok->inp ++;
	}// filter all spaces and tabs

	while ( *tok->inp != ' ' && *tok->inp != '\t' && *tok->inp != '\n' )
		tok->inp ++;


	int type = check( tok , error_code);

	if ( type == -1 ){
		return -1;
	}else if ( type == T_COMMENT ){
		tok->eol = 1;
		return get_next_token( tok, start, end, error_line, error_code );
	}else if ( type == T_BLANKLINE ){
		tok->eol = 1;
		return get_next_token( tok, start, end, error_line, error_code );
	}else if ( type >= T_STRING && type < T_END ){
		*start = tok->cur;
		*end = tok->inp;
		tok->cur = tok->inp;
		if ( *tok->cur == '\n' ) 
			tok->eol = 1;
		return type;
	}
	
	return 0;
}

int token_init( token_stat* tok, char* filename )
{
	tok->line = new char[BUF_SIZE];
	tok->cur = tok->inp = NULL; 
	tok->eol = 0;
	tok->fp = fopen( filename, "r" );
	if ( tok->fp == NULL ) return 2;

	return 0;
}

int token_deinit( token_stat* tok )
{
	if ( tok->line ) delete[] tok->line;
	if ( tok->fp )
		fclose( tok->fp );
	return 0;
}


token::token( char const* filename )
{
	int len = strlen( filename ) + 1;
	m_oFilename = new char[ len ];
	m_oError = new char[BUF_SIZE];
	if ( m_oFilename == NULL ) return; 
	strcpy( m_oFilename, filename );

	tok = new token_stat;
	error_code = token_init( tok, m_oFilename );
	eof = 0;
	error_line = 0;

	if ( error_code ){
		sprintf( m_oError, "error line %d: %s", error_line, error_string[error_code] );
	}

	start = end = NULL;
}

token::~token()
{
	token_deinit( tok );
	if ( m_oFilename ) 
		delete[] m_oFilename;
	if ( tok )
		delete tok;
	if ( m_oError )
		delete[] m_oError;
}

int token::next_token()
{
	if ( !error_code && eof != -1 ){
		eof = token_type = get_next_token( tok, &start, &end, error_line, error_code );
	}
	if ( error_code )
		sprintf( m_oError, "error line %d: %s", error_line, error_string[error_code] );
	return eof;
}

bool token::get_error(char *dest)
{
	if ( dest ){
		strcpy( dest, m_oError );
		return true;
	}
	else return false;
}

int token::get_token( char *str )
{
	if ( token_type == -1 ) return -1;

	char *src = start;
	char *dest = str;
	while ( src != end )
	{
		*dest++ = *src++;
	}
	*dest = '\0';
	return token_type;
}

int token::get_next( char * dest )
{
	int type = next_token();

	if ( error_code ){
		get_error( dest );
		return -1;
	}
	
	return get_token( dest );
}

int token::get_n_chars( char *dest, int n )
{
	return 0;
}

bool token::get_token_name(char *dest)
{
	if ( dest && token_type >= 0 ){
		strcpy( dest, token_type_string[token_type] );
		return true;
	}else return false;
}

bool token::has_error()
{
	if ( error_code ) return true;
	else return false;
}

int token::get_line_num()
{
	return error_line;
}


