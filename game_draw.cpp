#include "game_draw.h"
#include "game_consts.h"

#include "gui_util.h"

#include "geom.h"
#include <vector>

#include <stdio.h>

#define BLOCK_SIZE 100

#define DRC(x) ((int)(BLOCK_SIZE * (x)))

#define WALL_D_T DRC(WALL_THK)

#define TANK_D_W DRC(TANK_W)
#define TANK_D_H DRC(TANK_H)

#define TANKC_D_W TANK_D_W
#define TANKC_D_H DRC(TANK_H * 1.2)



BoardDrawer::BoardDrawer(GameQ* q, SDL_Renderer* r, std::vector<int> img_inds){
	renderer = r;
	game = q;
	tank_images = new TankImg[q->get_tank_num()];
	for(int i = 0; i<q->get_tank_num(); i++) {
		generate_tank(img_inds[i], r, tank_images + i);
	}
	SDL_SetRenderDrawColor(r, 255,255,255,255);
	circ = gen_circle(r,20.0);
}
BoardDrawer::~BoardDrawer(){
	delete[] tank_images;
	SDL_DestroyTexture(circ);
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
	
	auto shots = game->get_round()->get_shots();
	for(auto it = shots.begin(); it != shots.end(); it++){
		SDL_Rect r;
		ShotQ* sht;
		switch((*it)->get_type()){
		case GenShot::TYPE_REG:
			sht = (ShotQ*)(*it);
			r.w = r.h = DRC(2*sht->get_r());
			r.x = WALL_D_T + DRC(sht->get_x()) - r.w/2;
			r.y = WALL_D_T + DRC(sht->get_y()) - r.w/2;
			SDL_SetTextureColorMod(circ, 0,0,0);
			SDL_RenderCopy(renderer, circ, NULL, &r);
			break;
		}
		
		delete (*it);
	}
	
	for(int i = 0; i < game->get_tank_num(); i++){
		TankQ* t = game->get_tank(i);
		SDL_Color col = get_tank_col(i);
		SDL_SetRenderDrawColor(renderer,col.r,col.g,col.b,255);
		if(!t->is_dead()){
			double x = t->get_x(), y = t->get_y(), ang = t->get_ang();
			
			SDL_Rect r;
			SDL_Point p;
			p.x = WALL_D_T + DRC(x);
			p.y = WALL_D_T + DRC(y);
			r.w = TANK_D_W; r.h = TANK_D_H;
			r.x = p.x - r.w/2;
			r.y = p.y - r.h/2;
			
			p.x -= r.x; p.y -= r.y;
			
			SDL_RenderCopyEx(renderer,tank_images[i].body, NULL, &r, RAD2DEG(ang)+90, &p, SDL_FLIP_NONE);
			
			p.x = WALL_D_T + DRC(x);
			p.y = WALL_D_T + DRC(y);
			r.w = TANKC_D_W; r.h = TANKC_D_H;
			r.x = p.x - r.w/2;
			r.y = p.y - r.h/2;
			
			p.x -= r.x; p.y -= r.y;
			
			SDL_RenderCopyEx(renderer,tank_images[i].cannon, NULL, &r, RAD2DEG(ang)+90, &p, SDL_FLIP_NONE);
		}
	}
}



GameDrawer::GameDrawer(GameQ* q, SDL_Renderer* r, std::vector<int> img_inds){
	game = q;
	renderer = r;
	board = new BoardDrawer(q,r,img_inds);
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