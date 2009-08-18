#ifndef _COLOR_H_
#define _COLOR_H_
class color{
public:
	color();
	color( float _r, float _g, float _b, float _a );
	color( float _r, float _g, float _b );
	color( const color &rhs );
	~color();

	void setRGB( float _r, float _g, float _b );
	void setRGBA( float _r, float _g, float _b, float _a );
	float length();
	float getR() const;
	float getG() const;
	float getB() const;
	float getA() const;
	void setR( float _r );
	void setG( float _g );
	void setB( float _b );
	void setA( float _a );

	float* getdata() const;

	color operator+( const color &rhs );
	color operator-( const color &rhs );
	color operator+=( const color &rhs );
	color operator-=( const color &rhs );
	color operator=( const color &rhs );
	bool operator==( const color &rhs );
private:
	float r, g, b, a;
	static float dummydata[ 4 ];
	void check();
};
#endif
