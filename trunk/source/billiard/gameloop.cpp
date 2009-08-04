#include "ball.h"
#include "vector.h"
#include "collision.h"
#include "table.h"
#include <cstdio>

static Ball* pBalls;
static Table* pTable;

void GameInit()
{
	pBalls = new Ball[3];
	//pBalls[0].SetAllAttributes( 0.0, 0.0, 1.0, 0.0, 0.0, WHITE );
	//pBalls[1].SetAllAttributes( 1.0, 1.0, 1.0, 0.0, 0.0, BLACK );
	//pBalls[2].SetAllAttributes( 2.0, 2.0, 1.0, 0.0, 0.0, RED );

	pTable = new Table;
}

void GameDeinit()
{
	delete[] pBalls;
	delete pTable;
}

bool MainLoop()
{
	return true;
}

int main()
{
	bool Running;

	GameInit();

	printf("hello, world\n");
	while ( true )
	{
		Running = MainLoop();
		//if ( !Running ) break;
		if ( true ) break;
	}
	GameDeinit();

	return 0;
}

