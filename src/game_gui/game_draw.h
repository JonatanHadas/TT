#ifndef _GAME_DRAW_H
#define _GAME_DRAW_H

#include <SDL2/SDL.h>
#include "../game/game_config.h"
#include "../game/game_query.h"
#include "images.h"
#include "../gui_utils/texts.h"

#include "../gui_utils/vfx.h"

#define GSCR_W 1280
#define GSCR_H 960

// shard effect
class Shard : public Effect{
	double x, y, vx, vy, ang, w;
	int cx,cy;
	bool flip;
	int timer, time;
	TankImg* img;
	EffectManager* smk;
	SDL_Texture* tex;
public:
	Shard(TankImg* img, double x, double y, EffectManager* smk, SDL_Texture* smk_tex);
	void step();
};

// draw game board
class BoardDrawer{
	GameQ* game;
	SDL_Renderer* renderer;
	TankImg* tank_images;
	SDL_Texture* circ;
	SDL_Texture* rect;
	
	EffectManager back_fx;
	EffectManager front_fx;
	EffectManager mid_fx;
	
	std::vector<int> colors;
	
public:
	BoardDrawer(GameQ* q, SDL_Renderer* rend, GameConfig& cf);
	~BoardDrawer();
	void draw();
	TankImg* get_tank_img(int i);
	
	void start_round();
	
	void tank_death(int ind);
	
	void place_mine(GameQEventCreateMine* e);
	
	void tank_stuck(GameQEventTankStuck* e);
};

typedef void (*end_func)(void*);

class GameDrawer{
	GameQ* game;
	BoardDrawer* board;
	SDL_Renderer* renderer;
	SDL_Texture* board_t;
	
	std::vector<Msg*> scores;
	int w,h;
	end_func oe;
	void* oep;
public:
	GameDrawer(GameQ* q, SDL_Renderer* rend, GameConfig& cf, end_func on_end, void* on_end_param);
	~GameDrawer();
	void draw();
	void update_score(int ind);
};

#endif