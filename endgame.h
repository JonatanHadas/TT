#ifndef _ENDGAME_H
#define _ENDGAME_H

#include "game_query.h"

#include <vector>
#include <string>
#include "gui.h"
#include "texts.h"
#include "images.h"

class EndGame : public State{
	int timer;
	std::vector<int> inds;
	std::vector<int> scores;
	std::vector<Msg*> scores_m;
	std::vector<int> teams;
	std::vector<std::string> names;
	std::vector<Msg*> names_m;
	TankImg* tank_images;
	Msg* title;
public:
	EndGame(Main* u, GameQ* query, std::vector<int> img_inds, std::vector<std::string> names);
	~EndGame();
	bool step();
};

#endif