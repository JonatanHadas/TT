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
01   | > | tank update | ind,t, x,y,ang, ctrl_state, state, missile: id, x,y,ang, rt,lt, tar
02   | > | tank death  | ind
03   | > | score       | team,diff
     |   |             |
10   | > | create shot | id, time,round, tank_ind,type, x,y,vx,vy
11   | > | delete shot | id
12   | > | create frag | id, time,round, x,y,ang
13   | > | create dtry | id, round, tank_ind, num, x0,y0,x1,y1,...
1f   | > | no-op shot  | ---
     |   |             |
20   | > | create upg  | round, x,y,type
21   | > | delete upg  | round, x,y
     |   |             |
30   | > | create mine | id, round, x,y,ang, tank_ind
31   | > | remove mine | id
32   | > | actvt mine  | id
33   | > | start mine  | id
     |   |             |


*/

class NetGame;

#include "../network/network.h"
#include "game_setup.h"
#include "../game/game.h"

class NetGame{
	GameSetup* set;
	Game* game;
	std::vector<int> ids;
	
	bool ended;
	
	void push_ctrl(int peer_id,char* data);
	
	void advance();
	void send_update();
	
	void start_round();
	void kill_tank(GameEventTankDeath* e);
	void score(GameEventScore* e);
	
	void create_shot(GameEventCreateShot* e);
	void remove_shot(GameEventRemoveShot* e);
	void create_mine(GameEventCreateMine* e);
	void remove_mine(GameEventRemoveMine* e);
	void activate_mine(GameEventActivateMine* e);
	void start_mine(GameEventStartMine* e);
	void create_upg(GameEventCreateUpgrade* e);
	void remove_upg(GameEventRemoveUpgrade* e);
	
	void end_game(GameEventEndGame* e);
public:
	NetGame(GameConfig& cf, GameSetup* set);
	~NetGame();
	
	void mainloop();
};

#endif