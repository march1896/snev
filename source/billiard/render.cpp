#include "render.h"
#include "view.h"
#include "win32.h"
#include "opengl.h"
#include "color.h"
#include "vector.h"
#include <cassert>
#include <cstdlib>

GLenum LightType[ MAX_LIGHTS ] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3 };
Renderer* Renderer::Curr = NULL;

void Renderer::InitGL() {
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
}

// TODO: no matter which Renderer call clearScreen, the whole screen will be cleaned
// how to clean only the viewport
void Renderer::ClearScreen() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
}
	

Renderer::Renderer( WindowsHandle* winhandle ) {

	Viewport.posx = 0, Viewport.posy = 0;
	winhandle->GetDimensions( Viewport.width, Viewport.height );

	CurrentView = new View( Viewport.width, Viewport.height );
	if ( Renderer::Curr == NULL ) {
		Renderer::Curr = this;
		InitGL();
		ClearScreen();
		CurrentView->PrepareRendering();
		glViewport( Viewport.posx, Viewport.posy, Viewport.width, Viewport.height );
	}
	LightIdx = -1;
}

Renderer::Renderer( int x, int y, int width, int height )
{
	CurrentView = new View( width, height );
	if ( Renderer::Curr == NULL ) {
		Renderer::Curr = this;
		InitGL();
		ClearScreen();
		CurrentView->PrepareRendering();
		glViewport( Viewport.posx, Viewport.posy, Viewport.width, Viewport.height );
	}

	Viewport.posx = x, Viewport.posy = y;
	Viewport.width = width;
	Viewport.height = height;

	LightIdx = -1;
}

Renderer::~Renderer()
{
	if ( Renderer::Curr == this ) {
		Renderer::Curr = NULL; 
	}
	delete CurrentView;
}

void Renderer::SetAsWorking() {
	if ( Renderer::Curr == this ) return;

	// Remove the state of last renderer
	Renderer::Curr->DisableAllLights();

	glViewport( Viewport.posx, Viewport.posy, Viewport.width, Viewport.height );
	CurrentView->PrepareRendering();
	Renderer::Curr = this;
	this->EnableAllLights();
}

void Renderer::DisableAllLights() {
	for ( int i = 0; i <= LightIdx; i ++ ) {
		glDisable( LightType[ i ] );
	}
}

void Renderer::EnableAllLights() {
	for ( int i = 0; i <= LightIdx; i++ ) {
		glEnable( LightType[ ++ LightIdx ] );
		glLightfv( LightType[ LightIdx ], GL_AMBIENT, Lights[i]->getAmbient().getdata() );		// Setup The Ambient Light
		glLightfv( LightType[ LightIdx ], GL_DIFFUSE, Lights[i]->getDiffuse().getdata() );		// Setup The Diffuse Light
		glLightfv( LightType[ LightIdx ], GL_POSITION,Lights[i]->getPos().getdata() );	// Position The Light
	}
}

bool Renderer::AddLight( Light *lit ) {
	assert( lit );
	if ( lit == NULL ) return false;
	if ( LightIdx >= MAX_LIGHTS - 1 ) return false;

	glEnable( LightType[ ++ LightIdx ] );
	glLightfv( LightType[ LightIdx ], GL_AMBIENT, lit->getAmbient().getdata() );		// Setup The Ambient Light
	glLightfv( LightType[ LightIdx ], GL_DIFFUSE, lit->getDiffuse().getdata() );		// Setup The Diffuse Light
	glLightfv( LightType[ LightIdx ], GL_POSITION,lit->getPos().getdata() );	// Position The Light

	Lights[ LightIdx ] = lit;
}

bool Renderer::RemoveLight( Light *lit ) {
	assert( lit ); 
	if ( lit == NULL ) return false;

	for ( int i = 0; i < LightIdx; i ++ ) {
		if ( Lights[ i ] == NULL ) return false;

		if ( Lights[i] == lit ) {
			for ( int j = i; j < LightIdx; j ++ )
				Lights[ j ] = Lights[ j+1 ];
			LightIdx --;
			return true;
		}
	}
	return false;
}

View* Renderer::GetCurrentView() {
	return CurrentView;
}

int Renderer::GetNumLights() {
	return LightIdx + 1;
}
