#ifndef _SHADOW_H_
#define _SHADOW_H_
/*****************************************
 * this file generate shadow for ball use shadow volume method
 ****************************************/
#include "ball.h"
#include "opengl.h"
#include "vector.h"
#include "light.h"
#include <set>

class line {
public:
	bool operator==( const line& para );
	vector3 point[2];
	vector3 normal[2]
}

bool line::operator<( const line& para ) {
	if ( point[0] < para.point[0] ) return true;
	else if ( point[0] > para.point[0] ) return false;

	if ( point[1] < para.point[1] ) return true;
	else if ( point[1] > para.point[1] ) return false;

	if ( normal[0] < para.normal[0] ) return true;
	else if ( normal[0] > para.normal[0] ) return false;

	if ( normal[1] < para.normal[1] ) return true;
	else if ( normal[1] > para.normal[1] ) return false;
}

bool line::operator==( const line& para ) {
	if ( point[0] == para.point[0] && normal[0] == para.normal[0] && point[1] == para.point[1] && normal[1] == para.normal[1] ||
			( point[1] == para.point[0] && normal[1] == para.normal[0] && normal[0] == para.normal[1] && point[0] == para.point[1] ) ) return true;
	else return false;
}

static void AssignPlane( Plane* pl, Point pt1, Point pt2, Point pt3 ) {
	pl->p[ 0 ] = pt1;
	pl->p[ 1 ] = pt2;
	pl->p[ 2 ] = pt3;
	CalcPlane( pl );
}

void ConstructShadowPlane() {
	int longitude = 10;
	int latitude = 10;
	Point pt[3];
	float alpha, theta;
	float radius = ball->radius;

	PN = 0;
	pt[ 0 ].x = 0.0f;
	pt[ 0 ].y = 0.0f;
	pt[ 0 ].z = radius;
	theta = 180.0f / (longitude + 1 ) / 2;
	for ( int i = 0; i < longitude; i ++ ) {
		alpha = 360.0 / longitude * i;
		pt[ 1 ].x = radius * angle_sin( theta ) * angle_cos( alpha );
		pt[ 1 ].y = radius * angle_sin( theta ) * angle_sin( alpha );
		pt[ 1 ].z = radius * angle_cos( theta );
		alpha = 360.0 / longitude * ( i + 1 );
		pt[ 2 ].x = radius * angle_sin( theta ) * angle_cos( alpha );
		pt[ 2 ].y = radius * angle_sin( theta ) * angle_sin( alpha );
		pt[ 2 ].z = radius * angle_cos( theta );
		AssignPlane( &pl[ PN++ ], pt[0], pt[1], pt[2] );
	}

	theta = 180.0f - 180.0f / ( longitude + 1 ) / 2;
	for ( int i = longitude; i > 0; i -- ) {
		alpha = 360.0 / longitude * i;
		pt[ 1 ].x = radius * angle_sin( theta ) * angle_cos( alpha );
		pt[ 1 ].y = radius * angle_sin( theta ) * angle_sin( alpha );
		pt[ 1 ].z = radius * angle_cos( theta );
		alpha = 360.0 / longitude * ( i - 1 );
		pt[ 2 ].x = radius * angle_sin( theta ) * angle_cos( alpha );
		pt[ 2 ].y = radius * angle_sin( theta ) * angle_sin( alpha );
		pt[ 2 ].z = radius * angle_cos( theta );
		AssignPlane( &pl[ PN++ ], pt[0], pt[1], pt[2] );
	}

	for ( int i = 0; i < latitude; i ++ ) {
		float tempx = 180.0f / ( longitude + 1 ) / 2;
		float tempy = 180.0f - tempx;
		theta = tempx + tempy / latitude * i ;
		theta2 = tempx + tempy / latitude * ( i + 1 ) ;
		for ( int j = 1; j < longitude; j ++ ) {
			alpha = 360.0 / longitude * i;
			pt[ 0 ].x = radius * angle_sin( theta ) * angle_cos( alpha );
			pt[ 0 ].y = radius * angle_sin( theta ) * angle_sin( alpha );
			pt[ 0 ].z = radius * angle_cos( theta );
			alpha = 360.0 / longitude * i;
			pt[ 1 ].x = radius * angle_sin( theta2 ) * angle_cos( alpha );
			pt[ 1 ].y = radius * angle_sin( theta2 ) * angle_sin( alpha );
			pt[ 1 ].z = radius * angle_cos( theta );
			alpha = 360.0 / longitude * ( i + 1 );
			pt[ 2 ].x = radius * angle_sin( theta ) * angle_cos( alpha );
			pt[ 2 ].y = radius * angle_sin( theta ) * angle_sin( alpha );
			pt[ 2 ].z = radius * angle_cos( theta );
			AssignPlane( &pl[ PN++ ], pt[0], pt[1], pt[2] );

			alpha = 360.0 / longitude * ( i + 1 );
			pt[ 0 ].x = radius * angle_sin( theta ) * angle_cos( alpha );
			pt[ 0 ].y = radius * angle_sin( theta ) * angle_sin( alpha );
			pt[ 0 ].z = radius * angle_cos( theta );
			alpha = 360.0 / longitude * i;
			pt[ 1 ].x = radius * angle_sin( theta2 ) * angle_cos( alpha );
			pt[ 1 ].y = radius * angle_sin( theta2 ) * angle_sin( alpha );
			pt[ 1 ].z = radius * angle_cos( theta );
			alpha = 360.0 / longitude * ( i + 1 );
			pt[ 2 ].x = radius * angle_sin( theta2 ) * angle_cos( alpha );
			pt[ 2 ].y = radius * angle_sin( theta2 ) * angle_sin( alpha );
			pt[ 2 ].z = radius * angle_cos( theta2 );
			AssignPlane( &pl[ PN++ ], pt[0], pt[1], pt[2] );
		}
	}
}

void DrawShadow( Ball* ball, Light* light ) {
	// here we should first calculate the silhouette
	// then draw the shadow volume
}

static struct Plane {
	Point p[3];
	PlaneEq;
};

Plane pl[1024];
int PN;

static struct PlaneEq {
	float a, b, c, d;
	// ax + by + cz + d = 0;
};

static struct Point {
	float x, y, z;
};

static void CalcPlane( Plane *plane) {
	PlaneEq* planeeq = &plane->PlaneEq;
	Point v[4];
	int i;

	for (i=0;i<3;i++){
		v[i+1].x = plane->p[i].x;
		v[i+1].y = plane->p[i].y;
		v[i+1].z = plane->p[i].z;
	}
	planeeq->a = v[1].y*(v[2].z-v[3].z) + v[2].y*(v[3].z-v[1].z) + v[3].y*(v[1].z-v[2].z);
	planeeq->b = v[1].z*(v[2].x-v[3].x) + v[2].z*(v[3].x-v[1].x) + v[3].z*(v[1].x-v[2].x);
	planeeq->c = v[1].x*(v[2].y-v[3].y) + v[2].x*(v[3].y-v[1].y) + v[3].x*(v[1].y-v[2].y);
	planeeq->d =-( v[1].x*(v[2].y*v[3].z - v[3].y*v[2].z) +
					  v[2].x*(v[3].y*v[1].z - v[1].y*v[3].z) +
					  v[3].x*(v[1].y*v[2].z - v[2].y*v[1].z) );
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


#endif 
