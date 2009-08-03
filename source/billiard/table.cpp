#include "table.h"

Table::Table():
	Width( 10.0 ), Height( 6.0 ),
	Left( -5.0 ), Right( 5.0 ), Top( 3.0 ), Bottom( -3.0 ),
	pos( 0.0, 0.0, 0.0 ),
	normal( 0.0, 1.0, 0.0 ),
	foslide( 0.1 ),
	foroll( 0.1 )
{
}

Table::Table( float _weight, float _height ):
	Width( _weight ), Height( _height ),
	pos( 0.0, 0.0, 0.0 ),
	normal( 0.0, 1.0, 0.0 ),
	foslide( 0.1 ),
	foroll( 0.1 )
{
	Left = -Width / 2.0;
	Right = Width / 2.0;
	Top = Height / 2.0;
	Bottom = -Height / 2.0;
}

Table::Table( float _weight, float _height, const vector3 &_pos, const vector3 &_normal ):
	Width( _weight ), Height( _height ),
	pos( _pos ),
	normal( _normal ),
	foslide( 0.1 ),
	foroll( 0.1 )
{
	Left = -Width / 2.0;
	Right = Width / 2.0;
	Top = Height / 2.0;
	Bottom = -Height / 2.0;
}

Table::~Table()
{
}



