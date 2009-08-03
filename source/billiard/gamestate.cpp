#include "gamestate.h"

GameState::GameState()
{
}

GameState::~GameState()
{
}

GameState* GameState::getInstance()
{
	static GameState _instance;

	return &_instance;
}
