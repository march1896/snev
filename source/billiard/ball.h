#ifndef _BILLIARD_BALL_
#define _BILLIARD_BALL_
#include "object.h"

//class vector3;
//we have to know the size of the vector.....
//how to deal with these things
#include "vector.h"
#include "color.h"
#include "material.h"

class Renderer;

class Ball : public VisibleObject
{
public:
	Ball();
	Ball( color _color );
	Ball( float _radius, float _weight, color _color, vector3 _pos );

	virtual ~Ball();

	void Update( float deltaTime );
	void Draw();
	void Draw( const Renderer* render );
	void AssignRandomSpeed();
	void AssignRandomColor();
//private:
	float radius;
	float weight;
	color colour;
	Material mat;

	vector3 pos;
	vector3 rotate, speed;
};
#endif

