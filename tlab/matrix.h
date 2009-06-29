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
 * we have two frame A and B, the base vector of two frame are represent as A and B, where A = ( v1, v2, v3 ), vi is a vector.
 * we have a point P0, in frame A, P0 equeals a(a is a vector). and in frame B, P0 equals b
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

typedef struct __matrix{
	union{
		float m[16];
		struct {
			float r00, r01, r02, m03;
			float r10, r11, r12, m13;
			float r20, r21, r22, m23;
			float tx,  ty,  tz,  m33;
		};
	};
}matrix;

matrix * matrix_identity( matrix* a );
matrix * matrix_zero( matrix* a);
matrix * matrix_copy( matrix* a, matrix *b );  // a = b;

matrix * matrix_multiply( matrix *a, matrix *b, matrix *v ); // v = a * b;
matrix * matrix_left_mult( matrix *a, matrix *b );    // a = a*b;
matrix * matrix_right_mult( matrix *a, matrix *b);    // a = b*a;

matrix * matrix_scale( matrix *a, float x, float y, float z );
matrix * matrix_rotate_x( matrix *a, float angle );
matrix * matrix_rotate_y( matrix *a, float angle );
matrix * matrix_rotate_z( matrix *a, float angle );
matrix * matrix_rotate_xyz( matrix *a, float x, float y, float z, float angle );  // !!!TODO
matrix * matrix_translate( matrix *a, float x, float y, float z );
