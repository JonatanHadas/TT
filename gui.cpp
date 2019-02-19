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
	int teams[30] = {0,0,0,1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,7,7,7,8,8,8,9,9,9};
	state = new GameGui(new GameDQ(new Game(12,4,teams)),this, img_inds);
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