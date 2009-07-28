#ifndef _BALL_TABLE_
#define _BALL_TABLE_
class Table
{
public:
	Table();
	Table( float _w, float h );
	void SetAllAttributes( float width, float height );
	~Table();
private:
	float Width, Height;
	float Left, Right, Top, Bottom;
};

Table::Table():
	Width( 10.0 ), Height( 6.0 ),
	Left( -5.0 ), Right( 5.0 ), Top( 3.0 ), Bottom( -3.0 )
{
}

Table::Table( float _w, float _h ):
	Width( _w ), Height( _h )
{
	Left = -Width / 2.0;
	Right = Width / 2.0;
	Top = Height / 2.0;
	Bottom = -Height / 2.0;
}

void Table::SetAllAttributes( float _w, float _h )
{
	Width = _w;
	Height = _h;

	Left = -Width / 2.0;
	Right = Width / 2.0;
	Top = Height / 2.0;
	Bottom = -Height / 2.0;
}

Table::~Table()
{
}

#endif
