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

std::pair<Upgrade::Type, Img> upg2img_a[UPG_NUM] = {
	{Upgrade::GATLING, IMG_GATLING_SYM},
	{Upgrade::LASER, IMG_LASER_SYM},
	{Upgrade::BOMB, IMG_BOMB_SYM},
	};
std::map<Upgrade::Type, Img> upg2img(upg2img_a,upg2img_a+UPG_NUM);

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
	
	auto upgs = game->get_round()->get_upgs();
	for(auto it = upgs.begin(); it!=upgs.end(); it++){
		SDL_Rect r;
		r.w = r.h = DRC(UPG_SIZE);
		r.x = DRC((*it)->x + 0.5)-r.w/2;
		r.y = DRC((*it)->y + 0.5)-r.h/2;
		
		SDL_RenderCopyEx(renderer, get_img(upg2img[(*it)->type]), NULL, &r, UPG_ANG, NULL, SDL_FLIP_NONE);
		
		delete (*it);
	}
	
	auto shots = game->get_round()->get_shots();
	for(auto it = shots.begin(); it != shots.end(); it++){
		SDL_Rect r;
		ShotQ* sht;
		switch((*it)->get_type()){
		case GenShot::TYPE_REG:
		case GenShot::TYPE_GATLING:
		case GenShot::TYPE_BOMB:
			sht = (ShotQ*)(*it);
			r.w = r.h = DRC(2*sht->get_r());
			r.x = WALL_D_T + DRC(sht->get_x()) - r.w/2;
			r.y = WALL_D_T + DRC(sht->get_y()) - r.w/2;
			SDL_SetTextureColorMod(circ, 0,0,0);
			SDL_RenderCopy(renderer, circ, NULL, &r);
			break;
		case GenShot::TYPE_LASER:
			sht = (ShotQ*)(*it);
			SDL_SetRenderDrawColor(renderer, 255,0,0,255);
			SDL_Point* ps = new SDL_Point[sht->get_colls().size()];
			for(int i = 0; i<sht->get_colls().size(); i++){
				ps[i].x = DRC(sht->get_colls()[i].first);
				ps[i].y = DRC(sht->get_colls()[i].second);
			}
			SDL_RenderDrawLines(renderer, ps, sht->get_colls().size());
			delete ps;
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
			
			SDL_Texture* cannon;
			switch(t->get_state()){
			case Tank::REG:
				cannon = tank_images[i].cannon;
				break;
			case Tank::GATLING:
				cannon = tank_images[i].gatling[0];
				break;
			case Tank::GATLING_WAIT:
			case Tank::GATLING_SHOOT:
				cannon = tank_images[i].gatling[game->get_time()%3];
				break;
			case Tank::LASER:
			case Tank::LASER_SHOOT:
				cannon = tank_images[i].laser;
				break;
			case Tank::BOMB:
			case Tank::BOMB_SHOOT:
				cannon = tank_images[i].thick_cannon;
				break;
			}
			
			SDL_RenderCopyEx(renderer,cannon, NULL, &r, RAD2DEG(ang)+90, &p, SDL_FLIP_NONE);
		}
	}
}
TankImg* BoardDrawer::get_tank_img(int i){
	return tank_images+i;
}


GameDrawer::GameDrawer(GameQ* q, SDL_Renderer* r, std::vector<int> img_inds){
	game = q;
	renderer = r;
	board = new BoardDrawer(q,r,img_inds);
	board_t = NULL;
	for(int i = 0; i<game->get_team_num(); i++){
		scores.push_back(NULL);
		update_score(i);
	}
}
GameDrawer::~GameDrawer(){
	if(board) delete board;
	if(board_t) SDL_DestroyTexture(board_t);
	for(int i = 0; i<game->get_team_num(); i++){
		if(scores[i]) delete scores[i];
	}
}
void GameDrawer::update_score(int i){
	if(scores[i]) delete scores[i];
	char txt[100];
	sprintf(txt,"%d",game->get_team(i)->get_score());
	scores[i] = new Msg(txt, {0,0,0,255}, FONT_MID, renderer);
}

#define BOARD_Y 45
#define BOARD_H 810
#define BOARD_W 1010


#define SCORE_MAR 0
#define SCORE_Y 930

#define TKIMG_W 60
#define TKIMG_H 33
#define TKIMG_R 50
#define TKIMG_A (M_PI/4)

void GameDrawer::draw(){
	GameQEvent* event;
	while(event = game->get_event()){
		Maze* maze;
		switch(event->get_type()){
		case GameQEvent::TYPE_RND_START:
			if(board_t) SDL_DestroyTexture(board_t);
			maze = game->get_round()->get_maze();
			printf("%p\n", game->get_round());
			w = WALL_D_T*2 + BLOCK_SIZE*maze->get_w();
			h = WALL_D_T*2 + BLOCK_SIZE*maze->get_h();
			board_t = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_UNKNOWN,SDL_TEXTUREACCESS_TARGET, w,h);
			break;
		case GameQEvent::TYPE_SCORE:
			update_score(((GameQEventScore*)event)->get_ind());
			break;
		}
	}
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
	SDL_RenderClear(renderer);
	
	int num = 0;
	std::vector<int> nums;
	for(int i = 0; i<game->get_team_num(); i++){
		nums.push_back(num);
		num += game->get_team(i)->get_tank_num();
	}
	nums.push_back(num);
	
	double dx = (GSCR_W - 2*SCORE_MAR)/num;
	
	for(int i = 0; i<game->get_team_num(); i++){
		scores[i]->render_centered(SCORE_MAR + (int)(0.5*dx*(nums[i]+nums[i+1])),SCORE_Y,AL_CENTER);
		
		int nd = (game->get_team(i)->get_tank_num()-1);
		double da = nd==0 ? 0 : (M_PI - TKIMG_A*2)/nd;
		double ia = nd==0 ? M_PI/2 : M_PI - TKIMG_A;
		for(int j = 0; j<game->get_team(i)->get_tank_num(); j++){
			double x = SCORE_MAR + dx * (nums[i]+j+0.5), y = SCORE_Y;
			
			rotate_add(ia - da*j, -TKIMG_R, 0, x,y);
			SDL_Rect r;
			r.w = TKIMG_W; r.h = TKIMG_H;
			r.x = x-r.w/2; r.y = y-r.h/2;
			SDL_RenderCopyEx(	renderer, board->get_tank_img(game->get_team(i)->get_tank(j)->get_ind())->image,
								NULL, &r,
								0,NULL,
								(j > game->get_team(i)->get_tank_num()/2) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
		}
	}
	
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
