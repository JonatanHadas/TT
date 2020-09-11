#ifndef _GAME_SETUP_H
#define _GAME_SETUP_H

class GameSetup;

#include "../game/game_config.h"
#include "netgame.h"
#include "../network/network.h"
#include "time.h"
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


serv <--> client
stage0:

type |   | meaning        | data
-----+---+----------------+--------
00   | > | game settings  | game settings
00   | < | rejoin         | --- // request all data (as if you just joined)
01   | < | add player     | -1/pref_col
01   | > | add player     | id, team, color
02   | < | remove player  | index
02   | > | remove player  | id
03   | < | change name    | player_index,name
03   | > | change name    | id,name
04   | < | change color   | index, color
04   | > | change color   | id, color
05   | > | change color   | ind, color
06   | > | assign host    |
07   | > | host tank      | id
08   | < | change team    | id, team
08   | > | change team    | id, team
09   | > | tank yours     | id, ind
     |   |                |
10   | <H| change scr mth | new value
10   | > | change scr mth | new value
11   | <H| change end mth | new value
11   | > | change end mth | new value
12   | <H| change tie lim | new value
12   | > | change tie lim | new value
13   | <H| change limit   | new value
13   | > | change limit   | new value
14   | <H| team number    | num
14   | > | team number    | num
15   | <H| use teams      | true/false
15   | > | use teams      | true/false
16   | <H| change upgrade | mask, new value
16   | > | change upgrades| mask

stage1:
type |   | meaning    | data
-----+---+------------+--------
00   | <H| start      | ---
00   | > | count down | int num
01   | < | stop count | ---
02   | > | start game | ---


*/

class GameSetup{
	
	GameSettings set;
	bool use_teams;
	int team_num;
	std::map<int, std::vector<PlayerData>*> peers; //peer_id -> vector<players>
	std::map<int, std::pair<int,int>> players; //id -> (peer id, index)
	Server* serv;
	
	int upg_mask;
	
	int id;
	int cnt;
	
	clock_t last_tick;
	
	bool color_taken(int colnum);
	
	int get_free_col();
	
	void send_all(int peer_id); // send all data to this peer
	
	void add_player(int peer_id, int pref_col); //add player for this peer
	void remove_player(int peer_id, int ind);
	void update_name(int peer_id, int ind, const char* name);
	void update_col(int peer_id, int ind, int col);
	void assign_host();
	void change_team(int peer_id, int id, int team);
	
	void update_game_lim();
	void update_tie_lim();
	void update_team_num();
	void update_use_teams();
	void update_scr_mth();
	void update_end_mth();
	void update_upg_mask(int mask, bool val);
	
	void start_count();
	void count();
	void stop_count();
	friend NetGame;
	
	void reset();
public:
	GameSetup(Server* s);
	~GameSetup();
	
	void mainloop();
};

#endif