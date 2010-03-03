#include "sdt_pipe.h"
#include "sdt_memory.h"

SDT_Pipe::SDT_Pipe( f32 error, SDT_Memory* pmem ) :
	curr_upper_slope( 0.0f ),
	curr_lower_slope( 0.0f ),
	curr_start( 0.0f ),
	curr_length( 0 ),
	max_error( error ),
	n_data( 0 ),
	n_memory( 0 ),
	p_memory( pmem ),
	p_frame_unit_first( NULL ),
	p_frame_unit_curr( NULL )
{
}

SDT_Pipe::~SDT_Pipe() {
	// nothing
}

bool SDT_Pipe::PushBack( sdt_data data ) {
	if ( curr_length == 0 ) {
		curr_start = data;
		curr_upper_slope = MIN_SLOPE;
		curr_lower_slope = MAX_SLOPE;

		curr_length ++;
	}
	else {
		if ( curr_length + 1 == MAX_LENGTH ) {
			// the length extend the max length
			Flush();
			PushBack( data );
			return true;
		}

		f32 diff = data - curr_start;
		if ( diff / curr_length < MIN_SLOPE || diff / curr_length > MAX_SLOPE ) {
			Flush();
			PushBack( data );
			return true;
		}

		f32 up, down;
		up = ( data - curr_start - max_error ) / curr_length;
		down = ( data - curr_start + max_error ) / curr_length;

		if ( up > curr_lower_slope || down < curr_upper_slope ) {
			Flush();
			PushBack( data );
			return true;
		}

		if ( up > curr_upper_slope ) curr_upper_slope = up;
		if ( down < curr_lower_slope ) curr_lower_slope = down;
		curr_length ++;
	}
	return true;
}

void SDT_Pipe::Flush() {
	if ( curr_length == 0 ) return;

	sdt_frame* pfr = GetNewFrame();
	if ( pfr == NULL ) {
		//error 
	}
	
	pfr->start = curr_start;
	pfr->slope = float2slope( (curr_upper_slope + curr_lower_slope ) / 2 );
	pfr->length = curr_length;

	p_frame_unit_curr->n_data += curr_length;
	n_data += curr_length;

	curr_length = 0;
}

bool SDT_Pipe::PopFront() {
	// TODO: there no need for the current project
	return 0;
}

void SDT_Pipe::PopFirstUnit() {
	n_data -= p_frame_unit_first->n_data;
	sdt_frame_unit* to_free = p_frame_unit_first;
	p_frame_unit_first = p_frame_unit_first->next;

	p_memory->Dealloc( (void*)to_free );
	n_memory--;
}

int SDT_Pipe::GetFirstUnitLength() const {
	return p_frame_unit_first->n_data;
}

int SDT_Pipe::GetLength() const {
	return n_data;
}

int SDT_Pipe::GetMemorySize() const {
	return n_memory;
}

sdt_data SDT_Pipe::GetData( int index ) {
	// assert( curr_length == 0 ); // the data is flushed to used

	if ( index > n_data || index < -n_data ) {
		// error index!!
		return 0.0f;
	}
	if ( index < 0 ) {
		index += n_data;
	}

	int accum = 0;

	sdt_frame_unit* pfu = p_frame_unit_first;
	do {
		if ( pfu == NULL ) {
			// not so many data stored
			return 0.0f;
		}
		if ( accum + pfu->n_data > index ) break;
		accum += pfu->n_data;
		pfu = pfu->next;
	} while ( true );

	int s = index - accum;
	int k = 0;

	while ( s - pfu->frames[k].length >= 0 ) {
		s -= pfu->frames[k].length;
		k ++;
	}

	return sdt_frame_get( &pfu->frames[k], s );
}

sdt_frame* SDT_Pipe::GetNewFrame() {
	sdt_frame* ret = NULL;

	if ( p_frame_unit_first == NULL ) {
		sdt_frame_unit* pfu = p_memory->Alloc();
		n_memory ++;
		sdt_frame_unit_init( pfu );

		p_frame_unit_first = p_frame_unit_curr = pfu;

		ret = &(p_frame_unit_curr->frames[ p_frame_unit_curr->end++ ]);
	}
	else {
		if ( p_frame_unit_curr->end == FRAME_PER_UNIT ) {
			// need a new frame unit
			sdt_frame_unit* pfu = p_memory->Alloc();
			if ( pfu == NULL ) return NULL;
			
			n_memory ++;
			sdt_frame_unit_init( pfu );

			p_frame_unit_curr->next = pfu;
			p_frame_unit_curr = pfu;

			ret = &(p_frame_unit_curr->frames[ p_frame_unit_curr->end++ ]);
		}
		else {
			ret = &(p_frame_unit_curr->frames[ p_frame_unit_curr->end ++ ] );
		}
	}

	return ret;
}

void SDT_Pipe::SetMemoryCallback( SDT_Memory* pmem ) {
	p_memory = pmem;
}

