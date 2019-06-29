#ifndef _NETGAME_H
#define _NETGAME_H

/*
server<-->client

stage1
type |   | meaning  | data
-----+---+----------+--------
03   | > | end game | scores


stage2
type |   | meaning     | data
-----+---+-------------+--------
00   | > | start round | tank_poss, maze
01   | < | controls    | tank_ind, round, ctrl state
01   | > | tank update | ind,t, x,y,ang, ctrl_state
02   | > | tank death  | ind
03   | > | score       | team,diff
     |   |             |
10   | > | create shot | id, time,round, tank_ind,type, x,y,vx,vy
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
	
	void advance();
	void send_update();
	
	void start_round();
	void kill_tank(GameEventTankDeath* e);
	void score(GameEventScore* e);
	
	void create_shot(GameEventCreateShot* e);
	void remove_shot(GameEventRemoveShot* e);
	
	void end_game(GameEventEndGame* e);
public:
	NetGame(GameConfig& cf, GameSetup* set);
	~NetGame();
	
	void mainloop();
};

#endif