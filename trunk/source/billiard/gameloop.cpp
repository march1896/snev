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
#include "light.h"
#include "control.h"
#include "shadow.h"
#include "zmath.h"
#include <cmath>
#include <cstdio>

bool MainLoop()
{
	return true;
}

int DrawGLScene(unsigned int x, unsigned int y, unsigned int width, unsigned int height) 	// Here's Where We Do All The Drawing
{
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );

	/*
	glTranslatef(-1.5f,0.0f,0.0f);						// Move Left 1.5 Units And Into The Screen 6.0
	glBegin( GL_TRIANGLE_FAN );
		glVertex3f(-1.0f, 1.0f, 0.0f);					// Top Left
		glVertex3f( 1.0f, 1.0f, 0.0f);					// Top Right
		glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
		glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
	glEnd();
	*/
	
	glBegin(GL_TRIANGLES);								// Start Drawing A Triangle
		glColor3f(1.0f,0.0f,0.0f);						// Set Top Point Of Triangle To Red
		glNormal3f( 0.0, 0.0, 1.0f );
		glVertex3f( 0.0f, 1.0f, 0.0f);					// First Point Of The Triangle
		glColor3f(0.0f,1.0f,0.0f);						// Set Left Point Of Triangle To Green
		glNormal3f( 0.0, 0.0, 1.0f );
		glVertex3f(-1.0f,-1.0f, 0.0f);					// Second Point Of The Triangle
		glColor3f(0.0f,0.0f,1.0f);						// Set Right Point Of Triangle To Blue
		glNormal3f( 0.0, 0.0, 1.0f );
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


static const int N_BALLS = 20;
static Ball *redball, *blueball, *greenball, *ballArray[N_BALLS];
static Table *table;
static Light* lit;

void GameDataInit() {
	for ( int i = 0; i < N_BALLS; i ++ ) {
		ballArray[ i ] = new Ball();
		ballArray[ i ]->AssignRandomSpeed();
		ballArray[ i ]->AssignRandomColor();
		ballArray[ i ]->pos += vector3( -40.0 + i * 4, 0.0, 0.0 );
	}
	table = new Table();

	lit = new Light( vector4( 0.0, 0.0, 10.0, 1.0 ), 
			color( 0.1, 0.1, 0.1, 1.0 ),
			color( 0.8, 0.8, 0.8, 1.0 ),
			color( 0.0, 0.0, 0.0, 1.0 )
		       );

	/*redball = new Ball( color( 1.0, 0.0, 0.0, 1.0 ) );
	greenball = new Ball( color( 0.0, 1.0, 0.0, 1.0 ) );
	blueball = new Ball( color( 0.0, 0.0, 1.0, 1.0 ) );

	redball->pos += vector3( 10.0, 10.0, 0.0 );
	greenball->pos += vector3( 0.0, 10.0, 0.0 );
	blueball->pos += vector3( -10.0, 10.0, 0.0 );

	redball->AssignRandomSpeed();
	greenball->AssignRandomSpeed();
	blueball->AssignRandomSpeed();*/
}

void GameDataDeinit() {
	/*
	delete redball;
	delete greenball;
	delete blueball;
	*/

	for ( int i = 0; i < N_BALLS; i ++ ) { 
		delete ballArray[i];
	}
	delete table;
	delete lit;
}

void GameUpdate( float deltaTime ) {
	/*
	redball->Update( deltaTime );
	greenball->Update( deltaTime );
	blueball->Update( deltaTime );

	Collision_twoBall( redball, greenball );
	Collision_twoBall( greenball, blueball );
	Collision_twoBall( redball, blueball );

	Collision_withTable( redball, table );
	Collision_withTable( greenball, table );
	Collision_withTable( blueball, table );
	*/

	for ( int i = 0; i < N_BALLS; i ++ ) {
		ballArray[ i ]->Update( deltaTime );
	}

	for ( int i = 0; i < N_BALLS; i ++ ) {
		for ( int j = i + 1; j < N_BALLS; j ++ ) {
			Collision_twoBall( ballArray[ i ], ballArray[ j ] );
		}
	}

	for ( int i = 0; i < N_BALLS; i ++ ) {
		Collision_withTable( ballArray[ i ], table );
	}
}

void MoveCamera( View* view, Control *control ) {
	if ( control->IsPressed( 'W' ) || control->IsPressed( 'w' ) ) {
		view->Rotate( -1.0, 1.0, 0.0, 0.0 );
	}
	if ( control->IsPressed( 'S' ) || control->IsPressed( 's' ) ) {
		view->Rotate( 1.0, 1.0, 0.0, 0.0 );
	}
	if ( control->IsPressed( 'A' ) || control->IsPressed( 'a' ) ) {
		view->Rotate( -1.0, 0.0, 1.0, 0.0 );
	}
	if ( control->IsPressed( 'D' ) || control->IsPressed( 'd' ) ) {
		view->Rotate( 1.0, 0.0, 1.0, 0.0 );
	}
	if ( control->IsPressed( 'Q' ) || control->IsPressed( 'q' ) ) {
		view->Translate( 0.0, 0.0, 1.0 );
	}
	if ( control->IsPressed( 'E' ) || control->IsPressed( 'e' ) ) {
		view->Translate( 0.0, 0.0, -1.0 );
	}
}

