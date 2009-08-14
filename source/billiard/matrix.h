/* vector a column-major 
 * a = [ x
 * 	 y
 * 	 z
 * 	 1 ]
 * a^T = [ x y z 1 ]
 * ^T means the transpotion of the matrix
 * matrix definition   
 * M = 	[ r00  r01  r02  m03
 * 	  r10  r11  r12  m13
 * 	  r20  r21  r22  m23
 * 	  tx   ty   tz   1   ]
 *
 * M^T =[ r00  r10  r20  tx 
 * 	  r01  r11  r21  ty
 * 	  r02  r12  r22  tx
 * 	  m03  m13  m23  1   ]
 * we have two coordinates A and B, the base vector of two coordinates are represent as A and B, where A = { v1, v2, v3 }, vi is a vector.
 * we have a point, P0, in frame A. P0 equeals a(a is a vector). and in frame B, P0 equals b
 * and we have B = M.A        . means dot , A and B means base vectors of two frames
 * 		 |
 * 		\|/
 * 	       a = M^T.b
 * 	       b = ((M^T)^(-1)).a
 * 	       b = ((M^(-1))^T).a
 * 	       
 * a = M1^T.M2^T.b
 * a^T = b^T.M2.M1
 */

#ifndef _MATRIX_H_
#define _MATRIX_H_

class matrix44{
private:
	union {
		float m[16];
		struct {
			float r00, r01, r02, m03;
			float r10, r11, r12, m13;
			float r20, r21, r22, m23;
			float tx,  ty,  tz,  m33;
		};
	}data;
public:
	matrix44();
	matrix44( float *mp );
	matrix44( const matrix44 &rhs );
	~matrix44();

	const float *getdata() const;
	void setdata( const float * data );
	void set( int x, int y, float data );
	float get( int x, int y );

	matrix44 operator=( const matrix44 &rhs );
	bool operator==( const matrix44 &rhs );
	matrix44 left_mult( const matrix44 &rhs );
	matrix44 right_mult( const matrix44 &rhs );

	void identity();
	void zero();
	void scale( float x,  float y,  float z );
	void rotate_x(  float angle );
	void rotate_y(  float angle );
	void rotate_z(  float angle );
	void rotate_xyz(  float x,  float y,  float z,  float angle );
	void translate(  float x,  float y,  float z );
	friend matrix44 operator*( const matrix44 &lhs, const matrix44 &rhs );
};

matrix44 operator*(  matrix44 &lhs,  matrix44 &rhs );

#endif
