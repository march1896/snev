#include "collision.h"
#include "ball.h"
#include "table.h"
#include "vector.h"
#include <cassert>
#include <cmath>

bool IsCollided_twoBall( Ball* pBallA, Ball* pBallB )
{
	float disx, disy, dist;
	disx = fabs( pBallA->pos.getX() - pBallB->pos.getX() );
	disy = fabs( pBallA->pos.getY() - pBallB->pos.getY() );

	dist = fabs( pBallA->radius + pBallB->radius );

	return ( disx * disx + disy * disy < dist * dist ) ? true : false;
}

bool IsCollided_withTable( Ball* pBall, Table* pTable )
{
	/*
	if ( pBall->pos.getX() - pBall->radius < pTable->Left ) return true;
	if ( pBall->pos.getX() + pBall->radius > pTable->Right ) return true;
	if ( pBall->pos.getY() - pBall->radius < pTable->Bottom ) return true;
	if ( pBall->pos.getY() + pBall->radius > pTable->Top ) return true;
	return false;
	*/
}

void CalculateNewState_twoBall( Ball* pBallA, Ball* pBallB )
{
	assert( pBallA );
	assert( pBallB );
	if ( !IsCollided_twoBall( pBallA, pBallB ) ) return;

	vector3 cline; // collision line

	cline = pBallA->pos - pBallB->pos;
	cline.normalize();
	float vA, vB; 		// before collision
	float vA_, vB_; 	// after collision
	float mA, mB;

	vA = pBallA->speed.dot( cline );
	vB = pBallB->speed.dot( cline );
	mA = pBallA->weight;
	mB = pBallB->weight;

	if ( true ) // TODO: we assume elastic collsion, please add inelastic collision
	{
		vA_ = ( vA * ( mA - mB ) + 2 * mB * vB ) / ( mA + mB );
		vB_ = ( vB * ( mB - mA ) + 2 * mA * vA ) / ( mA + mB );
	}

	vector3 deltaV;
	deltaV.setXYZ( vA_ * cline.getX(), vA_ * cline.getY(), vA_ * cline.getZ() );
	//pBallA->pos += deltaV;

	deltaV.setXYZ( vB_ * cline.getX(), vB_ * cline.getY(), vB_ * cline.getZ() );
	//pBallB->pos += deltaV;
}

void CalculateNewState_withTable( Ball* pBall, Table* pTable )
{
}



