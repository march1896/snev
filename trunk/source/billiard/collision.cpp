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
	if ( pBall->pos.getX() - pBall->radius <= pTable->Left ) return true;
	if ( pBall->pos.getX() + pBall->radius >= pTable->Right ) return true;
	if ( pBall->pos.getY() - pBall->radius <= pTable->Bottom ) return true;
	if ( pBall->pos.getY() + pBall->radius >= pTable->Top ) return true;
	return false;
}

void Collision_twoBall( Ball* pBallA, Ball* pBallB )
{
	assert( pBallA );
	assert( pBallB );
	if ( !IsCollided_twoBall( pBallA, pBallB ) ) return;

	vector3 cline; // collision line

	//cline = pBallA->speed - pBallB->speed;
	cline = pBallA->pos - pBallB->pos;
	cline.normalize();
	float vA, vB; 		// before collision
	float vA_, vB_; 	// after collision
	float deltavA, deltavB;
	float mA, mB;

	vA = pBallA->speed.dot( cline );
	vB = pBallB->speed.dot( cline );
	mA = pBallA->weight;
	mB = pBallB->weight;

	if ( vB < 0.0 && vA > 0.0 ) return;

	if ( true ) // TODO: we assume elastic collsion, please add inelastic collision
	{
		vA_ = ( vA * ( mA - mB ) + 2 * mB * vB ) / ( mA + mB );
		vB_ = ( vB * ( mB - mA ) + 2 * mA * vA ) / ( mA + mB );
	}

	vector3 deltaV;
	deltavA = vA_ - vA;
	deltaV.setXYZ( deltavA * cline.getX(), deltavA * cline.getY(), deltavA * cline.getZ() );
	pBallA->speed += deltaV;

	deltavB = vB_ - vB;
	deltaV.setXYZ( deltavB * cline.getX(), deltavB * cline.getY(), deltavB * cline.getZ() );
	pBallB->speed += deltaV;
}

void Collision_withTable( Ball* pBall, Table* pTable )
{
	assert( pBall );
	assert( pTable );
	if ( !IsCollided_withTable( pBall, pTable ) ) return;

	// collide with left side
	if ( pBall->pos.getX() - pBall->radius < pTable->Left ) {
		// TODO: I don't know how deal with friction of rolling
		// the same situation occurs below
		// TODO: I assume all collisions are elastic collision
		//assert( pBall->speed.getX() <= 0.0 );
		pBall->speed.setX( - pBall->speed.getX() );
	}

	if ( pBall->pos.getX() + pBall->radius > pTable->Right ) {
		//this assert is not right because, when the ball hit the table border, it may also hit other balls
		//in this condition, the ball's speed may rapidly change and recollide with table border.
		//assert( pBall->speed.getX() >= 0.0 );
		pBall->speed.setX( - pBall->speed.getX() );
	}

	if ( pBall->pos.getY() - pBall->radius < pTable->Bottom ) {
		//assert( pBall->speed.getY() <= 0.0 );
		pBall->speed.setY( - pBall->speed.getY() );
	}

	if ( pBall->pos.getY() + pBall->radius > pTable->Top ) {
		//assert( pBall->speed.getY() >= 0.0 );
		pBall->speed.setY( - pBall->speed.getY() );
	}

	return;
}



