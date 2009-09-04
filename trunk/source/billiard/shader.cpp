#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shader.h"
#include "gl\glew.h"
#include "log.h"
static char *textFileRead(char *fn);
static int textFileWrite(char *fn, char *s);

void Shader::PrintShaderInfo()
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	Log::print( "vertex shader:");
	glGetShaderiv(vs, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(vs, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	}

	Log::print( "fragment shader:" );
	glGetShaderiv(fs, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(fs, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	}
}

void Shader::PrintProgramInfo()
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	}
}


bool Shader::IsSupported() {
	if ( glewIsSupported( "GL_VERSION_2_0" ) )
		return true;
	else return false;
}

Shader::Shader():
	vsFile( NULL ), fsFile( NULL ),
	vsSource( NULL), fsSource( NULL ),
	vs( -1 ), fs( -1 ),
	program( -1 ),
	inited( false )
{}

Shader::Shader( const char * vf, const char * ff ):
	vsSource( NULL), fsSource( NULL ),
	vs( -1 ), fs( -1 ),
	program( -1 ),
	inited( false )
{
	if ( vf != NULL ) {
		int length = strlen( vf );
		vsFile = (char*)malloc( ( length + 1 ) * sizeof( char ) );
		strcpy( vsFile, vf );
	} else {
		vsFile = NULL;
	}

	if ( ff != NULL ) {
		int length = strlen( ff );
		fsFile = ( char * ) malloc( ( length + 1 ) * sizeof( char ) );
		strcpy( fsFile, ff );
	} else {
		fsFile = NULL;
	}
}

Shader::~Shader() {
	if ( vsFile != NULL ) free( vsFile );
	if ( fsFile != NULL ) free( fsFile );
	if ( vsSource != NULL ) free( vsSource );
	if ( fsSource != NULL ) free( fsSource );
}

void Shader::SetVsFile( const char* filename ) {
	if ( vsFile != NULL ) {
		free( vsFile );
		if ( vsSource != NULL )
			free( vsSource );
		inited = false;
	}

	int length = strlen( filename );
	vsFile = (char*)malloc( ( length + 1 ) * sizeof( char ) );
	strcpy( vsFile, filename );
}

void Shader::SetFsFile( const char* filename ) {
	if ( fsFile != NULL ) {
		free( fsFile );
		if ( fsSource != NULL ) 
			free( fsSource );
		inited = false;
	}
	int length = strlen( filename );
	fsFile = ( char * ) malloc( ( length + 1 ) * sizeof( char ) );
	strcpy( fsFile, filename );
}

void Shader::Activate() {
	if ( !Shader::IsSupported() ) {
		Log::print( "Shader is not supported" );
		return;
	}

	if ( !inited ) {
		/* in fact, this is a longing that, only one shader is rewrite by the author, and the other shader is remain the same functionality
		 * if one of the shaders( vert & frag ) is rewrite, the other must be rewrite, so you can not just change the position of vertex and 
		 * expect that lighting system remains work, so the implementation commented below may be a better idea here
		 */
		if ( vsFile == NULL ) {
			Log::print( "vertex shader not exit" );
		} else {
			vs = glCreateShader( GL_VERTEX_SHADER );
			vsSource = textFileRead( vsFile );
			const char *vv = vsSource;
			glShaderSource( vs, 1, &vv, NULL );
			glCompileShader( vs );
		}

		if ( fsFile == NULL ) {
			Log::print( "fragment shader not exit" );
		} else {
			fs = glCreateShader( GL_FRAGMENT_SHADER );
			fsSource = textFileRead( fsFile );
			const char *ff = fsSource;
			glShaderSource( fs, 1, &ff, NULL );
			glCompileShader( fs );
		}

		PrintShaderInfo();
		if ( vsFile != NULL || fsFile != NULL ) {
			program = glCreateProgram();

			if ( vsFile != NULL ) glAttachShader( program, vs );
			if ( fsFile != NULL ) glAttachShader( program, fs );

			glLinkProgram( program );
			PrintProgramInfo();
			inited = true;
		}

		/*
		vs = glCreateShader(GL_VERTEX_SHADER);
		fs = glCreateShader(GL_FRAGMENT_SHADER);

		if ( vsFile == NULL || fsFile == NULL ) {
			Log::print( "shader does not exit" );
		}

		vsSource = textFileRead( vsFile );
		fsSource = textFileRead( fsFile );

		const char * vv = vsSource;
		const char * ff = fsSource;

		glShaderSource(vs, 1, &vv,NULL);
		glShaderSource(fs, 1, &ff,NULL);

		glCompileShader(vs);
		glCompileShader(fs);

		PrintShaderInfo();

		program = glCreateProgram();
		glAttachShader(program,vs);
		glAttachShader(program,fs);

		glLinkProgram(program);
		PrintProgramInfo();
		inited = true;
		*/
	}

	glUseProgram(program);
}

