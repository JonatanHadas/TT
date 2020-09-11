#include "endgame.h"

#include <algorithm>

struct Comp{
	int* ar;
	bool operator ()(int a, int b) const{
		return ar[a]>ar[b];
	}
};

#define SCR_W 1280
#define SCR_H 960

#define SCORE_TITLE_Y 30 
#define SCORE_TITLE_H 60

#define SCORE_S 150
#define SCORE_NAME_Y 80
#define SCORE_TANK_Y 10
#define SCORE_SCR_Y 110
#define SCORE_LN_M 30

#define TANK_IW 100
#define TANK_IH 55

#include "stdio.h"

EndGame::EndGame(Main* u, GameQ* q, std::vector<int> img_inds, std::vector<std::string> nms) : State(u){
	
	u->set_screen_size(SCR_W, SCR_H);
	timer  = 60;
	
	names = nms;
	tank_images = new TankImg[q->get_tank_num()];
	for(int i = 0; i<q->get_tank_num(); i++){
		teams.push_back(q->get_tank(i)->get_team()->get_ind());
		names_m.push_back(new Msg(nms[i].c_str(), {0,0,0,255}, FONT_MID, upper->get_renderer()));
		generate_tank(img_inds[i], upper->get_renderer(), tank_images + i);
	}
	for(int i = 0; i<q->get_team_num(); i++){
		inds.push_back(i);
		scores.push_back(q->get_team(i)->get_score());
		
		char s[100];
		sprintf(s, "%d", scores.back());
		scores_m.push_back(new Msg(s, {0,0,0,255}, FONT_MID, upper->get_renderer()));
	}
	std::sort<int*, Comp>(&inds[0], &inds[q->get_team_num()], {&scores[0]});
	
	title = new Msg("Game Over", {0,0,0,255}, FONT_BIG, upper->get_renderer());
}

EndGame::~EndGame(){
	for(int i = 0; i<names_m.size(); i++) delete names_m[i];
	for(int i = 0; i<scores_m.size(); i++) delete scores_m[i];
	delete[] tank_images;
	delete title;
}

bool EndGame::step(){
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		switch(e.type){
		case SDL_QUIT:
			return true;
		case SDL_KEYDOWN:
		case SDL_MOUSEBUTTONDOWN:
			if(timer>0);
			return true;
			break;
		}
	}
	
	if(timer>0) timer--;
	
	SDL_SetRenderDrawColor(upper->get_renderer(), 255,255,255,255);
	SDL_RenderClear(upper->get_renderer());
	
	title->render_centered(SCR_W/2, SCORE_TITLE_Y, AL_CENTER);
	
	std::vector<std::vector<int>> tinds(inds.size(), std::vector<int>());
	for(int i = 0; i<names.size(); i++){
		tinds[teams[i]].push_back(i);
	}
	
	for(int i = 0; i<inds.size(); i++){
		SDL_SetRenderDrawColor(upper->get_renderer(), 0,0,0,255);
		int y = i*SCORE_S + SCORE_TITLE_H;
		if(i != 0) SDL_RenderDrawLine(upper->get_renderer(), SCORE_LN_M, y, SCR_W-SCORE_LN_M, y);
		scores_m[i]->render_centered(SCR_W/2, y + SCORE_SCR_Y, AL_CENTER);
		
		for(int j = 0; j<tinds[i].size(); j++){
			SDL_Rect r;
			int ti = tinds[i][j];
			r.w = TANK_IW;
			r.h = TANK_IH;
			r.y = y;
			r.x = (2*j+1)*SCR_W / (tinds[i].size()*2);
			names_m[ti]->render_centered(r.x, r.y + SCORE_NAME_Y, AL_CENTER);
			r.x -= r.w/2;
			r.y += SCORE_TANK_Y;
			SDL_RenderCopy(upper->get_renderer(), tank_images[ti].image, NULL, &r);
		}
	}
	
	return false;
}
