#include "shadow.h"
#include "ball.h"
#include "opengl.h"
#include "vector.h"
#include "light.h"
#include "log.h"
#include <cmath>
#include <set>

using std::set;

class line {
public:
	bool operator==( const line& para ) const;
	bool operator<( const line& para ) const;
	vector3 p[2];
	//vector3 normal[2]
};

struct PlaneEq {
	float a, b, c, d;
	// ax + by + cz + d = 0;
};

struct Plane {
	vector3 p[3];
	PlaneEq pe;
};

static Plane pl[1024];
static int PN = 0;

bool operator<( const vector3& lhs, const vector3& rhs ) {
	if ( lhs.getX() < rhs.getX() ) return true;
	else if ( lhs.getX() > rhs.getX() ) return false;

	if ( lhs.getY() < rhs.getY() ) return true;
	else if ( lhs.getY() > rhs.getY() ) return false;
	
	if ( lhs.getZ() < rhs.getZ() ) return true;
	else if ( lhs.getZ() > rhs.getZ() ) return false;
}

bool operator>( const vector3& lhs, const vector3& rhs ) {
	if ( lhs < rhs ) return false;
	else if ( lhs == rhs ) return false;
	return true;
}

bool line::operator<( const line& para ) const {
	if ( p[0] < para.p[0] ) return true;
	else if ( p[0] > para.p[0] ) return false;

	if ( p[1] < para.p[1] ) return true;
	else if ( p[1] > para.p[1] ) return false;

	/*
	if ( normal[0] < para.normal[0] ) return true;
	else if ( normal[0] > para.normal[0] ) return false;

	if ( normal[1] < para.normal[1] ) return true;
	else if ( normal[1] > para.normal[1] ) return false;
	*/
}

bool line::operator==( const line& para ) const {
	/*if ( p[0] == para.p[0] && normal[0] == para.normal[0] && p[1] == para.p[1] && normal[1] == para.normal[1] ||
			( p[1] == para.p[0] && normal[1] == para.normal[0] && normal[0] == para.normal[1] && p[0] == para.p[1] ) ) return true;*/
	if ( p[0] == para.p[0] && p[1] == para.p[1] ||
			( p[1] == para.p[0] && p[0] == para.p[1] ) ) return true;
	else return false;
}

static void sortline( line& para ) {
	if ( para.p[0] > para.p[1] ) {
		vector3 temp = para.p[0];
		para.p[0] = para.p[1];
		para.p[1] = temp;
	}
}

void ConstructSilhouette( set< line >& lineset, set< line >::iterator& itr, const vector3& lp ) {
	if ( PN <= 0 ) return; // this module is not initialized yet

	float side;
	//std::set< line >lineset;
	//std::set< line >::iterator itr;
	line ln;
	for ( int i = 0; i < PN; i ++ ) {
		side = pl[ i ].pe.a * lp.getX() + 
			pl[ i ].pe.b * lp.getY() +
			pl[ i ].pe.c * lp.getZ() +
			pl[ i ].pe.d;
		if ( side <= 0 ) break;
		for ( int j = 0; j < 3; j ++ ) {
			ln.p[0] = pl[i].p[j];
			ln.p[1] = pl[i].p[(j+1)%3];

			sortline( ln );
			itr = lineset.find( ln );
			if ( itr != lineset.end() ) {
				// if we find the line, this line appears twice, remove it;
				lineset.erase( itr );
			} else {
				// if this line appears the first time, add it into the set
				lineset.insert( ln );
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
	set< line > lnset;
	set< line >::iterator itr;
	ConstructSilhouette( lnset, itr, lp );

	int number = 0;
	for ( itr = lnset.begin(); itr != lnset.end(); itr ++ ) {
		number++;
	}
	printf( "%d\n", number );

	// Draw the silhouette

	glDisable( GL_LIGHTING );
	glMatrixMode( GL_MODELVIEW );
	glTranslatef( ball->pos.getX(), ball->pos.getY(), ball->pos.getZ() );
	glTranslatef( 0.0, 0.0, ball->radius );
	glColor3f( 1.0, 0.0, 0.0 );

	for ( itr = lnset.begin(); itr != lnset.end(); itr ++ ) {
		printf( "%f %f %f\t\t%f %f %f\n", itr->p[0].getX(), itr->p[0].getY(), itr->p[0].getZ(), itr->p[1].getX(), itr->p[1].getY(), itr->p[1].getZ() );
		glLineWidth( 10.0f );
		glBegin( GL_LINES );
			glVertex3f( itr->p[0].getX(), itr->p[0].getY(), itr->p[0].getZ() );
			glVertex3f( itr->p[1].getX(), itr->p[1].getY(), itr->p[1].getZ() );
		glEnd();
	}
	return;
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
		v[0].setX( itr->p[0].getX() - light->getPos().getX() );
		v[0].setY( itr->p[0].getY() - light->getPos().getY() );
		v[0].setZ( itr->p[0].getZ() - light->getPos().getZ() );

		v[1].setX( itr->p[1].getX() - light->getPos().getX() );
		v[1].setY( itr->p[1].getY() - light->getPos().getY() );
		v[1].setZ( itr->p[1].getZ() - light->getPos().getZ() );

		glBegin( GL_TRIANGLE_STRIP );
			glVertex3f( itr->p[0].getX(), itr->p[0].getY(), itr->p[0].getZ() );
			glVertex3f( itr->p[0].getX() + v[0].getX() , itr->p[0].getY() + v[0].getY() , itr->p[0].getZ() + v[0].getZ() );
			glVertex3f( itr->p[1].getX(), itr->p[1].getY(), itr->p[1].getZ() );
			glVertex3f( itr->p[1].getX() + v[1].getX() , itr->p[1].getY() + v[1].getY() , itr->p[1].getZ() + v[1].getZ() );
		glEnd();
	}
	glFrontFace( GL_CW );
	glStencilOp( GL_KEEP, GL_KEEP, GL_DECR );
	for ( itr = lnset.begin(); itr != lnset.end(); itr ++ ) {
		vector3 v[2];
		v[0].setX( itr->p[0].getX() - light->getPos().getX() );
		v[0].setY( itr->p[0].getY() - light->getPos().getY() );
		v[0].setZ( itr->p[0].getZ() - light->getPos().getZ() );

		v[1].setX( itr->p[1].getX() - light->getPos().getX() );
		v[1].setY( itr->p[1].getY() - light->getPos().getY() );
		v[1].setZ( itr->p[1].getZ() - light->getPos().getZ() );

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

static void AssignPlane( Plane* pl, vector3 pt1, vector3 pt2, vector3 pt3 ) {
	pl->p[ 0 ] = pt1;
	pl->p[ 1 ] = pt2;
	pl->p[ 2 ] = pt3;
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

void DrawShadowModle( Ball* ball ) {
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
	int longitude = 10;
	int latitude = 10;
	vector3 pt[3];
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
		AssignPlane( &pl[ PN++ ], pt[0], pt[1], pt[2] );
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
		AssignPlane( &pl[ PN++ ], pt[0], pt[1], pt[2] );
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
			AssignPlane( &pl[ PN++ ], pt[0], pt[1], pt[2] );
			
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
			AssignPlane( &pl[ PN++ ], pt[0], pt[1], pt[2] );
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

