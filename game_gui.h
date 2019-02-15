#ifndef _GAME_GUI_H
#define _GAME_GUI_H

#include "gui.h"
#include "game_draw.h"
#include "game_query.h"

class GameGui : public State{
	GameQ* game;
	GameDrawer* drawer;
public:
	GameGui(GameQ* q, Main* upper);
	~GameGui();
	bool step();
};

#endif