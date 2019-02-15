#include "game_draw.h"
#include "game_consts.h"

#include <stdio.h>

#define BLOCK_SIZE 100

#define DRC(x) ((int)(BLOCK_SIZE * (x)))

#define WALL_D_T DRC(WALL_THK)

BoardDrawer::BoardDrawer(GameQ* q, SDL_Renderer* r){
	renderer = r;
	game = q;
}
void BoardDrawer::draw(){
	SDL_SetRenderDrawColor(renderer, 240,240,240,255);
	SDL_RenderClear(renderer);
	
	SDL_SetRenderDrawColor(renderer, 150,150,150,255);
	Maze* maze = game->get_round()->get_maze();
	for(int i = -1; i<maze->get_w(); i++){
		for(int j = -1; j<maze->get_h(); j++){
			SDL_Rect r;
			
			r.x = BLOCK_SIZE*(i);
			r.y = BLOCK_SIZE*(j+1);
			r.w = 2*WALL_D_T + BLOCK_SIZE;
			r.h = 2*WALL_D_T;
			if(maze->hwall(i,j)) SDL_RenderFillRect(renderer, &r);
			
			r.x = BLOCK_SIZE*(i+1);
			r.y = BLOCK_SIZE*(j);
			r.w = 2*WALL_D_T;
			r.h = 2*WALL_D_T + BLOCK_SIZE;
			if(maze->vwall(i,j)) SDL_RenderFillRect(renderer, &r);
		}
	}
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

#define BOARD_Y 45
#define BOARD_H 810
#define BOARD_W 1010


void GameDrawer::draw(){
	GameQEvent* event;
	while(event = game->get_event()){
		switch(event->get_type()){
		case GameQEvent::TYPE_RND_START:
			if(board_t) SDL_DestroyTexture(board_t);
			Maze* maze = game->get_round()->get_maze();
			w = WALL_D_T*2 + BLOCK_SIZE*maze->get_w();
			h = WALL_D_T*2 + BLOCK_SIZE*maze->get_h();
			board_t = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_UNKNOWN,SDL_TEXTUREACCESS_TARGET, w,h);
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
		r.w = std::min(BOARD_W, BOARD_H * w / h);
		r.h = std::min(BOARD_H, BOARD_W * h / w);
		
		r.x = (GSCR_W - r.w)/2;
		r.y = BOARD_Y + (BOARD_H-r.h)/2;
		
		SDL_RenderCopy(renderer, board_t, NULL, &r);
	}
}
