#include "game_gui.h"
#include <SDL2/SDL.h>

GameGui::GameGui(GameQ* q, Main* upper, std::vector<int> img_inds) : State(upper){
	game = q;
	drawer = new GameDrawer(q, upper->get_renderer(), img_inds);
	upper->set_screen_size(GSCR_W, GSCR_H);
}
GameGui::~GameGui(){
	delete drawer;
}
bool GameGui::step(){
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		switch(e.type){
		case SDL_QUIT:
			return true;
		}
	}
	
	std::vector<ControlState> ctrls;
	for(int i = 0; i<game->get_tank_num(); i++){
		ctrls.push_back({false,false,false,false,false});
	}
	const unsigned char* kstate = SDL_GetKeyboardState(NULL);
	// TEMP
	ctrls[0].fd = kstate[SDL_SCANCODE_UP];
	ctrls[0].bk = kstate[SDL_SCANCODE_DOWN];
	ctrls[0].rt = kstate[SDL_SCANCODE_LEFT];
	ctrls[0].lt = kstate[SDL_SCANCODE_RIGHT];
	ctrls[0].sht = kstate[SDL_SCANCODE_SPACE];
	ctrls[1].fd = kstate[SDL_SCANCODE_W];
	ctrls[1].bk = kstate[SDL_SCANCODE_S];
	ctrls[1].rt = kstate[SDL_SCANCODE_A];
	ctrls[1].lt = kstate[SDL_SCANCODE_D];
	ctrls[1].sht = kstate[SDL_SCANCODE_TAB];
	
	for(int i = 0; i<game->get_tank_num(); i++){
		game->get_tank(i)->push_ctrl(ctrls[i]);
	}
	
	game->advance();
	drawer->draw();
	return false;
}
