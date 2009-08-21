#ifndef _BALL_TABLE_
#define _BALL_TABLE_
#include "object.h"

// TODO: add vector to pch files
#include "vector.h"
#include "color.h"
#include "material.h"

class Renderer;

class Table: public VisibleObject
{
public:
	Table();
	Table( float _weight, float _height );
	Table( float _weight, float _height, const color &col, const vector3 &_pos, const vector3 &_normal );
	void Draw();
	void Draw( Renderer *rd );
	~Table();
//private:
	float Width, Height;
	float Bottom, Top, Left, Right;
	float foslide; 		// friction of sliding 
	float foroll; 		// friction of rolling

	color colour;
	Material mat;
	vector3 pos; 		// the center position of the table
	vector3 normal; 	// the normal line of table plane
};

#endif
