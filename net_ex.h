#ifndef _NET_EX
#define _NET_EX

#include "ex_in_events.h"
#include "network.h"

#include "game.h"
#include "game_config.h"


class NetEx : public ExInEvents{
	Client* clnt;
	int t_num;
public:
	NetEx(Client* c, GameConfig& cf);
	~NetEx();
	ExInEvent* get_event();
	void push_ctrl(ControlState ctrl, int ind, int rnd);
	void leave();
};

class NExInEventStartRound : public ExInEventStartRound{
	std::vector<std::pair<std::pair<int,int>,double>> tanks;
	Maze* maze;
public:
	NExInEventStartRound(char* data, char* del);
	~NExInEventStartRound();
	Maze* get_maze();
	int get_x(int i); // tank positions
	int get_y(int i);
	double get_ang(int i);
};
class NExInEventTankDeath : public ExInEventTankDeath{
	int ind;
public:
	NExInEventTankDeath(char* data, char* del);
	~NExInEventTankDeath();
	int get_ind();
};
class NExInEventScore : public ExInEventScore{
	int ind,diff;
public:
	NExInEventScore(char* data, char* del);
	~NExInEventScore();
	int get_ind();
	int get_diff();
};
class NExInEventEndGame : public ExInEventEndGame{
public:
	NExInEventEndGame(char* data, char* del);
	~NExInEventEndGame();
};
class NExInEventCreateShot : public ExInEventCreateShot{
	int id;
	GenShot::Type type;
	int tank_ind;
	long long int time;
	int	round;
	double x,y,vx,vy;
public:
	NExInEventCreateShot(char* data, char* del);
	~NExInEventCreateShot();
	int get_id();
	GenShot::Type get_stype();
	double get_x();
	double get_y();
	double get_vx();
	double get_vy();
	int get_tank_ind();
	long long int get_time();
	int get_round();
};
class NExInEventRemoveShot : public ExInEventRemoveShot{
	int id;
public:
	NExInEventRemoveShot(char* data, char* del);
	~NExInEventRemoveShot();
	int get_id();
};
class NExInEventTankUpdate : public ExInEventTankUpdate{
	double x;
	double y;
	double ang;
	long long int time;
	int ind;
	ControlState ctrl;
	Tank::State state;
public:
	NExInEventTankUpdate(char* data, char* del);
	~NExInEventTankUpdate();
	int get_ind();
	double get_x();
	double get_y();
	double get_ang();
	long long int get_time();
	ControlState get_ctrl();
	Tank::State get_state();
};

#endif