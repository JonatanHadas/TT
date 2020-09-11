#ifndef _GUI_H
#define _GUI_H

#include <SDL2/SDL.h>
#include "clock.h"

class Main;
class State;

typedef State*(*state_init_f)(Main*);

class Main{
	SDL_Renderer* rend;
	State* state;
	State* for_del;
	
	Clock clk;
	
	int w,h;
	bool chng_sz;
	
public:
	Main(SDL_Renderer* rend, state_init_f init);
	~Main();
	void draw_back();
	void mainloop();
	void set_state(State* stt);
	SDL_Renderer* get_renderer();
	void set_screen_size(int w, int h);
};

class State{
protected:
	Main* upper;
public:
	State(Main* up);
	virtual bool step() = 0;
};

#endif