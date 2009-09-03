#include "table.h"
#include "opengl.h"
#include "render.h"

Table::Table():
	Width( 100.0 ), Height( 60.0 ),
	Left( -50.0 ), Right( 50.0 ), Top( 30.0 ), Bottom( -30.0 ),
	pos( 0.0, 0.0, 0.0 ),
	normal( 0.0, 1.0, 0.0 ),
	colour( 0.5, 0.5, 0.5, 1.0 ),
	mat( color( 0.5, 0.5, 0.5, 1.0 ) ),
	foslide( 0.1 ),
	foroll( 0.1 )
{
}

Table::Table( float _weight, float _height ):
	Width( _weight ), Height( _height ),
	pos( 0.0, 0.0, 0.0 ),
	normal( 0.0, 1.0, 0.0 ),
	colour( 0.5, 0.5, 0.5, 1.0 ),
	mat( color( 0.5, 0.5, 0.5, 1.0 ) ),
	foslide( 0.1 ),
	foroll( 0.1 )
{
	Left = -Width / 2.0;
	Right = Width / 2.0;
	Top = Height / 2.0;
	Bottom = -Height / 2.0;
}

Table::Table( float _weight, float _height, const color& _col, const vector3 &_pos, const vector3 &_normal ):
	Width( _weight ), Height( _height ),
	pos( _pos ),
	normal( _normal ),
	colour( _col ),
	mat( _col ),
	foslide( 0.1 ),
	foroll( 0.1 )
{
	Left = -Width / 2.0;
	Right = Width / 2.0;
	Top = Height / 2.0;
	Bottom = -Height / 2.0;
}

void Table::Draw() {
	glColor4fv( colour.getdata() );							// Set The Color To Blue One Time Only
	glBegin(GL_QUADS);									// Draw A Quad
		glNormal3f( 0.0, 0.0, 1.0 );
		glVertex3f( Left, Bottom, 0.0f);					// Top Left
		glVertex3f( Right, Bottom, 0.0f);					// Top Right
		glVertex3f( Right, Top, 0.0f);					// Bottom Right
		glVertex3f( Left, Top, 0.0f);					// Bottom Left
	glEnd();
}

void Table::Draw( Renderer* rd ) {
	if ( rd->IsLightEnabled() ) {
		float shininess[] = { mat.GetShininess() };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat.GetAmbient().getdata() );			// Set Material Ambience
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.GetDiffuse().getdata() );			// Set Material Diffuse
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat.GetSpecular().getdata() );		// Set Material Specular
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess );		// Set Material Shininess
	} else {
		glColor4fv( colour.getdata() );
	}

	glBegin(GL_QUADS);									// Draw A Quad
		glVertex3f( Left, Bottom, 0.0f);					// Top Left
		glVertex3f( Right, Bottom, 0.0f);					// Top Right
		glVertex3f( Right, Top, 0.0f);					// Bottom Right
		glVertex3f( Left, Top, 0.0f);					// Bottom Left
	glEnd();
}

Table::~Table()
{
}



