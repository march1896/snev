#include "shadow.h"
#include "ball.h"
#include "opengl.h"
#include "vector.h"
#include "light.h"
#include "matrix.h"
#include "log.h"
#include "shader.h"
#include <cmath>
#include <set>

using std::set;

class line {
public:
	bool operator==( const line& para ) const;
	bool operator<( const line& para ) const;
	bool operator>( const line& para ) const;
	vector3 p[2];
	//vector3 normal[2]
};

struct PlaneEq {
	float a, b, c, d;
	// ax + by + cz + d = 0;
};

struct Plane {
	vector3 p[3];
	vector3 normal;
	PlaneEq pe;
};

static Plane pl[1024];
static int PN = 0;

static bool operator<( const vector3& lhs, const vector3& rhs ) {
	if ( lhs.getX() < rhs.getX() ) return true;
	else if ( lhs.getX() > rhs.getX() ) return false;

	if ( lhs.getY() < rhs.getY() ) return true;
	else if ( lhs.getY() > rhs.getY() ) return false;
	
	if ( lhs.getZ() < rhs.getZ() ) return true;
	else if ( lhs.getZ() > rhs.getZ() ) return false;

	return true;
}

static bool operator>( const vector3& lhs, const vector3& rhs ) {
	if ( lhs < rhs ) return false;
	else if ( lhs == rhs ) return false;
	return true;
}

bool line::operator<( const line& para ) const {
	if ( p[0] < para.p[0] ) return true;
	else if ( p[0] > para.p[0] ) return false;
	else if ( p[1] < para.p[1] ) return true;
	else if ( p[1] > para.p[1] ) return false;

	return false;
	/*
	if ( normal[0] < para.normal[0] ) return true;
	else if ( normal[0] > para.normal[0] ) return false;

	if ( normal[1] < para.normal[1] ) return true;
	else if ( normal[1] > para.normal[1] ) return false;
	*/
}

bool line::operator>( const line& para ) const {
	if ( p[0] < para.p[0] ) return false;
	else if ( p[0] > para.p[0] ) return true;
	else if ( p[1] < para.p[1] ) return false;
	else if ( p[1] > para.p[1] ) return true;

	return true;
}

bool line::operator==( const line& para ) const {
	//if ( p[0] == para.p[0] && normal[0] == para.normal[0] && p[1] == para.p[1] && normal[1] == para.normal[1] ||
	//		( p[1] == para.p[0] && normal[1] == para.normal[0] && normal[0] == para.normal[1] && p[0] == para.p[1] ) ) return true;
	if ( p[0] == para.p[0] && p[1] == para.p[1] || ( p[1] == para.p[0] && p[0] == para.p[1] ) ) return true;
	else return false;
}

static void sortline( line& para ) {
	if ( para.p[1] < para.p[0] ) {
		vector3 temp = para.p[0];
		para.p[0] = para.p[1];
		para.p[1] = temp;
	}
}

void ConstructSilhouette( set< line >& lineset, const vector3& lp ) {
	if ( PN <= 0 ) return; // this module is not initialized yet

	float side;
	line ln;
	set< line >::iterator itr;

	for ( int i = 0; i < PN; i ++ ) {
		//side = pl[ i ].pe.a * lp.getX() + pl[ i ].pe.b * lp.getY() + pl[ i ].pe.c * lp.getZ() + pl[ i ].pe.d;
		side = pl[i].normal.getX() * lp.getX() + pl[i].normal.getY() * lp.getY() + pl[i].normal.getZ() * lp.getZ();
		if ( side <= 0 ) continue;
		for ( int j = 0; j < 3; j ++ ) {
			ln.p[0] = pl[i].p[j];
			ln.p[1] = pl[i].p[(j+1)%3];

			//sortline( ln );
			//itr = lineset.find( ln );
			// it fails to find a certain line use above method, 
			// TODO: have a review of the STL containers
			// Here I find the reason.
			// The main reason is that two lines can not be compared
			// if I use the compare function above, if the first node of line A less than the first node of line B, 
			// then A < B, that is, there is nothing about the second node. So, if the first node of two lines are equal, we judge the line equal.

			for ( itr = lineset.begin(); itr != lineset.end(); itr ++ )
			{
				/*
				if ( *itr == ln ) {
					printf( "point a %f %f %f\t\t%f %f %f\n", itr->p[0].getX(), itr->p[0].getY(), itr->p[0].getZ(),
							itr->p[1].getX(), itr->p[1].getY(), itr->p[1].getZ() );

					printf( "point b %f %f %f\t\t%f %f %f\n", ln.p[0].getX(), ln.p[0].getY(), ln.p[0].getZ(),
							ln.p[1].getX(), ln.p[1].getY(), ln.p[1].getZ() );
				}
				*/
				if ( *itr == ln ) break;
			}

			if ( itr == lineset.end() ) {
				// if this line appears the first time, add it into the set
				lineset.insert( ln );
			} else {
				// if we find the line, this line appears twice, remove it;
				lineset.erase( itr );
			}
		}
	}
	// now, lines in lineset is considered to be the silhouette
}

