#ifndef _GAME_GUI_H
#define _GAME_GUI_H

#include "gui.h"
#include "game_draw.h"
#include "game_query.h"
#include "game_config.h"

class GameGui : public State{
	GameQ* game;
	GameDrawer* drawer;
	void* data;
	std::vector<std::pair<int,int>> keys;
public:
	GameGui(GameQ* q, Main* upper, GameConfig& cf, void* data);
	~GameGui();
	bool step();
};

#endif