#ifndef _BALL_TABLE_
#define _BALL_TABLE_

// TODO: add vector to pch files
#include "vector.h"

class Table
{
public:
	Table();
	Table( float _weight, float _height );
	Table( float _weight, float _height, const vector3 &_pos, const vector3 &_normal );
	~Table();
//private:
	float Width, Height;
	float Bottom, Top, Left, Right;
	float foslide; 		// friction of sliding 
	float foroll; 		// friction of rolling
	vector3 pos; 		// the center position of the table
	vector3 normal; 	// the normal line of table plane
};

#endif
