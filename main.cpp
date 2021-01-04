#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <ctime>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
static const float sEnemyPrepareTime = 12.0f;
static const float sEnemyAttackTime = 6.0f;
static const float sEndAttackTime = 6.0f;



//Starts up SDL and creates window
bool init();
void close();

//The window to be rendered to
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;



//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;
TTF_Font* gFont = NULL;
SDL_Color White = { 255, 255, 255 };


//game entity structs
struct segment {
	const int maxHP = 100000;
	int currentHP = maxHP;
	bool isAttacking = false;
	bool isPreparing = false;
	bool isPlayer = false;
} seg1, seg2, seg3, seg4, seg5;

struct player {
	bool isAlive = true;
	int lives = 3;
	std::string name;
};

struct GameState {
	std::vector<segment*> mSegment{ &seg1, &seg2, &seg3, &seg4, &seg5 };
	float mEnemyPrepareCounter = sEnemyPrepareTime;
	float mEnemyAttackCounter = sEnemyAttackTime;
	float mEndAttackCounter = sEndAttackTime;
	int currentPlayerPos = 2;
	SDL_Texture* texture = NULL;
};



bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_MAXIMIZED);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
				SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);


			}
		}
	}

	return success;
}



void close()
{
	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}


void UpdateAndRenderPlayer(int w, int h, GameState* gameState) {
	SDL_Surface* surface = IMG_Load("C:/Users/obish/Documents/Uni/GUH/Images/antmaker.png");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
	SDL_FreeSurface(surface);	
	SDL_Rect destination;
	destination.x = ((gameState->currentPlayerPos * w) / 5) + 140;
	destination.y = (4 * h / 5) - 25;
	destination.w = 100;
	destination.h = 100;
	SDL_RenderCopy(gRenderer, texture, NULL, &destination);

}

void UpdateAndRenderBullets() {

}

void UpdateAndRenderEnemies(int w, int h, GameState* gameState, float dt, SDL_Texture* enemyTexture) {

	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(gRenderer);

	//Draw line dividers
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
	SDL_RenderDrawLine(gRenderer, w / 5, 0, w / 5, h);
	SDL_RenderDrawLine(gRenderer, 2 * w / 5, 0, 2 * w / 5, h);
	SDL_RenderDrawLine(gRenderer, 3 * w / 5, 0, 3 * w / 5, h);
	SDL_RenderDrawLine(gRenderer, 4 * w / 5, 0, 4 * w / 5, h);


	gFont = TTF_OpenFont("C:/Users/obish/Documents/Fonts/steinberg/Steinberg.ttf", 12);


	SDL_Rect destination;
	for (int i = 0; i < 5; i++) {
		//if enemy is alive
		if (gameState->mSegment[i]->currentHP > 0) {
			//render sprite
			destination.x = (i * w / 5) + 20;
			destination.y = 0;
			destination.w = (w / 5) - 40;
			destination.h = h / 5;
			SDL_RenderCopy(gRenderer, enemyTexture, NULL, &destination);

			//hp text
			std::string hpText = std::to_string(gameState->mSegment[i]->currentHP);
			char const* hpchar = hpText.c_str();
			SDL_Surface* HPsurfaceMessage = TTF_RenderText_Solid(gFont, hpchar, White);
			SDL_Texture* HPMessage = SDL_CreateTextureFromSurface(gRenderer, HPsurfaceMessage);
			SDL_Rect HPMessage_rect;
			HPMessage_rect.x = (i * w / 5) + 150;
			HPMessage_rect.y = 100;
			HPMessage_rect.w = 100;
			HPMessage_rect.h = 30;
			SDL_RenderCopy(gRenderer, HPMessage, NULL, &HPMessage_rect);
			SDL_FreeSurface(HPsurfaceMessage);
		}

		if (i == gameState->currentPlayerPos) {
			gameState->mSegment[i]->isPlayer = true;
		}
		else {
			gameState->mSegment[i]->isPlayer = false;
		}

		//reduce hp
		if (gameState->mSegment[i]->isPlayer == true) {
			gameState->mSegment[i]->currentHP -= 5;		//temp value, will be adjusted for balancing
		}


		//check if is preparing
		if (gameState->mSegment[i]->isPreparing == true) {
			gameState->mEnemyAttackCounter -= dt;
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xEE, 0x00, 50);
			SDL_Rect fillRect6 = { ((i)*w / 5), 0, (w / 5), h };
			SDL_RenderFillRect(gRenderer, &fillRect6);
			if (gameState->mEnemyAttackCounter < 0.0f)
			{
				gameState->mSegment[i]->isPreparing = false;
				gameState->mEnemyAttackCounter = sEnemyAttackTime;
				gameState->mSegment[i]->isAttacking = true;
			}
		}
		else if (gameState->mSegment[i]->isAttacking == true) {
			gameState->mEndAttackCounter -= dt;
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 50);
			SDL_Rect fillRect6 = { ((i)*w / 5), 0, (w / 5), h };
			SDL_RenderFillRect(gRenderer, &fillRect6);
			if (gameState->mEndAttackCounter < 0.0f)
			{
				gameState->mEndAttackCounter = sEndAttackTime;
				gameState->mSegment[i]->isAttacking = false;
			}
		}
	}
	gameState->mEnemyPrepareCounter -= dt;
	if (gameState->mEnemyPrepareCounter < 0.0f) {
		int rand_seg = (rand() % 5);
		while (gameState->mSegment[rand_seg]->isPreparing == true || gameState->mSegment[rand_seg]->isAttacking == true) {
			rand_seg = (rand() % 5);
		}
		gameState->mSegment[rand_seg]->isPreparing = true;
		gameState->mEnemyPrepareCounter = sEnemyPrepareTime;
	}
}

void UpdateAndRender() {

}

int main(int argc, char* args[])
{
	GameState gameState = {};
	//SDL_Surface* Surface = IMG_Load("C:/Users/obish/Documents/Uni/GUH/Images/ship (15).png");
	//SDL_Texture* enemyTexture = SDL_CreateTextureFromSurface(gRenderer, Surface);
	//SDL_FreeSurface(Surface);
	//Start up SDL and create window
	gameState.texture = IMG_LoadTexture(gRenderer, "C:/Users/obish/Documents/Uni/GUH/Images/ship (15).png");
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		//While application is running
		while (!quit)
		{
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
				if (e.type == SDL_KEYDOWN) {
					switch (e.key.keysym.sym) {
					case SDLK_LEFT:
						if (gameState.currentPlayerPos != 0) {
							gameState.currentPlayerPos -= 1;
						}
						break;
					case SDLK_RIGHT:
						if (gameState.currentPlayerPos != 4) {
							gameState.currentPlayerPos += 1;
						}
						break;
					}
				}
			}
			TTF_Init();
			SDL_DisplayMode dm;

			if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
			{
				SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
				return 1;
			}

			int w, h;
			w = dm.w;
			h = dm.h;
			//std::clock_t c_start = std::clock();
			//float dt = c_start;
			//dt = (dt > 0.01666f ? 0.01666f : dt);
			float dt = 0.01666f;
			UpdateAndRenderEnemies(w, h, &gameState, dt, gameState.texture);
			UpdateAndRenderPlayer(w, h, &gameState);

			//Update screen
			SDL_RenderPresent(gRenderer);
		}
	}

	close();

	return 0;

}