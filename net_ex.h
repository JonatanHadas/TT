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
	
	double m_x,m_y,m_a;
	bool m_rt, m_lt;
	int m_id, m_tar;
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

	double get_missile_x();
	double get_missile_y();
	double get_missile_ang();
	bool get_missile_rt();
	bool get_missile_lt();
	int get_missile_id();
	int get_missile_target();

};
class NExInEventCreateUpgrade : public ExInEventCreateUpgrade{
	Upgrade u;
	int rnd;
public:
	NExInEventCreateUpgrade(char* data, char* del);
	~NExInEventCreateUpgrade();
	Upgrade get_upg();
	int get_round();
};
class NExInEventRemoveUpgrade : public ExInEventRemoveUpgrade{
	int x,y,rnd;
public:
	NExInEventRemoveUpgrade(char* data, char* del);
	~NExInEventRemoveUpgrade();
	int get_x();
	int get_y();
	int get_round();
};
class NExInEventCreateFragment : public ExInEventCreateFragment{
	double x,y,ang;
	long long int time;
	int round;
	int id;
public:
	NExInEventCreateFragment(char* data, char* del);
	~NExInEventCreateFragment();
	double get_x();
	double get_y();
	double get_ang();
	long long int get_time();
	int get_round();
	int get_id();
};
class NExInEventCreateDeathRay : public ExInEventCreateDeathRay{
	std::vector<std::pair<double, double>> ps;
	int tank_ind;
	int id;
	int round;
public:
	NExInEventCreateDeathRay(char* data, char* del);
	~NExInEventCreateDeathRay();
	int get_point_num();
	std::pair<double, double> get_point(int i);
	int get_round();
	int get_tank_ind();
	int get_id();
};
class NExInEventCreateMine : public ExInEventCreateMine{
	int id, round, tank_ind;
	double x,y,ang;
public:
	NExInEventCreateMine(char* data, char* del);
	~NExInEventCreateMine();
	int get_id();
	double get_x();
	double get_y();
	double get_ang();
	int get_round();
	int get_tank_ind();
};
class NExInEventRemoveMine : public ExInEventRemoveMine{
	int id;
public:
	NExInEventRemoveMine(char* data, char* del);
	~NExInEventRemoveMine();
	int get_id();
};
class NExInEventStartMine : public ExInEventStartMine{
	int id;
public:
	NExInEventStartMine(char* data, char* del);
	~NExInEventStartMine();
	int get_id();
};
class NExInEventActivateMine : public ExInEventActivateMine{
	int id;
public:
	NExInEventActivateMine(char* data, char* del);
	~NExInEventActivateMine();
	int get_id();
};

#endif