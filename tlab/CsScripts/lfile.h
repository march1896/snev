#ifndef _CS_LFILE_
#define _CS_LFILE_

#include "platform.h"

#ifdef __PC__
#include <cstdio>
namespace CSSPT {

class LFile {
public:
						LFile( const char* filename );
		 				~LFile();
	const char* 		GetNextLine();
	u32 				Tell() const;
	void 				Seek( u32 pos );
private:
	static const u32 	N_LINE;
	FILE* 				m_pfile;
	char* 				m_linebuff;
	u32					m_linenumber;
};
};
#endif // PC

#ifdef __WII__
#include <revolution.h>
#include "TKCSystemFactory.h"

namespace CSSPT {
class LFile {
public:
						LFile( const char* filename );
		 				~LFile();
	const char* 		GetNextLine();
	u32 				Tell() const;
	void 				Seek( u32 pos );

private:
	static const u32 	N_LINE;
	static const u32 	N_FILENAME;
	char* 				m_filebuffer;
	char* 				m_linebuffer;
	char* 				m_filename;
	u32 				m_filelength;
	u32 				m_fileoffset;
	u32 				m_linenumber;
};
};

#endif //WII


#endif //_CS_LFILE_