void EndDrawShadow() {
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glDisable( GL_CULL_FACE );
	glDisable( GL_STENCIL_TEST );
	glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );
	glEnable( GL_LIGHTING );
}

void PreDrawShadow() {
	glEnable( GL_STENCIL_TEST );
	glDisable( GL_LIGHTING );

	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glDepthMask( GL_FALSE );

	glDisable( GL_DEPTH_TEST );
	glStencilFunc( GL_NEVER, 5, 0xffffffff );
	glStencilOp( GL_REPLACE, GL_KEEP, GL_KEEP );

	glPushMatrix();
	glLoadIdentity();
	glBegin( GL_TRIANGLE_STRIP );
		glVertex3f(-1.1f, 1.1f,-1.0f);
		glVertex3f(-1.1f,-1.1f,-1.0f);
		glVertex3f( 1.1f, 1.1f,-1.0f);
		glVertex3f( 1.1f,-1.1f,-1.0f);
	glEnd();
	glPopMatrix();

	glEnable( GL_DEPTH_TEST );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
}
void DrawShadow() {
	glDisable( GL_LIGHTING );
	glEnable( GL_STENCIL_TEST );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask( GL_FALSE );
	glColor4f( 0.0f, 0.0f, 0.0f, 0.4f );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glStencilFunc( GL_NOTEQUAL, 5, 0xffffffff );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	glPushMatrix();
	glLoadIdentity();
	glBegin( GL_TRIANGLE_STRIP );
		glVertex3f(-1.1f, 1.1f,-1.0f);
		glVertex3f(-1.1f,-1.1f,-1.0f);
		glVertex3f( 1.1f, 1.1f,-1.0f);
		glVertex3f( 1.1f,-1.1f,-1.0f);
	glEnd();
	glPopMatrix();
	glDisable( GL_BLEND );

	glDisable( GL_CULL_FACE );
	glDisable( GL_STENCIL_TEST );

	glDepthMask( GL_TRUE );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
}

