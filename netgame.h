#ifndef _NETGAME_H
#define _NETGAME_H

/*
server<-->client

stage1
type |   | meaning  | data
-----+---+----------+--------
03   | > | end game | ---


stage2
type |   | meaning     | data
-----+---+-------------+--------
00   | > | start round | tank_poss, maze
01   | < | controls    | tank_ind, round, ctrl state
01   | > | tank update | 
02   | > | tank death  | ind
03   | > | score       | team,diff
     |   |             |
10   | > | create shot | 
11   | > | delete shot | id


*/

class NetGame;

#include "network.h"
#include "game_setup.h"
#include "game.h"

class NetGame{
	GameSetup* set;
	Game* game;
	std::vector<int> ids;
	
	void push_ctrl(int peer_id,char* data);
public:
	NetGame(GameConfig& cf, GameSetup* set);
	~NetGame();
	
	void mainloop();
};

#endif