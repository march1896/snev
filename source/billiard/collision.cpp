#include "collision.h"
#include "ball.h"
#include "table.h"
#include <cmath>

bool IsCollided_twoBall( Ball* pBallA, Ball* pBallB )
{
	float disx, disy, dist;
	disx = fabs( pBallA->x - pBallB->x );
	disy = fabs( pBallA->y - pBallB->y );

	dist = fabs( pBallA->radius + pBallB->radius );

	return ( disx * disx + disy * disy < dist * dist ) ? true : false;
}

bool IsCollided_withTable( Ball* pBall, Table* pTable );
{
	if ( pBall->x - pBall->radius < pTable->Left ) return true;
	if ( pBall->x + pBall->radius > pTable->Right ) return true;
	if ( pBall->y - pBall->radius < pTable->Bottom ) return true;
	if ( pBall->y + pBall->radius > pTable->Top ) return true;
	return false;
}

void CalculateNewState_twoBall( Ball* pBallA, Ball* pBallB );
{
}

void CalculateNewState_withTable( Ball* pBall, Table* pTable );
{
}



