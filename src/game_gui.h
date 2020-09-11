#ifndef _GAME_GUI_H
#define _GAME_GUI_H

#include "gui_utils/gui.h"
#include "game_gui/game_draw.h"
#include "game/game_query.h"

class GameGui : public State{
	GameQ* game;
	GameDrawer* drawer;
	std::vector<int> img_is;
public:
	GameGui(GameQ* q, Main* upper, std::vector<int> img_inds);
	~GameGui();
	bool step();
	void end();
};

#endif