void GameDraw( Renderer* rd, Light *lit = NULL ) {
	/*
	rd->ResetView();
	redball->Draw();

	rd->ResetView();
	greenball->Draw();

	rd->ResetView();
	blueball->Draw();
	*/

	rd->ResetView(); // we must reset the view, or opengl will use the current modelview matrix, how could make it be better look
	table->Draw( rd );

	if ( lit != NULL ) 
		ConstructShadowModel( ballArray[0] );

	for ( int i = 0; i < N_BALLS; i ++ ) {
		rd->ResetView();
		ballArray[ i ]->Draw( rd ); 

		// the model is constructed succeed
		//DrawShadowModle( ballArray[i] );
		// has been tested
	}		

	//glClear( GL_COLOR_BUFFER_BIT );
	PreDrawShadow();
	for ( int i = 0; i < N_BALLS; i ++ ) {
		if ( lit != NULL ) {
			rd->ResetView();
			DrawShadowVolume( ballArray[i], lit );
		}
	}
	DrawShadow();
	//EndDrawShadow();

	/*
	glDisable( GL_LIGHTING );
	glEnable( GL_STENCIL_TEST );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glColor4f( 0.0f, 0.0f, 1.0f, 0.4f );
	glEnable( GL_BLEND );
	glStencilFunc( GL_NEVER, 0, 0xffffffff );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	glPushMatrix();
	glLoadIdentity();
	printf( "stencil buffer is enabled: %s\n", glIsEnabled( GL_STENCIL_TEST ) ? "true" : "false" );
	glBegin( GL_TRIANGLE_STRIP );
		glVertex3f(-1.1f, 1.1f,-1.0f);
		glVertex3f(-1.1f,-1.1f,-1.0f);
		glVertex3f( 1.1f, 1.1f,-1.0f);
		glVertex3f( 1.1f,-1.1f,-1.0f);
	glEnd();
	glPopMatrix();
	glDisable( GL_BLEND );

	glDepthMask( GL_TRUE );
	glDisable( GL_STENCIL_TEST );
	glEnable( GL_LIGHTING );
	*/
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

	GameDataInit();

	Control* control = new Control( &winHandle );

	Renderer* render1 = new Renderer( &winHandle );
	View* view1 = render1->GetView();   // this is the default view created by render1
	render1->AddLight( lit );
	view1->Translate( 0.0, 0.0, 100.0 );
	view1->Rotate( 70.0, 1.0, 0.0, 0.0 );
	//render2->GetView()->Rotate( 70.0, 1.0, 0.0, 0.0 );
	int winWidth, winHeight;
	winHandle.GetDimensions( winWidth, winHeight );
	//view1->Perspective( 45.0, (float)winWidth / winHeight, 0.1, 500.0 );

	View* view2 = new View( 640, 480 );
	view2->Translate( 0.0, 0.0, 10.0 );

	Renderer* render2 = new Renderer( 0, 0, 200, 150 );
	
	render2->GetView()->Translate( 0.0, 0.0, 100.0 );
	//render2->GetView()->Rotate( 70.0, 1.0, 0.0, 0.0 );
	//render2->GetView()->Translate( 0.0, 0.0, 30.0 );

	//glTranslatef(-1.5f,0.0f,-6.0f);						// Move Left 1.5 Units And Into The Screen 6.0
	//color red( 1.0, 0.0, 0.0, 1.0 );
	Frame GameFrame;
	float deltatime;

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

		int width, height;
		//printf("%d\t%d\n", width, height );
		winHandle.GetDimensions( width, height );
		render1->Resize( 0, 0, width, height );

		// no matter which render calls clearscreen(), the whole screen will be cleared
		render1->ClearScreen();

		/*if ( view1->IsActive() )
			view1->Rotate( 1.0, 1.0, 0.0, 0.0 );
		else if ( view2->IsActive() ) 
			view2->Rotate( 1.0, 0.0, 0.0, 1.0 );
			*/
		/*
		glEnable( GL_CULL_FACE );
		glFrontFace( GL_CWW );
		glCullFace( GL_BACK );
		*/

		/*
		render1->SetView( view2 );
		glDisable( GL_LIGHTING );
		DrawGLScene(0, 0, 500, 300);
		*/

		render1->GetView()->Rotate( -0.1, 0.0, 0.0, 1.0 );
		GameDraw( render1, lit );

		render2->Activate();
		glClear( GL_DEPTH_BUFFER_BIT);	// Clear The Depth Buffer, so render2 could cover render1
		//table->colour.setRGB( 0.2, 0.6, 0.6 );
		
		GameDraw( render2 );
		//DrawGLScene(0, 0, 500, 500 );
		
		winHandle.SwapBuffer();
		
		GameFrame.Update();
		deltatime = GameFrame.GetLength();


		render1->Activate();

		MoveCamera( render1->GetView(), control );

		// prite the input message
		{
			int x, y;
			bool left, right;
			control->GetMousePosition( x, y );
			control->GetMousePressed( left, right );
			//printf( "Mouse Position: %d\t%d\n", x, y );
			//printf( "Mouse Pressed: %s   %s\n", left ? "true" : "false", right ? "true" : "false" );
			
		}

		const float DELTATIME = 33.0;

		GameUpdate( DELTATIME / 1000.0 );
		if ( deltatime < DELTATIME ) {
			Sleep( static_cast<DWORD> ( DELTATIME - deltatime) );
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

	delete control;
	delete view1;
	delete view2;
	delete render1;
	delete render2;

	winHandle.KillWindow();
	printf( "end\n" );

	return 0;
}

