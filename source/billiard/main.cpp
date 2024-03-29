#include "gl\glew.h"
#include "timer.h"

#include "ball.h"
#include "vector.h"
#include "collision.h"
#include "table.h"
#include "win32.h"
#include "opengl.h"
#include "log.h"
#include "color.h"
#include "render.h"
#include "view.h"
#include "light.h"
#include "control.h"
#include "shadow.h"
#include "zmath.h"
#include "shader.h"
#include <cmath>
#include <cstdio>

bool (*_GameMainLoop_)( float deltatime );
typedef void (*INITFUNC)();
INITFUNC _GameInit_, _GameDeinit_;

const float DELTATIME = 33.0 / 1000.0;

#include "game1.game"

void Init() {
	// we must init _GameMainLoop_(), _GameInit_(), _GameDeinit_() here;
	_GameMainLoop_ = gameloop;
	_GameInit_ = gameinit;
	_GameDeinit_ = gamedeinit;
}

#ifdef _USE_HEAP2_
void* memory;
void InitMemory() {
	const int INIT_MEMORY_SIZE = 1 << 20;
	void* memory = (void*)malloc( INIT_MEMORY_SIZE );
	if ( memory == NULL ) printf( "!!!!!FATAL ERROR!!!!!\n" );
	InitHeap( memory, INIT_MEMORY_SIZE );
}

void DeinitMemory() {
	CheckLeakPoint();
	if ( memory != NULL ) 
		free( memory );
}
#endif 

int main() {
#ifdef _USE_HEAP2_
	InitMemory();
#endif
	bool Running = true;
	float deltatime = DELTATIME;
	MSG 	msg;

	Frame GameFrame;
	// glewinit() must be called after the window is initialized, so it should be called in gameinit

	Init();
	
	_GameInit_();

	while ( Running )
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				Running = false;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}

		deltatime = GameFrame.GetLength();
		if ( !_GameMainLoop_( DELTATIME ) ) break;

		if ( deltatime < DELTATIME ) {
			Sleep( static_cast<DWORD> ( DELTATIME - deltatime) );
		}
	}

	_GameDeinit_();
#ifdef _USE_HEAP2_
	DeinitMemory();
#endif
	return 0;
}

