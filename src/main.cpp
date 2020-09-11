#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <enet/enet.h>

#include "gui_utils/gui.h"
#include "gui_utils/texts.h"
#include "keys.h"
#include "game_gui/sounds.h"
#include "game_gui/images.h"

#include "main_scr.h"

SDL_Window*  screen;
SDL_Renderer* rend;

void close_rend(){
	SDL_DestroyRenderer(rend);
}
void close_window(){
	SDL_DestroyWindow(screen);
}

State* init_state(Main* gui){
	new MainScr(gui, NULL);
}

int main(int argc, char* argv[]){
	srand(time(0));	

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		fprintf(stderr, "Error initializing SDL:\n%s\n", SDL_GetError());
		return 0;
	}
	atexit(SDL_Quit);
	
	if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
		fprintf(stderr, "Error initializing SDL_image:\n%s\n", IMG_GetError());
		return 0;
	}
	atexit(IMG_Quit);
		
	if(TTF_Init() < 0){
		fprintf(stderr, "Error initializing SDL_ttf:\n%s\n", TTF_GetError());
	}
	atexit(TTF_Quit);
	
	if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, CHANNELS, 2048 ) < 0){
		fprintf(stderr, "Error initializing SDL_mixer:\n%s\n", Mix_GetError());
	}
	atexit(Mix_Quit);
	
	if(! load_sounds()){
		fprintf(stderr, "Error loading sounds:\n%s\n", Mix_GetError());
		return 0;
	}
	
	if(enet_initialize()<0){
		fprintf(stderr, "Error initializing ENet\n");
		return 0;
	}
	atexit(enet_deinitialize);
	
	if(!load_fonts()){
		fprintf(stderr, "Error while loading fonts\n%s%s\n",SDL_GetError(), TTF_GetError());
		return 0;
	}
	if(!load_keys()){
		fprintf(stderr, "Error while loading keysets\n");
		return 0;
	}
	
	screen = SDL_CreateWindow(
										"Tank Trouble",
										SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
										//1024,768,0);
										0,0,
										SDL_WINDOW_FULLSCREEN_DESKTOP);
	
	if(screen == NULL){
		fprintf(stderr,"Error while opening window:\n%s\n", SDL_GetError());
		return 0;
	}
	atexit(close_window);

	rend = SDL_CreateRenderer(screen, -1,SDL_RENDERER_TARGETTEXTURE);
	
	if(rend == NULL){
		fprintf(stderr,"Error while opening renderer:\n%s\n", SDL_GetError());
		return 0;
	}
	atexit(close_rend);
	
	if(!load_images(rend)){
		fprintf(stderr, "Error while loading images\n%s%s\n",SDL_GetError(), IMG_GetError());
		return 0;
	}
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
	//SDL_RenderSetLogicalSize(rend, 1280, 960);

	Main* m = new Main(rend, init_state);
	m->mainloop();
	delete m;
	
	return 0;
}