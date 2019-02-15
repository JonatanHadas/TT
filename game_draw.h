#ifndef _GAME_DRAW_H
#define _GAME_DRAW_H

#include <SDL2/SDL.h>
#include "game_query.h"

#define GSCR_W 1280
#define GSCR_H 960

// draw game board
class BoardDrawer{
	GameQ* game;
	SDL_Renderer* renderer;
public:
	BoardDrawer(GameQ* q, SDL_Renderer* rend);
	void draw();
};

class GameDrawer{
	GameQ* game;
	BoardDrawer* board;
	SDL_Renderer* renderer;
	SDL_Texture* board_t;
	int w,h;
public:
	GameDrawer(GameQ* q, SDL_Renderer* rend);
	~GameDrawer();
	void draw();
};

#endif