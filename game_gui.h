#ifndef _GAME_GUI_H
#define _GAME_GUI_H

#include "gui.h"
#include "game_draw.h"
#include "game_query.h"
#include "game_config.h"

#include "utils.h"

class GameGui : public State{
	GameQ* game;
	GameDrawer* drawer;
	Data* data;
	std::vector<int> img_is;
	std::vector<std::pair<int,int>> keys;
public:
	GameGui(GameQ* q, Main* upper, GameConfig& cf, Data* data);
	~GameGui();
	bool step();
	void end();
};

#endif