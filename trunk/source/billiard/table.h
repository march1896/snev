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

#endif
