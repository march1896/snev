#include "ball.h"
#include "vector.h"
#include "opengl.h"
#include "render.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

#include "log.h"

GLUquadricObj * quadratic=gluNewQuadric();	// Create A Pointer To The Quadric Object (Return 0 If No Memory) (NEW)


Ball::Ball():
	radius( 2.0 ), 
	weight( 10.0 ),
	colour( 0.5, 0.5, 0.5, 1.0 ),
	mat(),
	pos( 0.0, 0.0, 0.0 ),
	rotate( 1.0, 0.0, 0.0 ),
	speed( 1.0, 0.0, 0.0 )
{
}

Ball::Ball( color _color ):
	radius( 2.0 ), 
	weight( 10.0 ),
	colour( _color ),
	mat( _color ),
	pos( 0.0, 0.0, 0.0 ),
	rotate( 1.0, 0.0, 0.0 ),
	speed( 1.0, 0.0, 0.0 )
{
}

Ball::Ball( float _radius, float _weight, color _color, vector3 _pos ):
	radius( _radius ),
	weight( _weight ),
	colour( _color ),
	mat( _color ),
	pos( _pos ),
	rotate( 1.0, 0.0, 0.0 ),
	speed( 1.0, 0.0, 0.0 )
{
}

static float r2a( float radian ) // radian to angle 
{
	return radian / M_PI * 180.0f;
}

static float a2r( float angle ) // angle to radian
{ 
	return angle / 180.f * M_PI;
}

static float angle_sin( float angle ) {
	return sin( a2r( angle ) );
}

static float angle_cos( float angle ) {
	return cos( a2r( angle ) );
}

static void DrawSphere( float radius, float longitude, float latitude ) {
	float alpha, theta;
	
	glBegin( GL_TRIANGLE_FAN );
	{
		glNormal3f( 0.0f, 0.0, 1.0f );
		glVertex3f( 0.0f, 0.0f, radius );
		theta = 10.0f;
		for ( int i = 0; i <= longitude; i ++ ) {
			alpha = 360.0 / longitude * i;
			glNormal3f( radius * angle_sin( theta ) * angle_cos( alpha ), radius * angle_sin( theta ) * angle_sin( alpha ), radius * angle_cos( theta ) );
			glVertex3f( radius * angle_sin( theta ) * angle_cos( alpha ), radius * angle_sin( theta ) * angle_sin( alpha ), radius * angle_cos( theta ) );
		}
	}
	glEnd();

	glBegin( GL_TRIANGLE_FAN );
	{
		glNormal3f( 0.0f, 0.0, -1.0f );
		glVertex3f( 0.0f, 0.0f, -radius );
		theta = 170.0f;
		for ( int i = 0; i <= longitude; i ++ ) {
			alpha = 360.0 / longitude * i;
			glNormal3f( radius * angle_sin( theta ) * angle_cos( alpha ), radius * angle_sin( theta ) * angle_sin( alpha ), radius * angle_cos( theta ) );
			glVertex3f( radius * angle_sin( theta ) * angle_cos( alpha ), radius * angle_sin( theta ) * angle_sin( alpha ), radius * angle_cos( theta ) );
		}
	}
	glEnd();

	float theta2;
	for ( int i = 0; i < latitude; i ++ ) {
		theta = 10.0f + 160.0f / latitude * i ;
		theta2 = 10.0f + 160.0f / latitude * ( i + 1 ) ;
		glBegin( GL_TRIANGLE_STRIP );
		{
			glNormal3f( radius * angle_sin( theta ) , 0.0f, radius * angle_cos( theta ) );
			glVertex3f( radius * angle_sin( theta ) , 0.0f, radius * angle_cos( theta ) );
			glNormal3f( radius * angle_sin( theta2 ), 0.0f, radius * angle_cos( theta2 ) );
			glVertex3f( radius * angle_sin( theta2 ), 0.0f, radius * angle_cos( theta2 ) );
			for ( int j = 1; j <= longitude; j ++ ) {
				alpha = 360.0 / longitude * j;
				glNormal3f( radius * angle_sin( theta ) * angle_cos( alpha ), radius * angle_sin( theta ) * angle_sin( alpha ), radius * angle_cos( theta ) );
				glVertex3f( radius * angle_sin( theta ) * angle_cos( alpha ), radius * angle_sin( theta ) * angle_sin( alpha ), radius * angle_cos( theta ) );
				glNormal3f( radius * angle_sin( theta2 ) * angle_cos( alpha ), radius * angle_sin( theta2 ) * angle_sin( alpha ), radius * angle_cos( theta2 ) );
				glVertex3f( radius * angle_sin( theta2 ) * angle_cos( alpha ), radius * angle_sin( theta2 ) * angle_sin( alpha ), radius * angle_cos( theta2 ) );
			}
		}
		glEnd();
	}
}

