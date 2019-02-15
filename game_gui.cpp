#include "game_gui.h"
#include <SDL2/SDL.h>

GameGui::GameGui(GameQ* q, Main* upper) : State(upper){
	game = q;
	drawer = new GameDrawer(q, upper->get_renderer());
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
