#include "ball.h"
#include "vector.h"

Ball::Ball():
	radius( 0.0 ), 
	weight( 10.0 ),
	color( WHITE ),
	pos( 0.0, 0.0, 0.0 ),
	rotate( 1.0, 0.0, 0.0 ),
	speed( 1.0, 0.0, 0.0 )
{
}

Ball::Ball( BALLCOLOR _color ):
	radius( 0.0 ), 
	weight( 10.0 ),
	color( _color ),
	pos( 0.0, 0.0, 0.0 ),
	rotate( 1.0, 0.0, 0.0 ),
	speed( 1.0, 0.0, 0.0 )
{
}

Ball::Ball( float _radius, float _weight, BALLCOLOR _color, vector3 _pos ):
	radius( _radius ),
	weight( _weight ),
	color( _color ),
	pos( _pos ),
	rotate( 1.0, 0.0, 0.0 ),
	speed( 1.0, 0.0, 0.0 )
{
}

Ball::~Ball()
{
}