void DrawShadowVolume( Ball* ball, Light* light ) {
	// here we should first calculate the silhouette
	// then draw the shadow volume
	vector3 lp;
	lp.setX( light->getPos().getX() - ball->pos.getX() );
	lp.setY( light->getPos().getY() - ball->pos.getY() );
	lp.setZ( light->getPos().getZ() - ball->pos.getZ() );
	set<line> lnset;
	set<line>::iterator itr;
	ConstructSilhouette( lnset, lp );

	int number = 0;
	for ( itr = lnset.begin(); itr != lnset.end(); itr ++ ) {
		number++;
	}
	//printf( "%d\n", number );

	// Draw the silhouette
	/*
	glDisable( GL_LIGHTING );
	glMatrixMode( GL_MODELVIEW );
	glTranslatef( ball->pos.getX(), ball->pos.getY(), ball->pos.getZ() );
	glTranslatef( 0.0, 0.0, ball->radius );
	glColor3f( 1.0, 0.0, 0.0 );
	*/

		/*
		glLineWidth( 10.0f );
		glBegin( GL_LINES );
			glVertex3f( itr->p[0].getX(), itr->p[0].getY(), itr->p[0].getZ() );
			glVertex3f( itr->p[1].getX(), itr->p[1].getY(), itr->p[1].getZ() );
		glEnd();
		*/
	// End


	glMatrixMode( GL_MODELVIEW );
	glTranslatef( ball->pos.getX(), ball->pos.getY(), ball->pos.getZ() );
	glTranslatef( 0.0, 0.0, ball->radius );

	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glDepthMask( GL_FALSE );
	glEnable( GL_STENCIL_TEST );
	glDisable( GL_LIGHTING );

	glEnable( GL_DEPTH_TEST );
	glStencilFunc( GL_ALWAYS, 1, 0xffffffff );
	glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );
	for ( itr = lnset.begin(); itr != lnset.end(); itr ++ ) {
		vector3 v[2];
		v[0].setX( ( itr->p[0].getX() - lp.getX() ) * 100.0f );
		v[0].setY( ( itr->p[0].getY() - lp.getY() ) * 100.0f );
		v[0].setZ( ( itr->p[0].getZ() - lp.getZ() ) * 100.0f );

		v[1].setX( ( itr->p[1].getX() - lp.getX() ) * 100.0f );
		v[1].setY( ( itr->p[1].getY() - lp.getY() ) * 100.0f );
		v[1].setZ( ( itr->p[1].getZ() - lp.getZ() ) * 100.0f );

		glBegin( GL_TRIANGLE_STRIP );
			glVertex3f( itr->p[0].getX(), itr->p[0].getY(), itr->p[0].getZ() );
			glVertex3f( itr->p[0].getX() + v[0].getX() , itr->p[0].getY() + v[0].getY() , itr->p[0].getZ() + v[0].getZ() );
			glVertex3f( itr->p[1].getX(), itr->p[1].getY(), itr->p[1].getZ() );
			glVertex3f( itr->p[1].getX() + v[1].getX() , itr->p[1].getY() + v[1].getY() , itr->p[1].getZ() + v[1].getZ() );
		glEnd();
	}
	glStencilOp( GL_KEEP, GL_KEEP, GL_DECR );
	glFrontFace( GL_CW );
	for ( itr = lnset.begin(); itr != lnset.end(); itr ++ ) {
		vector3 v[2];
		v[0].setX( ( itr->p[0].getX() - lp.getX() ) * 100.0f );
		v[0].setY( ( itr->p[0].getY() - lp.getY() ) * 100.0f );
		v[0].setZ( ( itr->p[0].getZ() - lp.getZ() ) * 100.0f );

		v[1].setX( ( itr->p[1].getX() - lp.getX() ) * 100.0f );
		v[1].setY( ( itr->p[1].getY() - lp.getY() ) * 100.0f );
		v[1].setZ( ( itr->p[1].getZ() - lp.getZ() ) * 100.0f );

		glBegin( GL_TRIANGLE_STRIP );
			glVertex3f( itr->p[0].getX(), itr->p[0].getY(), itr->p[0].getZ() );
			glVertex3f( itr->p[0].getX() + v[0].getX() , itr->p[0].getY() + v[0].getY() , itr->p[0].getZ() + v[0].getZ() );
			glVertex3f( itr->p[1].getX(), itr->p[1].getY(), itr->p[1].getZ() );
			glVertex3f( itr->p[1].getX() + v[1].getX() , itr->p[1].getY() + v[1].getY() , itr->p[1].getZ() + v[1].getZ() );
		glEnd();
	}
	glFrontFace( GL_CCW );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask( GL_TRUE );
	glEnable( GL_LIGHTING );
	glDisable( GL_STENCIL_TEST );
}

