#include "game_gui.h"
#include "keys.h"
#include <SDL2/SDL.h>

GameGui::GameGui(GameQ* q, Main* upper, GameConfig& cf, void* d) : State(upper){
	data = d;
	game = q;
	drawer = new GameDrawer(q, upper->get_renderer(), cf);
	for(int i = 0; i<cf.tank_num; i++){
		if(cf.keys[i]>=0) keys.push_back({i,cf.keys[i]});
	}
	upper->set_screen_size(GSCR_W, GSCR_H);
}
GameGui::~GameGui(){
	delete game;
	delete drawer;
}
bool GameGui::step(){
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		switch(e.type){
		case SDL_QUIT:
			game->leave();
			return true;
		case SDL_KEYDOWN:
			switch(e.key.keysym.sym){
			case SDLK_ESCAPE:
				game->leave();
				return true;
			}
			break;
		}
	}
	
	auto kstate = SDL_GetKeyboardState(NULL);
	for(int i = 0; i<keys.size(); i++){
		KeySet k = get_keyset(keys[i].second);
		ControlState ctrl;
		ctrl.fd = kstate[SDL_GetScancodeFromKey(k.fd)];
		ctrl.bk = kstate[SDL_GetScancodeFromKey(k.bk)];
		ctrl.rt = kstate[SDL_GetScancodeFromKey(k.rt)];
		ctrl.lt = kstate[SDL_GetScancodeFromKey(k.lt)];
		ctrl.sht = kstate[SDL_GetScancodeFromKey(k.sht)];
		
		game->get_tank(keys[i].first)->push_ctrl(ctrl);
	}
	
	game->advance();
	drawer->draw();
	return false;
}
