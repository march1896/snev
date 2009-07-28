#ifndef _BILLIARD_BALL_
#define _BILLIARD_BALL_
enum BALLCOLOR
{
	WHITE,
	BLACK,
	RED,
};

//class vector3;
//we have to know the size of the vector.....
//sigh
#include "vector.h"

class Ball
{
public:
	Ball();
	Ball( BALLCOLOR _color );
	Ball( float _x, float _y, float _radius, BALLCOLOR _color );
	void SetAllAttributes( float _x, float _y, float _radius, float _movespd, float _rotatespd, BALLCOLOR color );
	~Ball();
private:
	float x, y, radius;
	float movespd, rotatespd;
	vector3 rotateaxis, movedir;
	BALLCOLOR color;
};
#endif

