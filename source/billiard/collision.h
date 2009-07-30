#ifndef _BALL_COLLISION_H_
#define _BALL_COLLISION_H_
class Ball;
class Table;

bool IsCollided_twoBall( Ball* pBallA, Ball* pBallB );
bool IsCollided_withTable( Ball* pBall, Table* pTable );

void Collision_twoBall( Ball* pBallA, Ball* pBallB );
void Collision_withTable( Ball* pBall, Table* pTable );
#endif
