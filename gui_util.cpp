#include "gui_util.h"

#include <math.h>

double sq2 = sqrt(2);

void fill_circle(SDL_Renderer* rend, double x, double y, double r){
	for(int i = (int)floor(x-sq2*r); i <= (int)ceil(x+sq2*r); i++){
		double h = sqrt(r*r-(i-x)*(i-x));
		SDL_RenderDrawLine(rend, i, (int)ceil(y + h), i, (int)floor(y - h));
	}
	for(int i = (int)floor(y-sq2*r); i <= (int)ceil(y+sq2*r); i++){
		double h = sqrt(r*r-(i-y)*(i-y));
		SDL_RenderDrawLine(rend, (int)ceil(x + h), i, (int)floor(x - h), i);
	}
}

SDL_Texture* gen_circle(SDL_Renderer* rend, double rad){
	int s = 2*((int)(ceil(rad)));
	SDL_Texture* ret = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, s,s);
	SDL_Texture* temp = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, s,s);
	
	SDL_Rect all;
	all.x=all.y=0;
	all.w = all.h = s;
	
	unsigned char r,g,b,a;
	SDL_GetRenderDrawColor(rend, &r,&g,&b,&a);
	SDL_Texture* tar = SDL_GetRenderTarget(rend);
	SDL_SetRenderTarget(rend, temp);
	
	
	SDL_SetRenderDrawColor(rend, 0,0,0,0);
	SDL_RenderClear(rend);
	SDL_SetRenderDrawColor(rend, r,g,b,a);
	fill_circle(rend, s/2, s/2, rad);
	int* pixels = new int[s*s];
	
	SDL_RenderReadPixels(rend, &all, SDL_PIXELFORMAT_RGBA8888, pixels, sizeof(int)*s);
	SDL_UpdateTexture(ret, &all, pixels, sizeof(int)*s);
	
	SDL_SetTextureBlendMode(ret, SDL_BLENDMODE_BLEND);
	
	//clean up;
	delete[] pixels;
	SDL_SetRenderTarget(rend, tar);
	SDL_DestroyTexture(temp);
	return ret;
}

SDL_Texture* gen_uniform(SDL_Renderer* rend, int w, int h){
	SDL_Texture* ret = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, w,h);
	SDL_Texture* temp = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w,h);
	
	SDL_Rect all;
	all.x=all.y=0;
	all.w = w;all.h = h;
	
	SDL_Texture* tar = SDL_GetRenderTarget(rend);
	SDL_SetRenderTarget(rend, temp);
	
	
	SDL_RenderClear(rend);
	int* pixels = new int[w*h];
	
	SDL_RenderReadPixels(rend, &all, SDL_PIXELFORMAT_RGBA8888, pixels, sizeof(int)*w);
	SDL_UpdateTexture(ret, &all, pixels, sizeof(int)*w);
	
	SDL_SetTextureBlendMode(ret, SDL_BLENDMODE_BLEND);
	
	//clean up;
	delete[] pixels;
	SDL_SetRenderTarget(rend, tar);
	SDL_DestroyTexture(temp);
	return ret;
}