static void CalcPlane( Plane *plane) {
	PlaneEq* pe = &plane->pe;
	vector3 v[4];
	int i;

	for (i=0;i<3;i++){
		v[i+1].setX( plane->p[i].getX() );
		v[i+1].setY( plane->p[i].getY() );
		v[i+1].setZ( plane->p[i].getZ() );
	}
	pe->a = v[1].getY()*(v[2].getZ()-v[3].getZ()) + v[2].getY()*(v[3].getZ()-v[1].getZ()) + v[3].getY()*(v[1].getZ()-v[2].getZ());
	pe->b = v[1].getZ()*(v[2].getX()-v[3].getX()) + v[2].getZ()*(v[3].getX()-v[1].getX()) + v[3].getZ()*(v[1].getX()-v[2].getX());
	pe->c = v[1].getX()*(v[2].getY()-v[3].getY()) + v[2].getX()*(v[3].getY()-v[1].getY()) + v[3].getX()*(v[1].getY()-v[2].getY());
	pe->d =-( v[1].getX()*(v[2].getY()*v[3].getZ() - v[3].getY()*v[2].getZ()) +
					  v[2].getX()*(v[3].getY()*v[1].getZ() - v[1].getY()*v[3].getZ()) +
					  v[3].getX()*(v[1].getY()*v[2].getZ() - v[2].getY()*v[1].getZ()) );
}

static void AssignPlane( Plane* pl, vector3 pt1, vector3 pt2, vector3 pt3, vector3 normal ) {
	pl->p[ 0 ] = pt1;
	pl->p[ 1 ] = pt2;
	pl->p[ 2 ] = pt3;
	//pl->normal = normal;
	pl->normal = pt1 + pt2 + pt3;
	CalcPlane( pl );
}

static float r2a( float radian ) // radian to angle 
{
	return radian / M_PI * 180.0f;
}

static float a2r( float angle ) // angle to radian
{ 
	return angle / 180.f * M_PI;
}

static float angle_sin( float angle ) {
	return sin( a2r( angle ) );
}

static float angle_cos( float angle ) {
	return cos( a2r( angle ) );
}

void DrawShadowModel( Ball* ball ) {
	glMatrixMode( GL_MODELVIEW );
	glTranslatef( ball->pos.getX(), ball->pos.getY(), ball->pos.getZ() );
	glTranslatef( 0.0, 0.0, ball->radius );
	
	for ( int i = 0; i < PN; i ++ ) {
		glBegin( GL_TRIANGLES );
			glVertex3f( pl[ i ].p[0].getX(), pl[ i ].p[0].getY(), pl[ i ].p[0].getZ() );
			glVertex3f( pl[ i ].p[1].getX(), pl[ i ].p[1].getY(), pl[ i ].p[1].getZ() );
			glVertex3f( pl[ i ].p[2].getX(), pl[ i ].p[2].getY(), pl[ i ].p[2].getZ() );
		glEnd();
	}
}

