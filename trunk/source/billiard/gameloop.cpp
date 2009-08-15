#include "ball.h"
#include "vector.h"
#include "collision.h"
#include "table.h"
#include "win32.h"
#include "opengl.h"
#include "log.h"
#include <cstdio>

static Ball* pBalls;
static Table* pTable;

void GameInit()
{
	pBalls = new Ball[3];
	//pBalls[0].SetAllAttributes( 0.0, 0.0, 1.0, 0.0, 0.0, WHITE );
	//pBalls[1].SetAllAttributes( 1.0, 1.0, 1.0, 0.0, 0.0, BLACK );
	//pBalls[2].SetAllAttributes( 2.0, 2.0, 1.0, 0.0, 0.0, RED );

	pTable = new Table;
}

void GameDeinit()
{
	delete[] pBalls;
	delete pTable;
}

bool MainLoop()
{
	return true;
}

int DrawGLScene(unsigned int x, unsigned int y, unsigned int width, unsigned int height) 	// Here's Where We Do All The Drawing
{
	glViewport(x,y,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
	

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glClear( GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(-1.5f,0.0f,-6.0f);						// Move Left 1.5 Units And Into The Screen 6.0
	glRotatef(0.0f,0.0f,1.0f,0.0f);						// Rotate The Triangle On The Y axis ( NEW )
	glBegin(GL_TRIANGLES);								// Start Drawing A Triangle
		glColor3f(1.0f,0.0f,0.0f);						// Set Top Point Of Triangle To Red
		glVertex3f( 0.0f, 1.0f, 0.0f);					// First Point Of The Triangle
		glColor3f(0.0f,1.0f,0.0f);						// Set Left Point Of Triangle To Green
		glVertex3f(-1.0f,-1.0f, 0.0f);					// Second Point Of The Triangle
		glColor3f(0.0f,0.0f,1.0f);						// Set Right Point Of Triangle To Blue
		glVertex3f( 1.0f,-1.0f, 0.0f);					// Third Point Of The Triangle
	glEnd();											// Done Drawing The Triangle
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(1.5f,0.0f,-6.0f);						// Move Right 1.5 Units And Into The Screen 6.0
	glRotatef(0.0f,1.0f,0.0f,0.0f);					// Rotate The Quad On The X axis ( NEW )
	glColor3f(0.5f,0.5f,1.0f);							// Set The Color To Blue One Time Only
	glBegin(GL_QUADS);									// Draw A Quad
		glVertex3f(-1.0f, 1.0f, 0.0f);					// Top Left
		glVertex3f( 1.0f, 1.0f, 0.0f);					// Top Right
		glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
		glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
	glEnd();											// Done Drawing The Quad
	glFlush();
	return TRUE;										// Keep Going
}


int main()
{
	bool Running = true;
	MSG 	msg;

	GameInit();

	printf("hello, world\n");

	WindowsHandle winHandle;
	if ( !winHandle.InitWindow( "BILLIARD", 100, 100, 640, 480, false ) ) {
		printf( "create window failed\n" );
	} else {
		printf( "create successful\n" );
	}

	Log::print( "hello, world" );
	Log::print( "fuck you, Tangel" );

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
		//DrawGLScene(0, 0, 640, 480);
		//Log::print( "the pic has been drawed" );
		DrawGLScene(0, 0, 100, 100);
		DrawGLScene(100, 100, 100, 100);
		DrawGLScene(200, 200, 100, 100);
		DrawGLScene(300, 300, 100, 100);

		winHandle.SwapBuffer();
		
		//Running = MainLoop();
		//if ( !Running ) break;

		//if ( true ) break;
	}

	winHandle.KillWindow();
	GameDeinit();
	printf( "end\n" );

	return 0;
}

