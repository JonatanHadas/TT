#include "game_draw.h"

BoardDrawer::BoardDrawer(GameQ* q, SDL_Renderer* r){
	renderer = r;
	game = q;
}
void BoardDrawer::draw(){
	SDL_SetRenderDrawColor(renderer, 240,240,240,255);
	SDL_RenderClear(renderer);
}



GameDrawer::GameDrawer(GameQ* q, SDL_Renderer* r){
	game = q;
	renderer = r;
	board = new BoardDrawer(q,r);
	board_t = NULL;
}
GameDrawer::~GameDrawer(){
	if(board) delete board;
	if(board_t) SDL_DestroyTexture(board_t);
}
#include <stdio.h>
void GameDrawer::draw(){
	GameQEvent* event;
	while(event = game->get_event()){
		switch(event->get_type()){
		case GameQEvent::TYPE_RND_START:
			if(board_t) SDL_DestroyTexture(board_t);
			board_t = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_UNKNOWN,SDL_TEXTUREACCESS_TARGET,1000,1000);
			break;
		}
	}
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
	SDL_RenderClear(renderer);
	
	if(board_t){
		SDL_Texture* tex = SDL_GetRenderTarget(renderer);
		SDL_SetRenderTarget(renderer, board_t);
		board->draw();
		SDL_SetRenderTarget(renderer,tex);
		
		SDL_Rect r;
		r.w = r.h = GSCR_H;
		r.x = (GSCR_W - r.w)/2;
		r.y = 0;
		
		SDL_RenderCopy(renderer, board_t, NULL, &r);
	}
}
