#include "lfile.h"
#ifdef PC
#include <cstring>
#include <cctype>

const int LFile::N_LINE = 200;

LFile::LFile( const char* filename ) {
	m_pfile = fopen( filename, "r" );
	m_linenumber = 0;

	m_linebuff = new char[ N_LINE ];
}

LFile::~LFile() {
	fclose( m_pfile );

	delete[] m_linebuff;
}

const char* LFile::GetNextLine() {
	if ( fgets( m_linebuff, N_LINE, m_pfile ) ) {
		m_linenumber ++;
		return m_linebuff;
	}
	else {
		return NULL;
	}
}

int LFile::Tell() const {
	return ftell( m_pfile );
}

void LFile::Seek( int pos ) {
	fseek( m_pfile, pos, SEEK_SET);
}
#endif

