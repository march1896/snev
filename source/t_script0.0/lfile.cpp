#include "lfile.h"
#ifdef __PC__
#include <cstring>
#include <cctype>

using namespace CSSPT;

const u32 LFile::N_LINE = 200;

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

u32 LFile::Tell() const {
	return ftell( m_pfile );
}

void LFile::Seek( u32 pos ) {
	fseek( m_pfile, pos, SEEK_SET);
}
#endif

#ifdef __WII__

using namespace CSSPT;

const u32 LFile::N_LINE = 200;
const u32 LFile::N_FILENAME = 100;

#define RSSYS ( dynamic_cast<CResourceSystemPrivateImpl*>(&CTKCSystemFactory::GetInstance().GetResourceSystemPrivate()) )

LFile::LFile( const char* filename ) {
	m_filename = new char[ N_FILENAME ];
	strcpy( m_filename, filename );

	m_filebuffer = static_cast< char* >( RSSYS->GetFileBuffer((const s8*) m_filename, "CSSCRIPT", &m_filelength ) );
	m_linebuffer = new char[ N_LINE ];

	m_linenumber = 0;
	m_fileoffset = 0;
}

LFile::~LFile() {
	RSSYS->ReleaseFileBuffer( m_filename );

	delete[] m_filename;
	delete[] m_linebuffer;
	//delete[] m_filebuffer;
}

const char* LFile::GetNextLine() {
	if ( m_fileoffset >= m_filelength ) {
		return NULL;
	}
	
	u32 offset = m_fileoffset;
	u32 i;
	
	for ( i = 0; m_filebuffer[ offset + i ] != '\n'; i ++ ) {
		if ( offset + i == m_filelength ) {
			// the end of the file
			break;
		}
		m_linebuffer[ i ] = m_filebuffer[ offset + i ];
	}
	
	m_linebuffer[ i ] = '\n';
	m_linebuffer[ i + 1 ] = '\0';
	m_fileoffset = offset + i + 1;

	m_linenumber ++;
	return m_linebuffer;
}

u32 LFile::Tell() const {
	return m_fileoffset;
}

void LFile::Seek( u32 pos ) {
	m_fileoffset = pos;
}

#endif
