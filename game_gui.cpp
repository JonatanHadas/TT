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
	drawer->draw();
	return false;
}
