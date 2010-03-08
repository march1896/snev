#include "msdt.h"
#include "sdt_pipe.h"
#include "sdt_memory.h"

MSDT::MSDT() :
	p_memory( NULL ),
	n_pipes( 0 ),
	p_pipes( NULL ),
	p_dummy_dataline( NULL ),
	b_inited( false )
{}

MSDT::~MSDT() {
	if ( b_inited ) Destroy();
}

void MSDT::Initialize( int pn, f32 max_error, int memory_size ) {
	if ( b_inited ) return;
	hlaf_float_init();

	p_memory = new SDT_Memory();

	if ( memory_size == 0 ) memory_size = _REPLAY_MEM_SIZE_;
	p_memory->Initialize( memory_size );

	n_pipes = pn;
	p_pipes = new SDT_Pipe[ n_pipes ];

	for ( int i = 0; i < n_pipes; i ++ ) {
		p_pipes[i].SetMaxError( max_error );
		p_pipes[i].SetMemoryCallback( p_memory );
		p_pipes[i].SetOwner( this );
	}

	p_dummy_dataline = new f32[ n_pipes ];

	b_inited = true;
}

void MSDT::Destroy() {
	if ( !b_inited ) return;

	delete p_memory;
	delete[] p_pipes;
	delete[] p_dummy_dataline;

	p_memory = NULL;
	p_pipes = NULL;
	b_inited = false;
}

void MSDT::PushBack( const sdt_data* const data ) {
	for ( int i = 0; i < n_pipes; i ++ ) {
		p_pipes[i].PushBack( data[i] );
	}

	CleanUpMemory();
}

int MSDT::GetValidDataLength() const {
	int ret = 0x7FFFFFFF;

	for ( int i = 0; i < n_pipes; i ++ ) {
		if ( ret > p_pipes[i].GetLength() ) {
			ret = p_pipes[i].GetLength();
		}
	}

	return ret;
}

bool MSDT::GetData( sdt_data* data, int index ) const {
	int valid_data_length = GetValidDataLength();
	if ( index >= valid_data_length || index < -valid_data_length ) return false;

	if ( index > 0 ) {
		index -= valid_data_length;
	}

	for ( int i = 0; i < n_pipes; i ++ ) {
		data[i] = p_dummy_dataline[i] = p_pipes[i].GetData( index );
	}

	return true;
	//return p_dummy_dataline;
}

void MSDT::Flush() {
	for ( int i = 0; i < n_pipes; i++ ) {
		p_pipes[i].Flush();
	}

	return;
}

void MSDT::CleanUpMemory() {
	//OSReport( "replay memory used percent %f\n", p_memory->GetUsedPercent() );

	SDT_Pipe* pp = NULL;
	int size = 0;

	while ( p_memory->GetUsedPercent() > 0.97 ) {
		pp = NULL;
		size = 0;
		for ( int i = 0; i < n_pipes; i ++ ) {
			if ( p_pipes[i].GetMemorySize() > 1 && p_pipes[i].GetLength() - p_pipes[i].GetFirstUnitLength() > size ) {
				size = p_pipes[i].GetLength() - p_pipes[i].GetFirstUnitLength();
				pp = &p_pipes[i];
			}
		}

		pp->PopFirstUnit();
	}
	

	return;
}


