#ifndef _GAME_SETUP_H
#define _GAME_SETUP_H

#include "game_config.h"
#include "network.h"
#include <map>
#include <vector>

struct PlayerData{
	int color;
	int team;
	int peer_id;
	int id;
	std::string name;
};

/*
--- protocol ---

  1B   |  1B  | ...
<stage>|<type>|<data>

stages:
0=setup			(change names,colors game settings etc.)
1=game_manage	(start/end game)
2=game			(round start, tank positions etc.)


stage0:
serv <--> client

type |   | meaning        | data
-----+---+----------------+--------
0    | > | game settings  | game settings
1    | < | add player     | ---
1    | > | add player     | id, team, color
2    | < | remove player  | index
2    | > | remove player  | id
3    | < | change name    | player_index,name
3    | > | change name    | id,name
4    | < | change color   | index, color
4    | > | change color   | id, color
5    | > | change color   | ind, color


*/

class GameSetup{
	GameSettings set;
	int team_num;
	std::map<int, std::vector<PlayerData>*> peers; //peer_id -> vector<players>
	std::map<int, std::pair<int,int>> players; //id -> (peer id, index)
	Server* serv;
	
	int id;
	
	bool color_taken(int colnum);
	
	int get_free_col();
	
	void send_all(int peer_id); // send all data to this peer
	
	void add_player(int peer_id); //add player for this peer
	void remove_player(int peer_id, int ind);
	void update_name(int peer_id, int ind, const char* name);
	void update_col(int peer_id, int ind, int col);
public:
	GameSetup(Server* s);
	~GameSetup();
	
	void mainloop();
};

#endif