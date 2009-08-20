#include "view.h"
#include "opengl.h"
#include "log.h"
#include <cstdlib>

// it's not a good idea to use the default width and height, how could it be better
//#define DEFAULT_WIDTH 640
//#define DEFAULT_HEIGHT 480
//the default constructor is declared private

static float matrixdata[16];

void View::Activate() {
	// Set this matrix as working matrix
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( this->Projection.getdata() );
	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( this->Modelview.getdata() );

	this->isActive = true;
}

void View::Deactivate() {
	this->isActive = false;
}

bool View::IsActive() {
	return this->isActive;
}

View::View( int width, int height ) {
	isActive = false;
	AspectRatio = (float)width / (float)height;

	// The process of generating projection matrix is not implemented yet, 
	// So I have to use GL to compute the projection matrix
	// TODO: implement this module
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluPerspective( 45.0f, (GLfloat) width / (GLfloat) height, 0.1f, 100.0f );
	glGetFloatv( GL_PROJECTION_MATRIX, matrixdata );
	this->Projection.setdata( matrixdata );
	glPopMatrix();

	this->Modelview.identity();
}

View::~View() {
}

void View::Reset() {
	if ( isActive ) {
		glMatrixMode( GL_PROJECTION );
		gluPerspective(45.0f,AspectRatio,0.1f,100.0f);
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
	} 
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluPerspective( 45.0f, AspectRatio, 0.1f, 100.0f );
	glGetFloatv( GL_PROJECTION_MATRIX, matrixdata );
	this->Projection.setdata( matrixdata );
	glPopMatrix();

	this->Modelview.identity();
}

void View::Translate( float x, float y, float z ) {
	if ( isActive ) {
		glMatrixMode( GL_MODELVIEW );
		glTranslatef( -x, -y, -z );
	} 
	this->Modelview.translate( -x, -y, -z );
}

void View::Rotate( float angle, float x, float y, float z ) {
	if ( isActive ) {
		glMatrixMode( GL_MODELVIEW );
		glRotatef( -angle, x, y, z );
	}
	this->Modelview.rotate_xyz( x, y, z, -angle );
}

void View::Scale( float x, float y, float z ) {
	if ( isActive ) {
		glMatrixMode( GL_MODELVIEW );
		glScalef( x, y, z );
	}
	this->Modelview.scale( x, y, z );
}

void View::LookAt( vector3 pos, vector3 at, vector3 up ) {
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(pos.getX(),pos.getY(),pos.getZ(),at.getX(),at.getY(),at.getZ(),up.getX(),up.getY(),up.getZ());
	glGetFloatv( GL_MODELVIEW_MATRIX, matrixdata );
	this->Projection.setdata( matrixdata );
	glPopMatrix(); 
	
	if ( isActive ) {
		glLoadIdentity();
		gluLookAt(pos.getX(),pos.getY(),pos.getZ(),at.getX(),at.getY(),at.getZ(),up.getX(),up.getY(),up.getZ());
	} 
}

/*
void View :: Pos2D(int x,int y)
{
	glRasterPos2i(x,y);
}

void View :: Pos2D(float x,float y)
{
	glRasterPos2f(x,y);
}

void View :: Save()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
}

void View :: Restore()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
*/