void ConstructShadowModel( Ball* ball ) {
	int longitude = 12;
	int latitude = 12;
	vector3 pt[3], normal;
	float alpha, theta, theta2;
	float radius = ball->radius;

	PN = 0;
	pt[ 0 ].setX( 0.0f );
	pt[ 0 ].setY( 0.0f );
	pt[ 0 ].setZ( radius );
	theta = 180.0f / (longitude + 1 ) / 2;
	for ( int i = 0; i < longitude; i ++ ) {
		alpha = 360.0 / longitude * i;
		pt[ 1 ].setX( radius * angle_sin( theta ) * angle_cos( alpha ) );
		pt[ 1 ].setY( radius * angle_sin( theta ) * angle_sin( alpha ) );
		pt[ 1 ].setZ( radius * angle_cos( theta ) );
		alpha = 360.0 / longitude * ( i + 1 );
		pt[ 2 ].setX( radius * angle_sin( theta ) * angle_cos( alpha ) );
		pt[ 2 ].setY( radius * angle_sin( theta ) * angle_sin( alpha ) );
		pt[ 2 ].setZ( radius * angle_cos( theta ) );
		AssignPlane( &pl[ PN++ ], pt[0], pt[1], pt[2], pt[0] );
	}

	pt[ 0 ].setX( 0.0f );
	pt[ 0 ].setY( 0.0f );
	pt[ 0 ].setZ( -radius );
	theta = 180.0f - 180.0f / ( longitude + 1 ) / 2;
	for ( int i = longitude; i > 0; i -- ) {
		alpha = 360.0 / longitude * i;
		pt[ 1 ].setX( radius * angle_sin( theta ) * angle_cos( alpha ) );
		pt[ 1 ].setY( radius * angle_sin( theta ) * angle_sin( alpha ) );
		pt[ 1 ].setZ( radius * angle_cos( theta ) );
		alpha = 360.0 / longitude * ( i - 1 );
		pt[ 2 ].setX( radius * angle_sin( theta ) * angle_cos( alpha ) );
		pt[ 2 ].setY( radius * angle_sin( theta ) * angle_sin( alpha ) );
		pt[ 2 ].setZ( radius * angle_cos( theta ) );
		AssignPlane( &pl[ PN++ ], pt[0], pt[1], pt[2], pt[0] );
	}

	for ( int i = 0; i < latitude; i ++ ) {
		float tempx = 180.0f / ( longitude + 1 ) / 2;
		float tempy = 180.0f - tempx;
		theta = tempx + tempy / latitude * i ;
		theta2 = tempx + tempy / latitude * ( i + 1 ) ;
		for ( int j = 1; j <= longitude; j ++ ) {
			alpha = 360.0 / longitude * j;
			pt[ 0 ].setX( radius * angle_sin( theta ) * angle_cos( alpha ) );
			pt[ 0 ].setY( radius * angle_sin( theta ) * angle_sin( alpha ) );
			pt[ 0 ].setZ( radius * angle_cos( theta ) );
			alpha = 360.0 / longitude * j;
			pt[ 1 ].setX( radius * angle_sin( theta2 ) * angle_cos( alpha ) );
			pt[ 1 ].setY( radius * angle_sin( theta2 ) * angle_sin( alpha ) );
			pt[ 1 ].setZ( radius * angle_cos( theta2 ) );
			alpha = 360.0 / longitude * ( j + 1 );
			pt[ 2 ].setX( radius * angle_sin( theta ) * angle_cos( alpha ) );
			pt[ 2 ].setY( radius * angle_sin( theta ) * angle_sin( alpha ) );
			pt[ 2 ].setZ( radius * angle_cos( theta ) );
			AssignPlane( &pl[ PN++ ], pt[0], pt[1], pt[2], pt[0] );
			
			alpha = 360.0 / longitude * ( j + 1 );
			pt[ 0 ].setX( radius * angle_sin( theta ) * angle_cos( alpha ) );
			pt[ 0 ].setY( radius * angle_sin( theta ) * angle_sin( alpha ) );
			pt[ 0 ].setZ( radius * angle_cos( theta ) );
			alpha = 360.0 / longitude * j;
			pt[ 1 ].setX( radius * angle_sin( theta2 ) * angle_cos( alpha ) );
			pt[ 1 ].setY( radius * angle_sin( theta2 ) * angle_sin( alpha ) );
			pt[ 1 ].setZ( radius * angle_cos( theta2 ) );
			alpha = 360.0 / longitude * ( j + 1 );
			pt[ 2 ].setX( radius * angle_sin( theta2 ) * angle_cos( alpha ) );
			pt[ 2 ].setY( radius * angle_sin( theta2 ) * angle_sin( alpha ) );
			pt[ 2 ].setZ( radius * angle_cos( theta2 ) );
			AssignPlane( &pl[ PN++ ], pt[0], pt[1], pt[2], pt[0] );
		}
	}
}


/*
static void CalcPlane(glObject o, sPlane *plane){
	sPoint v[4];
	int i;

	for (i=0;i<3;i++){
		v[i+1].x = o.points[plane->p[i]].x;
		v[i+1].y = o.points[plane->p[i]].y;
		v[i+1].z = o.points[plane->p[i]].z;
	}
	plane->PlaneEq.a = v[1].y*(v[2].z-v[3].z) + v[2].y*(v[3].z-v[1].z) + v[3].y*(v[1].z-v[2].z);
	plane->PlaneEq.b = v[1].z*(v[2].x-v[3].x) + v[2].z*(v[3].x-v[1].x) + v[3].z*(v[1].x-v[2].x);
	plane->PlaneEq.c = v[1].x*(v[2].y-v[3].y) + v[2].x*(v[3].y-v[1].y) + v[3].x*(v[1].y-v[2].y);
	plane->PlaneEq.d =-( v[1].x*(v[2].y*v[3].z - v[3].y*v[2].z) +
					  v[2].x*(v[3].y*v[1].z - v[1].y*v[3].z) +
					  v[3].x*(v[1].y*v[2].z - v[2].y*v[1].z) );
}
*/

