#include "Grid.h"
#include "Game.h"

Grid::Grid(Game* game)
{
	mGame = game;
	mGame->AddGrid(this);
}

Grid::~Grid()
{
	mGame->RemoveGrid(this);
}