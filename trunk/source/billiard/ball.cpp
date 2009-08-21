#include "ball.h"
#include "vector.h"
#include "opengl.h"
#include "render.h"
#include <cstdlib>
#include <ctime>

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

void Ball::Draw() {
	glMatrixMode( GL_MODELVIEW );
	glTranslatef( pos.getX(), pos.getY(), pos.getZ() );
	glTranslatef( 0.0, 0.0, radius );
	gluQuadricNormals(quadratic, GLU_SMOOTH);			// Create Smooth Normals (NEW)
	gluQuadricTexture(quadratic, GL_TRUE);				// Create Texture Coords (NEW)
	glColor4fv( colour.getdata() );

	gluSphere(quadratic,1.3f,32,32);				// Draw A Sphere With A Radius Of 1 And 16 Longitude And 16 Latitude Segments
}

void Ball::Draw( const Renderer* render ) {
	glMatrixMode( GL_MODELVIEW );
	glTranslatef( pos.getX(), pos.getY(), pos.getZ() );
	glTranslatef( 0.0, 0.0, radius );
	gluQuadricNormals(quadratic, GLU_SMOOTH);			// Create Smooth Normals (NEW)
	gluQuadricTexture(quadratic, GL_TRUE);				// Create Texture Coords (NEW)

	float shininess[] = { mat.GetShininess() };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat.GetAmbient().getdata() );			// Set Material Ambience
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.GetDiffuse().getdata() );			// Set Material Diffuse
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat.GetSpecular().getdata() );		// Set Material Specular
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess );		// Set Material Shininess

	gluSphere(quadratic,1.3f,32,32);				// Draw A Sphere With A Radius Of 1 And 16 Longitude And 16 Latitude Segments
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
	_x = GenerateRandomNumber() / 3;
	_y = GenerateRandomNumber() / 3;
	_z = GenerateRandomNumber();

	speed.setXYZ( _x , _y, 0.0 );
}

void Ball::AssignRandomColor() {
	float r, g, b;
	r = GenerateRandomNumber() / 100.0;
	g = GenerateRandomNumber() / 100.0;
	b = GenerateRandomNumber() / 100.0;

	colour.setRGB( r, g, b );

	mat.SetDiffuse( color( r, g, b, 1.0 ) );
}

Ball::~Ball()
{
}
