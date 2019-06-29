#ifndef _GAME_DRAW_H
#define _GAME_DRAW_H

#include <SDL2/SDL.h>
#include "game_query.h"
#include "images.h"
#include "texts.h"

#define GSCR_W 1280
#define GSCR_H 960

// draw game board
class BoardDrawer{
	GameQ* game;
	SDL_Renderer* renderer;
	TankImg* tank_images;
	SDL_Texture* circ;
public:
	BoardDrawer(GameQ* q, SDL_Renderer* rend, std::vector<int> img_inds);
	~BoardDrawer();
	void draw();
	TankImg* get_tank_img(int i);
};

class GameDrawer{
	GameQ* game;
	BoardDrawer* board;
	SDL_Renderer* renderer;
	SDL_Texture* board_t;
	
	std::vector<Msg*> scores;
	int w,h;
public:
	GameDrawer(GameQ* q, SDL_Renderer* rend, std::vector<int> img_inds);
	~GameDrawer();
	void draw();
	void update_score(int ind);
};

#endif