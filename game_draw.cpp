#include "game_draw.h"

BoardDrawer::BoardDrawer(GameQ* q, SDL_Renderer* r){
	renderer = r;
	game = q;
}
void BoardDrawer::draw(){
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
	SDL_RenderClear(renderer);
}



GameDrawer::GameDrawer(GameQ* q, SDL_Renderer* r){
	game = q;
	renderer = r;
	board = NULL;
}
GameDrawer::~GameDrawer(){
	if(board) delete board;
}
void GameDrawer::draw(){
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
	SDL_RenderClear(renderer);
}
