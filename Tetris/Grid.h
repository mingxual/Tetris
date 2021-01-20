#pragma once
#include "SDL/SDL.h"
#include "Math.h"

class Grid
{
public:
	Grid(class Game* game);
	~Grid();
	const Vector2& GetPosition() const { return mPosition; }
	void SetPosition(const Vector2& pos) { mPosition = pos; }

private:
	class Game* mGame;
	Vector2 mPosition;
};

