#ifndef _GUI_UTILS_H
#define _GUI_UTILS_H

#include <SDL2/SDL.h>

void fill_circle(SDL_Renderer* rend, double x, double y, double r);
SDL_Texture* gen_circle(SDL_Renderer* rend, double rad);

#endif