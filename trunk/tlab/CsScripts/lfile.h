#ifndef _CS_LFILE_
#define _CS_LFILE_
#define PC
#include <cstdio>

class LFile {
public:
						LFile( const char* filename );
		 				~LFile();
	const char* 		GetNextLine();
	int 			Tell() const;
	void 			Seek( int pos );
private:
	static const int 	N_LINE;
#ifdef PC
	FILE* 				m_pfile;
	char* 				m_linebuff;
	int					m_linenumber;
#endif 
};

#endif //_CS_LFILE_
