#ifndef _CLOCK_H
#define _CLOCK_H

#include <SDL2/SDL.h>

class Clock{
	double last_tick;
public:
	Clock();
	double tick(double mx_fps);
};

#endif