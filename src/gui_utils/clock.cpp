#include "clock.h"
#include "cmath"

#define ALLOW_ERR 0.1

Clock::Clock(){
	last_tick = SDL_GetTicks();
}

double Clock::tick(double fps){
	double dt = 1000.0/fps;
	int time = SDL_GetTicks();
	double new_tick = last_tick + dt;
	
	int tk = (int)(new_tick - time);
	if(tk>1) SDL_Delay(tk);
	time = SDL_GetTicks();
	if(std::abs(time-new_tick) < ALLOW_ERR * dt) new_tick = time;
	dt = time - last_tick;
	last_tick = new_tick;
	return 1000.0/dt;
}