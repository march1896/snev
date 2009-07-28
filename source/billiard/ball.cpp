#include "ball.h"
#include "vector.h"

Ball::Ball():
	x( 0.0 ), y( 0.0 ), radius( 0.0 ), 
	movespd( 0.0 ), rotatespd( 0.0 ),
	//rotateaxis( vector3(1.0, 0.0, 0.0) ),
	//movedir( vector3(1.0, 0.0, 0.0) ),
	color( WHITE )
{
	rotateaxis.setXYZ( 1.0, 0.0, 0.0 );
	movedir.setXYZ( 1.0, 0.0, 0.0 );
}

Ball::Ball( BALLCOLOR _color ):
	x( 0.0 ), y( 0.0 ), radius( 0.0 ), 
	movespd( 0.0 ), rotatespd( 0.0 ),
	color( _color )
{
	rotateaxis.setXYZ( 1.0, 0.0, 0.0 );
	movedir.setXYZ( 1.0, 0.0, 0.0 );
}

Ball::Ball( float _x, float _y, float _radius, BALLCOLOR _color ):
	x( _x ), y( _y ), radius( _radius ),
	movespd( 0.0 ), rotatespd( 0.0 ),
	color( _color )
{
	rotateaxis.setXYZ( 1.0, 0.0, 0.0 );
	movedir.setXYZ( 1.0, 0.0, 0.0 );
}

void Ball::SetAllAttributes( float _x, float _y, float _radius, float _movespd, float _rotatespd, vector3 _raxis, vector3 _dir, BALLCOLOR _color )
{
	x = _x, y = _y, radius = _radius;
	rotateaxis = _raxis;
	movedir = _dir;
	movespd = _movespd, rotatespd = _rotatespd;
	color = _color;
}


Ball::~Ball()
{
}