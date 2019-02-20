#ifndef _IMAGES_H
#define _IMAGES_H

#include "SDL2/SDL.h"

struct TankImg{
	SDL_Texture* body;
	SDL_Texture* cannon;
	SDL_Texture* gutling[3];
	SDL_Texture* laser;
	SDL_Texture* ray_gun0;
	SDL_Texture* ray_gun1;
	SDL_Texture* ray_gun2;
	SDL_Texture* ray_gun3;
	SDL_Texture* launcher;
	SDL_Texture* missile;
	SDL_Texture* thick_cannon;
	SDL_Texture* mine_off;
	SDL_Texture* mine_on;
	SDL_Texture* shards;
	SDL_Texture* deathray;
	SDL_Texture* broadcast;
	SDL_Texture* image;
	SDL_Color col;
	TankImg();
	~TankImg();
	bool check();
};

enum Img{
	IMG_GUTLING_SYM, IMG_LASER_SYM, IMG_DEATH_RAY_SYM, IMG_WIFI_SYM, IMG_MISSILE_SYM, IMG_BOMB_SYM, IMG_MINE_SYM, 
	IMG_FRAGMENT
};

bool load_images(SDL_Renderer* rend);

SDL_Color get_tank_col(int ind);

SDL_Texture* tex_img(int ind, SDL_Surface* src, SDL_Renderer* rend, bool use_tex);
void generate_tank(int ind, SDL_Renderer* rend, TankImg* img);

SDL_Texture* get_img(Img i);

#endif