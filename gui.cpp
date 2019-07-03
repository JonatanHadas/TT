#include "gui.h"

#define FPS 60

#include "game_gui.h"
#include "d_game_query.h"

Main::Main(SDL_Renderer* renderer){
	rend = renderer;
	for_del = NULL;
	chng_sz = false;
	std::vector<int> img_inds;
	for(int i = 0; i<30; i++) img_inds.push_back(i);
	
	GameConfig cf(12,4, UPG_MASK_MINE);
	for(int i = 0; i<cf.tank_num; i++) cf.team_inds[i] = i/3;
	cf.set.scr_mth = GameSettings::SCR_LAST;
	cf.set.end_mth = GameSettings::END_NONE;
	cf.set.lim = 10;
	cf.set.allow_dif = 0;
	
	state = new GameGui(new GameDQ(new Game(cf)),this, img_inds);
}
Main::~Main(){
	if(state) delete state;
	if(for_del) delete for_del;
}
void Main::draw_back(){
	SDL_SetRenderDrawColor(rend, 255,255,255,255);
	SDL_RenderClear(rend);
}
void Main::mainloop(){
	while(true){
		clk.tick(FPS);
		
		if(state->step()) return;
		if(for_del) delete for_del;
		for_del = NULL;
		
		SDL_RenderPresent(rend);
		
		if(chng_sz) SDL_RenderSetLogicalSize(rend, w,h);
		chng_sz = false;
	}
}
void Main::set_state(State* stt){
	if(state!=stt) for_del = state;
	state = stt;
}
SDL_Renderer* Main::get_renderer(){
	return rend;
}

void Main::set_screen_size(int ww, int hh){
	w=ww;h=hh;chng_sz = true;
}

State::State(Main* up){
	upper = up;
}