void Shader::Deactivate() {
	if ( !Shader::IsSupported() ) {
		Log::print( "Shader is not supported" );
		return; 
	}
	glUseProgram( 0 );
	// glUnUseProgram() ?
}

bool Shader::SetAttribute( const char *vn, float p1 ) {
	if ( !inited ) return false;
	if ( vn == NULL ) return false;

	GLint loc;
	loc = glGetAttribLocation( program, vn );

	if ( loc == -1 ) return false;
	glVertexAttrib1f( loc, p1 );
	return true;
}

bool Shader::SetAttribute( const char *vn, float p1, float p2 ) {
	if ( !inited ) return false;
	if ( vn == NULL ) return false;

	GLint loc;
	loc = glGetAttribLocation( program, vn );

	if ( loc == -1 ) return false;
	glVertexAttrib2f( loc, p1, p2 );
	return true;
}

bool Shader::SetAttribute( const char *vn, float p1, float p2, float p3 ) {
	if ( !inited ) return false;
	if ( vn == NULL ) return false;

	GLint loc;
	loc = glGetAttribLocation( program, vn );

	if ( loc == -1 ) return false;
	glVertexAttrib3f( loc, p1, p2, p3 );
	return true;
}

bool Shader::SetAttribute( const char *vn, float p1, float p2, float p3, float p4 ) {
	if ( !inited ) return false;
	if ( vn == NULL ) return false;

	GLint loc;
	loc = glGetAttribLocation( program, vn );

	if ( loc == -1 ) return false;
	glVertexAttrib4f( loc, p1, p2, p3, p4 );
	return true;
}

bool Shader::SetUniform( const char *vn, float p1 ) {
	if ( !inited ) return false;
	if ( vn == NULL ) return false;

	GLint loc;
	loc = glGetUniformLocation( program, vn );

	if ( loc == -1 ) return false;
	glUniform1f( loc, p1 );
	return true;
}

bool Shader::SetUniform( const char *vn, float p1, float p2 ) {
	if ( !inited ) return false;
	if ( vn == NULL ) return false;

	GLint loc;
	loc = glGetUniformLocation( program, vn );

	if ( loc == -1 ) return false;
	glUniform2f( loc, p1, p2 );
	return true;
}

bool Shader::SetUniform( const char *vn, float p1, float p2, float p3 ) {
	if ( !inited ) return false;
	if ( vn == NULL ) return false;

	GLint loc;
	loc = glGetUniformLocation( program, vn );

	if ( loc == -1 ) return false;
	glUniform3f( loc, p1, p2, p3 );
	return true;
}

bool Shader::SetUniform( const char *vn, float p1, float p2, float p3, float p4 ) {
	if ( !inited ) return false;
	if ( vn == NULL ) return false;

	GLint loc;
	loc = glGetUniformLocation( program, vn );

	if ( loc == -1 ) return false;
	glUniform4f( loc, p1, p2, p3, p4 );
	return true;
}

char *textFileRead(char *fn) {
	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {
		fp = fopen(fn,"rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

int textFileWrite(char *fn, char *s) {

	FILE *fp;
	int status = 0;

	if (fn != NULL) {
		fp = fopen(fn,"w");

		if (fp != NULL) {

			if (fwrite(s,sizeof(char),strlen(s),fp) == strlen(s))
				status = 1;
			fclose(fp);
		}
	}
	return(status);
}