/***********************************************************************************
 *
 *
 *
 *
 *
 * ABOVE IS SHADOW VOLUME IMPLEMENTATION, AND RIGHT NOW IT IS NOT LOOKED WELL
 * BELOW IS SHADOW MAPPING, USE GLSL
 *
 *
 *
 *
 ***********************************************************************************/

/*
const int shadowMapSize = 512;
static matrix44 lightProjectionMatrix, lightViewMatrix, textureMatrix;
static float tmp[16];
static GLuint shadowMapTexture;
static GLuint depth_fb;  // depth frambuffer
static GLuint depth_tex; // depth texture
static Shader* shader;
static bool Initialized = false;

// pre draw the scene from light's pos
void PreShadowMap( Light * light ) {

	if ( !Initialized ) {
		// init
		glGenFramebuffersEXT(1, &depth_fb);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, depth_fb);
		glDrawBuffer(GL_NONE);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		glGenTextures(1, &depth_tex);

		glBindTexture(GL_TEXTURE_2D, depth_tex);
		//glTexImage3D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, depth_size, depth_size, MAX_SPLITS, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		Initialized = true;
	}

	// generate a texture to save the shadow map
	glEnable( GL_TEXTURE_2D );
	glGenTextures(1, &shadowMapTexture);
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0,
					GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// generate end
	
	// Get matrix from light pos
	glDisable( GL_LIGHTING );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(45.0f, 1.0f, 2.0f, 8.0f);
	// save the matrix look from light's pos 
	glGetFloatv(GL_PROJECTION_MATRIX, tmp);
	lightProjectionMatrix.setdata( tmp );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	gluLookAt( light->getPos().getX(), light->getPos().getY(), light->getPos().getZ() ,
				0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f);

	glGetFloatv(GL_MODELVIEW_MATRIX, tmp);
	lightViewMatrix.setdata( tmp );
	// Get matrix from light pos, end

	matrix44 biasMatrix( 	0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.5f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f 	);	//bias from [-1, 1] to [0, 1]
	matrix44 textureMatrix=biasMatrix*lightProjectionMatrix*lightViewMatrix;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// redirect rendering to the depth texture
	glDisable( GL_TEXTURE_2D );
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, depth_fb);
	// store the screen viewport
	glPushAttrib(GL_VIEWPORT_BIT);

	//Use viewport the same size as the shadow map
	glViewport(0, 0, shadowMapSize, shadowMapSize);

	//Draw back faces into the shadow map
	glCullFace(GL_FRONT);

	//Disable color writes, and use flat shading for speed
	glShadeModel(GL_FLAT);
	//glColorMask(0, 0, 0, 0);
	//glColorMask(1, 1, 1, 1);
	
	// make the current depth map a rendering target
	glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, depth_tex, 0);

}
	
//Draw the scene, first pass
//DrawScene();

void DuringShadowMap()
{
	//Read the depth buffer into the shadow map texture
	//glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, shadowMapSize, shadowMapSize);

	// clear the depth texture from last time
	glClear(GL_DEPTH_BUFFER_BIT);
	
	//restore states
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);
	glShadeModel(GL_SMOOTH);
	glColorMask(1, 1, 1, 1);

	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	// test code
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef( 0.0, 0.0, -3.0 );
	glBegin( GL_QUADS );
		glTexCoord2f( 0.0, 0.0 ); glVertex3f( -1.0, -1.0, 0.0 );
		glTexCoord2f( 1.0, 0.0 ); glVertex3f( 1.0, -1.0, 0.0 );
		glTexCoord2f( 1.0, 1.0 ); glVertex3f( 1.0, 1.0, 0.0 );
		glTexCoord2f( 0.0, 1.0 ); glVertex3f( -1.0, 1.0, 0.0 );
	glEnd();
	// end

	glEnable( GL_LIGHTING );

	shader = new Shader( "Shader\\shadowmap.vert", "Shader\\shadowmap.frag" );
	//shader->Activate();
}

// Draw the scene, second pass
// DrasScene();

void PostShadowMap() {
	//shader->Deactivate();
}
*/
