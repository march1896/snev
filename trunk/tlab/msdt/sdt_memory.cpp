#include "sdt_memory.h"
#include <cstdlib>
#include <cstring>

SDT_Memory::SDT_Memory() :
	p_buffer( NULL ),
	p_curr( NULL ),
	n_total_size( 0 ),
	n_used_size( 0 ),
	b_inited( false )
{
}

SDT_Memory::~SDT_Memory() {
	if ( b_inited ) {
		Destroy();
	}
}

void SDT_Memory::Initialize( int size ) {
	if ( b_inited ) return;

	// get memory by system call;
#ifdef __PC__
	p_buffer = (char*)malloc( size );
#elif __WII__
#endif
	
	if ( !p_buffer ) return; 
	memset( p_buffer, 0, size );

	int block_size = sizeof( sdt_frame_unit );
	sdt_frame_unit* fu = ( sdt_frame_unit* )p_buffer;
	n_total_size = size / block_size;

	for ( int i = 0; i < n_total_size; i ++ ) {
		fu->next = fu + 1;
		fu = fu->next;
	}
	fu->next = NULL;

	p_curr = ( sdt_frame_unit* )p_buffer;
	b_inited = true;
}

void SDT_Memory::Destroy() {
	if ( !b_inited ) return;

#ifdef __PC__
	if ( p_buffer ) free( p_buffer );
#elif __WII__
#endif

	b_inited = false;
}

sdt_frame_unit* SDT_Memory::Alloc() {
	if ( p_curr == NULL ) return NULL;

	sdt_frame_unit* ret = p_curr;
	p_curr = p_curr->next;
	n_used_size ++;

	return ret;
}

void SDT_Memory::Dealloc( void* p ) {
	sdt_frame_unit* addr = (sdt_frame_unit*)p;
	if ( addr == NULL ) return;

	if ( p_curr == NULL ) {
		p_curr = addr;
	}
	else {
		addr->next = p_curr->next;
		p_curr->next = addr;
	}

	n_used_size --;
	return;
}

f32 SDT_Memory::GetUsedPercent() const {
	return (f32)n_used_size / n_total_size;
}

