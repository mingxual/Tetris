#include <SDL/SDL_image.h>
#include <algorithm>
#include <string>
#include <fstream>
#include "Random.h"
#include "Game.h"
#include "Actor.h"
#include "Grid.h"
#include "Font.h"

Game::Game():window(NULL),render(NULL),running(true), mActors(NULL)
{ 	

}

bool Game::Initialize() 
{
	Random::Init();

	// Init SDL
	int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (result != 0) 
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Create the window
	window = SDL_CreateWindow(
		"Tetris",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		col * size,
		row * size,
		0
	);
	if (window == NULL) 
	{
		SDL_Log("Could not create window: %s", SDL_GetError());
		return false;
	}

	// Create the render
	render = SDL_CreateRenderer(
		window, 
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (render == NULL) 
	{
		SDL_Log("Could not create renderer: %s\n", SDL_GetError());
		return false;
	}

	// Initialize SDL Image
	int flags = IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted & flags) != flags)
	{
		SDL_Log("IMG_Init: Failed to init required png support.\n");
		SDL_Log("IMG_Init: %s\n", IMG_GetError());
	}

	LoadData();
	time_count = SDL_GetTicks();

	return true;
}

void Game::RunLoop() 
{
	while (running) 
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput() 
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) 
	{
		switch (event.type) 
		{
		case SDL_QUIT:
			running = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			// mButton->CheckClickState();
			break;
		}
	}
	// Create keyboard input
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE]) 
	{
		running = false;
	}

	// Loop over
	std::vector<Actor*> copyactors = mActors;
	for (auto actor : copyactors)
	{
		actor->ProcessInput(state);
	}
}

void Game::UpdateGame() 
{
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), time_count + 16));

	// Count the time
	float deltatime = (SDL_GetTicks() - time_count) / 1000.0f;
	if (deltatime > 0.033f) deltatime = 0.033f;
	time_count = SDL_GetTicks();

	// Shallow copy actors vector
	std::vector<Actor*> copyactors = mActors;
	for (auto actor : copyactors) 
	{
		actor->Update(deltatime);
	}
	// Insert destroy-state actor to the list
	std::vector<Actor*> destroylist;
	for (auto actor : mActors)
	{
		if (actor->GetState() == ActorState::Destroy)
		{
			destroylist.push_back(actor);
		}
	}
	// Destroy the actor
	for (auto actor : destroylist)
	{
		delete actor;
	}
}

void Game::GenerateOutput() 
{
	SDL_SetRenderDrawColor(render, grid_bg_color.r, grid_bg_color.g, grid_bg_color.b, grid_bg_color.a);
	SDL_RenderClear(render);

	SDL_Texture* grid_texture = GetTexture("Assets/BlockE.png");
	for (size_t i = 0; i < mGrids.size(); ++i)
	{
		Vector2 pos = mGrids[i]->GetPosition();

		SDL_Rect r;
		r.w = size;
		r.h = size;
		r.x = pos.x;
		r.y = pos.y;

		SDL_RenderCopyEx(render,
			grid_texture,
			nullptr,
			&r,
			0,
			nullptr,
			SDL_FLIP_NONE
			);
	}

	SDL_RenderPresent(render);
}

void Game::Shutdown() 
{
	UnloadData();
	IMG_Quit();
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Game::AddActor(Actor* actor) 
{
	mActors.push_back(actor);
}

void Game::RemoveActor(Actor* actor) 
{
	std::vector<Actor*>::iterator it = std::find(mActors.begin(), mActors.end(), actor);
	mActors.erase(it);
}

SDL_Texture* Game::GetTexture(const char* filename)
{
	// Check if texture exists
	std::unordered_map<std::string, SDL_Texture*>::iterator it = mTextures.find(filename);
	if (it != mTextures.end())
	{
		return it->second;
	}

	// Import the file
	SDL_Surface* image = IMG_Load(filename);
	if (!image)
	{
		SDL_Log("IMG_Load: %s\n", IMG_GetError());
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(render, image);
	SDL_FreeSurface(image);
	mTextures.insert(std::make_pair(filename, texture));
	
	return texture;
}

void Game::AddGrid(Grid* grid)
{
	mGrids.push_back(grid);
}

void Game::RemoveGrid(Grid* grid)
{
	std::vector<Grid*>::iterator it = std::find(mGrids.begin(), mGrids.end(), grid);
	mGrids.erase(it);
}

void Game::LoadData()
{
	std::ifstream mfile;
	mfile.open("Assets/Layout.txt");

	// Error check
	if (!mfile.is_open())
	{
		SDL_Log("Could not load Layout.txt");
		return;
	}

	std::string currLine;
	float left = 0.0f, top = 0.0f;
	// A placeholder to the pointer
	Grid* grid = NULL;

	while (getline(mfile, currLine))
	{
		for (auto character : currLine)
		{
			switch (character)
			{
			case 'A':
				grid = new Grid(this);
				grid->SetPosition(Vector2(left, top));
				break;
			default:
				break;
			}
			left += size;
		}
		top += size;
		left = 0.0f;
	}
}

void Game::UnloadData()
{
	for (int i = mGrids.size() - 1; i >= 0; --i)
	{
		delete mGrids[i];
	}
}