void Ball::Draw() {
	glMatrixMode( GL_MODELVIEW );
	glTranslatef( pos.getX(), pos.getY(), pos.getZ() );
	glTranslatef( 0.0, 0.0, radius );
	gluQuadricNormals(quadratic, GLU_SMOOTH);			// Create Smooth Normals (NEW)
	gluQuadricTexture(quadratic, GL_TRUE);				// Create Texture Coords (NEW)
	glColor4fv( colour.getdata() );

#if 1
	gluSphere(quadratic,radius,32,32);				// Draw A Sphere With A Radius Of 1 And 16 Longitude And 16 Latitude Segments
#else 
	DrawSphere( radius, 16, 16 );
#endif
}

void Ball::Draw( const Renderer* render ) {
	glMatrixMode( GL_MODELVIEW );
	glTranslatef( pos.getX(), pos.getY(), pos.getZ() );
	glTranslatef( 0.0, 0.0, radius );
	gluQuadricNormals(quadratic, GLU_SMOOTH);			// Create Smooth Normals (NEW)
	gluQuadricTexture(quadratic, GL_TRUE);				// Create Texture Coords (NEW)

	if ( render->IsLightEnabled() ) {
		float shininess[] = { mat.GetShininess() };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat.GetAmbient().getdata() );			// Set Material Ambience
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.GetDiffuse().getdata() );			// Set Material Diffuse
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat.GetSpecular().getdata() );		// Set Material Specular
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess );		// Set Material Shininess
	} else {
		glColor4fv( colour.getdata() );
	}

#if 0
	gluSphere(quadratic,radius,32,32);				// Draw A Sphere With A Radius Of 1 And 16 Longitude And 16 Latitude Segments
#else 
	DrawSphere( radius, 16, 16 );
#endif
}

// calculate the position next second
void Ball::Update( float deltaTime ) {
	vector3 temp( speed );
	pos += temp.scale( deltaTime );
}

// from 0 ~ 100
static int GenerateRandomNumber() {
	static int randomNum = -1;
	if ( randomNum == -1 ) {
		srand( time( NULL ) );
	}

	randomNum = rand() % 100;
	if ( randomNum < 0 ) randomNum = 0;
	return randomNum; 
}

void Ball::AssignRandomSpeed() {
	float _x, _y, _z;
	_x = GenerateRandomNumber() / 5;
	_y = GenerateRandomNumber() / 5;
	_z = GenerateRandomNumber();

	speed.setXYZ( _x , _y, 0.0 );
}

void Ball::AssignRandomColor() {
	float r, g, b;
	r = GenerateRandomNumber() / 100.0;
	g = GenerateRandomNumber() / 100.0;
	b = GenerateRandomNumber() / 100.0;

	colour.setRGB( r, g, b );

	mat.SetAmbient( color( r, g, b, 1.0 ) );
	mat.SetDiffuse( color( r, g, b, 1.0 ) );
	//mat.SetSpecular( color( r, g, b, 1.0 ) );

}

Ball::~Ball()
{
}
