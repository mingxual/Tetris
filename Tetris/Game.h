#pragma once

#include "SDL/SDL.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Game {
public:
	Game();
	bool Initialize();
	void Shutdown();
	void RunLoop();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);
	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);
	SDL_Texture* GetTexture(const char* filename);
	void AddGrid(class Grid* grid);
	void RemoveGrid(class Grid* grid);
	SDL_Renderer* GetRenderer() { return render; }

private:
	void DrawLines();

	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	SDL_Window* window;
	SDL_Renderer* render;

	// Color of bg
	SDL_Color grid_bg_color = { 25, 25, 25, 255 };
	// Some setting numbers
	int row = 16;
	int col = 21;
	float size = 30.0f;

	// Variable to control the exit of game loop
	bool running;

	Uint32 time_count = 0;

	std::vector<class Actor*> mActors;
	std::unordered_map<std::string, SDL_Texture*> mTextures;
	std::vector<class Grid*> mGrids;
	
	void LoadData();
	void UnloadData();
};