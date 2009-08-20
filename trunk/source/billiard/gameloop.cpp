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
#include "timer.h"
#include <cmath>
#include <cstdio>

bool MainLoop()
{
	return true;
}

int DrawGLScene(unsigned int x, unsigned int y, unsigned int width, unsigned int height) 	// Here's Where We Do All The Drawing
{
	glTranslatef(-1.5f,0.0f,0.0f);						// Move Left 1.5 Units And Into The Screen 6.0
	glBegin(GL_TRIANGLES);								// Start Drawing A Triangle
		glColor3f(1.0f,0.0f,0.0f);						// Set Top Point Of Triangle To Red
		glVertex3f( 0.0f, 1.0f, 0.0f);					// First Point Of The Triangle
		glColor3f(0.0f,1.0f,0.0f);						// Set Left Point Of Triangle To Green
		glVertex3f(-1.0f,-1.0f, 0.0f);					// Second Point Of The Triangle
		glColor3f(0.0f,0.0f,1.0f);						// Set Right Point Of Triangle To Blue
		glVertex3f( 1.0f,-1.0f, 0.0f);					// Third Point Of The Triangle
	glEnd();											// Done Drawing The Triangle
	glTranslatef(3.0f,0.0f,0.0f);						// Move Right 1.5 Units And Into The Screen 6.0
	glColor3f(0.5f,0.5f,1.0f);							// Set The Color To Blue One Time Only
	glBegin(GL_QUADS);									// Draw A Quad
		glVertex3f(-1.0f, 1.0f, 0.0f);					// Top Left
		glVertex3f( 1.0f, 1.0f, 0.0f);					// Top Right
		glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
		glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
	glEnd();											// Done Drawing The Quad
	glTranslatef(-1.5f,0.0f,0.0f);						// Move Right 1.5 Units And Into The Screen 6.0
	glFlush();
	return TRUE;										// Keep Going
}

static Ball *redball, *blueball, *greenball;
static Table *table;

void GameDataInit() {
	redball = new Ball( color( 1.0, 0.0, 0.0, 1.0 ) );
	greenball = new Ball( color( 0.0, 1.0, 0.0, 1.0 ) );
	blueball = new Ball( color( 0.0, 0.0, 1.0, 1.0 ) );
	table = new Table();

	redball->pos += vector3( 10.0, 0.0, 0.0 );
	greenball->pos += vector3( 0.0, 10.0, 0.0 );
	blueball->pos += vector3( -10.0, 0.0, 0.0 );

	redball->AssignRandomSpeed();
	greenball->AssignRandomSpeed();
	blueball->AssignRandomSpeed();
}

void GameDataDeinit() {
	delete redball;
	delete greenball;
	delete blueball;
	delete table;
}

void GameUpdate( float deltaTime ) {
	Collision_twoBall( redball, greenball );
	Collision_twoBall( greenball, blueball );
	Collision_twoBall( redball, blueball );

	Collision_withTable( redball, table );
	Collision_withTable( greenball, table );
	Collision_withTable( blueball, table );

	redball->Update( deltaTime );
	greenball->Update( deltaTime );
	blueball->Update( deltaTime );
}

void GameDraw( Renderer* rd ) {
	rd->ResetView(); // we must reset the view, or opengl will use the current modelview matrix, how could make it be better look
	table->Draw();

	rd->ResetView();
	redball->Draw();

	rd->ResetView();
	greenball->Draw();

	rd->ResetView();
	blueball->Draw();
}


int main()
{
	bool Running = true;
	MSG 	msg;

	WindowsHandle winHandle;

	Log::print( "fuck you, Tangel" );
	if ( !winHandle.InitWindow( "BILLIARD", 100, 100, 640, 480, false ) ) {
		Log::print( "create window failed" );
	} else {
		Log::print( "create window successful" );
	}

	Renderer* render1 = new Renderer( &winHandle );
	View* view1 = render1->GetView();   // this is the default view created by render1
	view1->Translate( 0.0, 0.0, 100.0 );
	int winWidth, winHeight;
	winHandle.GetDimensions( winWidth, winHeight );
	view1->Perspective( 45.0, (float)winWidth / winHeight, 0.1, 500.0 );

	View* view2 = new View( 640, 480 );
	view2->Translate( 0.0, 0.0, 10.0 );

	Renderer* render2 = new Renderer( 0, 0, 300, 200 );
	render2->GetView()->Translate( 0.0, 0.0, 60.0 );
	render2->GetView()->Rotate( 90.0, 1.0, 0.0, 0.0 );
	render2->GetView()->Translate( 0.0, 0.0, 10.0 );

	//glTranslatef(-1.5f,0.0f,-6.0f);						// Move Left 1.5 Units And Into The Screen 6.0
	//color red( 1.0, 0.0, 0.0, 1.0 );
	Frame GameFrame;
	float deltatime;

	GameDataInit();

	int SwapCount = 0;
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

		// no matter which render calls clearscreen(), the whole screen will be cleared
		render1->ClearScreen();

		/*if ( view1->IsActive() )
			view1->Rotate( 1.0, 1.0, 0.0, 0.0 );
		else if ( view2->IsActive() ) 
			view2->Rotate( 1.0, 0.0, 0.0, 1.0 );
			*/
		//view1->Rotate( 1.0, 0.0, 0.0, 1.0 );
		table->colour.setRGB( 0.5, 0.5, 0.5 );
		//DrawGLScene(0, 0, 500, 300);
		GameDraw( render1 );

		render2->Activate();
		render2->GetView()->Rotate( -1.0, 0.0, 0.0, 1.0 );
		glClear( GL_DEPTH_BUFFER_BIT);	// Clear The Depth Buffer, so render2 could cover render1
		table->colour.setRGB( 0.8, 0.8, 0.2 );
		
		GameDraw( render2 );
		//DrawGLScene(0, 0, 500, 500 );
		
		winHandle.SwapBuffer();
		
		GameFrame.Update();
		deltatime = GameFrame.GetLength();

		GameUpdate( 33.0 / 1000.0 );

		render1->Activate();
		if ( deltatime < 33.0 ) {
			Sleep( static_cast<DWORD> (33.0 - deltatime) );
			/*SwapCount ++;

			if ( SwapCount > 30 ) {
				SwapCount = 0;
				if ( view1->IsActive() ) {
					Log::print( "view changed, now is view2" );
					render1->SetView( view2 );
				} else {
					Log::print( "view changed, now is view1" );
					render1->SetView( view1 );
				}
			}*/
		}
	}

	GameDataDeinit();

	delete view1;
	delete view2;
	delete render1;
	delete render2;

	winHandle.KillWindow();
	printf( "end\n" );

	return 0;
}

