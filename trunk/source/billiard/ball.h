#ifndef _BILLIARD_BALL_
#define _BILLIARD_BALL_
#include "object.h"

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

class Ball : public VisibleObject
{
public:
	Ball();
	Ball( BALLCOLOR _color );
	Ball( float _radius, float _weight, BALLCOLOR _color, vector3 _pos );
	virtual ~Ball();
//private:
	float radius;
	float weight;
	vector3 pos;
	vector3 rotate, speed;
	BALLCOLOR color;
};
#endif

