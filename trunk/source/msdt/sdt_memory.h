#ifndef _SDT_MEMORY_
#define _SDT_MEMORY_
#include "sdt_def.h"

#define _REPLAY_MEM_SIZE_ (1<<19)

class SDT_Memory {
public:
	SDT_Memory();
	~SDT_Memory();

	void 					Initialize( int size );
	void 					Destroy();

	// allocate a frame_unit, return NULL if allocate failed
	sdt_frame_unit* 		Alloc();

	// return a frame_unit to the buffer
	void 					Dealloc( void* addr );

	// get used percent of the buffer, return 0.0 ~ 1.0
	f32 					GetUsedPercent() const;
private:
	char* 		 			p_buffer;
	sdt_frame_unit* 		p_curr;

	int 					n_total_size;
	int 					n_used_size;

	bool 					b_inited;
};

#endif // _SDT_MEMORY_
