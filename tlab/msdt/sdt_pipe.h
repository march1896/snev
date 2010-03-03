#ifndef _SDT_PIPE_
#define _SDT_PIPE_
#include "sdt_def.h"
#include "sdt_memory.h"
#include <cstdlib>

class SDT_Memory;

class SDT_Pipe {
public:
public:

							SDT_Pipe( f32 max_error = 0.0f, SDT_Memory* pmem = NULL );
							~SDT_Pipe();

	// push back a data in this pipe, return true if new memory is allocated, vice versa
	bool 					PushBack( sdt_data data );

	// pop front a data in ths pipe, return true if memory is released
	bool 					PopFront();

	// althrough a data is pushed into the pipe, it may still not be stored in memory, this will store it into memory
	void 					Flush();

	// pop the front memory unit, return the number of data losed.
	void 					PopFirstUnit();

	// return the length of data stroed in the first frame_unit
	int 					GetFirstUnitLength() const;

	// return the size of data stored in this pipe
	int 					GetLength() const;

	// return the number of frame_unit used in this pipe
	int						GetMemorySize() const;

	// get the index'th data in this pipe, if index < 0, it will count inverse.
	sdt_data 				GetData( int index );

	void 					SetMaxError( f32 me ) { max_error = me; }
	void 					SetMemoryCallback( SDT_Memory* pmem );
private:
	sdt_frame* 				GetNewFrame();

private:
	// for the current 
	f32 					curr_upper_slope;
	f32 					curr_lower_slope;
	f32 					curr_start;
	int 					curr_length;

	f32 					max_error;
	int 					n_data; // number of data stored in this pipe
	int 					n_memory;  // size of memery this pipe used

	SDT_Memory* 			p_memory;

	sdt_frame_unit* 		p_frame_unit_first;
	sdt_frame_unit* 		p_frame_unit_curr;
};

#endif // _SDT_PIPE_
