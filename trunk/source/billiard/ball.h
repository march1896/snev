#ifndef _BILLIARD_BALL_
#define _BILLIARD_BALL_

//class vector3;
//we have to know the size of the vector.....
//how to deal with these things
#include "vector.h"

enum BALLCOLOR
{
	WHITE,
	BLACK,
	RED,
};


class Ball
{
public:
	Ball();
	Ball( BALLCOLOR _color );
	Ball( float _x, float _y, float _radius, float _weight, BALLCOLOR _color );
	~Ball();

	void SetAllAttributes( float _x, float _y, float _radius, float _weight, float _movespd, float _rotatespd, vector3 _raxis, vector3 _dir, BALLCOLOR color );
//private:
	float x, y, radius;
	float movespd, rotatespd;
	float weight;
	vector3 rotateaxis, movedir;
	BALLCOLOR color;
};
#endif

