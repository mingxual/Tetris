#include "Game.h"

int main(int argc, char** argv)
{
	Game tetris;
	bool isInit = tetris.Initialize();
	if (!isInit) return 1;
	tetris.RunLoop();
	tetris.Shutdown();

	return 0;
}
