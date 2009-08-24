#include "render.h"
#include "view.h"
#include "win32.h"
#include "opengl.h"
#include "color.h"
#include "vector.h"
#include <cassert>
#include <cstdlib>

#include "log.h"

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
	LightIdx = -1;

	Viewport.posx = 0, Viewport.posy = 0;
	winhandle->GetDimensions( Viewport.width, Viewport.height );

	DefaultView = new View( Viewport.width, Viewport.height );
	DefaultView->Translate( 0.0f, 0.0f, 10.0f );
	CurrentView = DefaultView;

	// if this is the first renderer, we will set it active
	if ( Renderer::Curr == NULL ) {
		InitGL();
		Activate();
	}
}

Renderer::Renderer( int x, int y, int width, int height )
{
	LightIdx = -1;
	// create a default veiw
	DefaultView = new View( width, height );
	DefaultView->Translate( 0.0f, 0.0f, 10.0f );
	CurrentView = DefaultView;

	// if this is the first renderer, activate this renderer
	if ( Renderer::Curr == NULL ) {
		InitGL();
		Activate();
	}

	Viewport.posx = x, Viewport.posy = y;
	Viewport.width = width;
	Viewport.height = height;
}

Renderer::~Renderer()
{
	if ( Renderer::Curr == this ) {
		Renderer::Curr = NULL; 
	}
	if ( DefaultView != NULL )
		delete DefaultView;
}

void Renderer::Resize( int x, int y, int wid, int hit ) {
	Viewport.posx = x;
	Viewport.posy = y;
	Viewport.width = wid;
	Viewport.height = hit;

	if ( Renderer::Curr == this ) {
		glViewport( Viewport.posx, Viewport.posy, Viewport.width, Viewport.height );
	}
}

void Renderer::Activate() {
	if ( Renderer::Curr == this ) return;

	// Remove the state of last renderer
	if ( Renderer::Curr != NULL ) {
		Renderer::Curr->DisableAllLights();
		Renderer::Curr->GetView()->Deactivate();
	}

	glViewport( Viewport.posx, Viewport.posy, Viewport.width, Viewport.height );
	CurrentView->Activate();
	Renderer::Curr = this;

	this->EnableAllLights();
}

void Renderer::Deactivate() {
	if ( Renderer::Curr != this ) return;

	this->DisableAllLights();
	this->GetView()->Deactivate();
	
	Renderer::Curr = NULL;
}

void Renderer::DisableAllLights() const {
	glDisable( GL_LIGHTING );
	for ( int i = 0; i <= LightIdx; i ++ ) {
		glDisable( LightType[ i ] );
	}
}

void Renderer::EnableAllLights() const {
	if ( LightIdx == -1 ) {
		glDisable( GL_LIGHTING );
		return;
	}

	glEnable( GL_LIGHTING );
	for ( int i = 0; i <= LightIdx; i++ ) {
		glEnable( LightType[ i ] );
		glLightfv( LightType[ i ], GL_AMBIENT, Lights[i]->getAmbient().getdata() );		// Setup The Ambient Light
		glLightfv( LightType[ i ], GL_DIFFUSE, Lights[i]->getDiffuse().getdata() );		// Setup The Diffuse Light
		glLightfv( LightType[ i ], GL_POSITION,Lights[i]->getPos().getdata() );	// Position The Light
	}
}

bool Renderer::AddLight( Light *lit ) {
	assert( lit );
	if ( lit == NULL ) return false;
	if ( LightIdx >= MAX_LIGHTS - 1 ) return false;
	Lights[ ++LightIdx ] = lit;

	if ( Renderer::Curr == this ) {
		glEnable( GL_LIGHTING );
		glEnable( LightType[ LightIdx ] );
		glLightfv( LightType[ LightIdx ], GL_AMBIENT, lit->getAmbient().getdata() );		// Setup The Ambient Light
		glLightfv( LightType[ LightIdx ], GL_DIFFUSE, lit->getDiffuse().getdata() );		// Setup The Diffuse Light
		glLightfv( LightType[ LightIdx ], GL_POSITION,lit->getPos().getdata() );	// Position The Light
	}
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
			if ( Renderer::Curr == this ) {
				this->DisableAllLights();
				this->EnableAllLights();
			}
			if ( LightIdx == -1 ) {
				glDisable( GL_LIGHTING );
			}
			return true;
		}
	}
	return false;
}

View* Renderer::GetView() {
	return CurrentView;
}

bool Renderer::SetView( View* v ) {
	if ( v == NULL ) return false;

	CurrentView->Deactivate();
	CurrentView = v;
	if ( Renderer::Curr == this ) 
		CurrentView->Activate();
	return true;
}

void Renderer::ResetView() {
	if ( Renderer::Curr == this )
		DefaultView->Activate();
}

int Renderer::GetNumLights() const {
	return LightIdx + 1;
}

bool Renderer::IsLightEnabled() const {
	return GetNumLights() != 0;
}
