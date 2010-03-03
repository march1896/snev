#ifndef _MSDT_
#define _MSDT_
#include "sdt_def.h"
class SDT_Memory;
class SDT_Pipe;

class MSDT {
public:
	MSDT();
	~MSDT();

	// pn is the number of pipes
	void 					Initialize( int pn, f32 max_error = 0.001f, int memory_size = 0 );

	void 					Destroy();
	
	// push back a couple of data, the size is n_pipes.
	void 					PushBack( const sdt_data* const data );

	// get the index'th data in this pipe, if index < 0, it will count inverse.
	bool			 		GetData( sdt_data* data, int index ) const;

	// return the useful data length
	int 					GetValidDataLength() const;

	// flush all the pipes, see sdt_pipe.h to why we should flush
	// we should flush the data, then call 'GetData()' func
	void 					Flush();
private:
	// if the memory is not enough to use, just clean up, 
	// this process will automaticly done.
	void 					CleanUpMemory();

private:
	SDT_Memory* 			p_memory;

	int 					n_pipes;
	SDT_Pipe* 				p_pipes;
	sdt_data* 				p_dummy_dataline;

	bool 					b_inited;
};

#endif // _MSDT_
