#ifndef _DIRECT_EX
#define _DIRECT_EX

#include "ex_in_events.h"
#include "game.h"

class DirectEx : public ExInEvents{
	Game* game;
	int t_ind;
public:
	DirectEx(Game* game);
	ExInEvent* get_event();
	void push_ctrl(ControlState ctrl, int ind, int rnd);
};

class DExInEventStartRound : public ExInEventStartRound{
	GameEventStartRnd* e;
	Game* g;
public:
	DExInEventStartRound(GameEventStartRnd* event, Game* game);
	~DExInEventStartRound();
	Maze* get_maze();
	int get_x(int i); // tank positions
	int get_y(int i);
	double get_ang(int i);
};
class DExInEventTankDeath : public ExInEventTankDeath{
	GameEventTankDeath* e;
public:
	DExInEventTankDeath(GameEventTankDeath* event);
	~DExInEventTankDeath();
	int get_ind();
};
class DExInEventScore : public ExInEventScore{
	GameEventScore* e;
public:
	DExInEventScore(GameEventScore* event);
	~DExInEventScore();
	int get_ind();
	int get_diff();
};
class DExInEventEndGame : public ExInEventEndGame{
	GameEventEndGame* e;
public:
	DExInEventEndGame(GameEventEndGame* event);
	~DExInEventEndGame();
};
class DExInEventCreateShot : public ExInEventCreateShot{
	GameEventCreateShot* e;
public:
	DExInEventCreateShot(GameEventCreateShot* event);
	~DExInEventCreateShot();
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
class DExInEventRemoveShot : public ExInEventRemoveShot{
	GameEventRemoveShot* e;
public:
	DExInEventRemoveShot(GameEventRemoveShot* event);
	~DExInEventRemoveShot();
	int get_id();
};
class DExInEventTankUpdate : public ExInEventTankUpdate{
	Tank* t;
	Game* g;
public:
	DExInEventTankUpdate(Tank* tank, Game* game);
	~DExInEventTankUpdate();
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
class DExInEventCreateUpgrade : public ExInEventCreateUpgrade{
	GameEventCreateUpgrade* e;
public:
	DExInEventCreateUpgrade(GameEventCreateUpgrade* event);
	~DExInEventCreateUpgrade();
	Upgrade get_upg();
	int get_round();
};
class DExInEventRemoveUpgrade : public ExInEventRemoveUpgrade{
	GameEventRemoveUpgrade* e;
public:
	DExInEventRemoveUpgrade(GameEventRemoveUpgrade* event);
	~DExInEventRemoveUpgrade();
	int get_x();
	int get_y();
	int get_round();
};
class DExInEventCreateFragment : public ExInEventCreateFragment{
	GameEventCreateShot* e;
public:
	DExInEventCreateFragment(GameEventCreateShot* event);
	~DExInEventCreateFragment();
	double get_x();
	double get_y();
	double get_ang();
	long long int get_time();
	int get_round();
	int get_id();
};
class DExInEventCreateDeathRay : public ExInEventCreateDeathRay{
	GameEventCreateShot* e;
public:
	DExInEventCreateDeathRay(GameEventCreateShot* event);
	~DExInEventCreateDeathRay();
	int get_point_num();
	std::pair<double, double> get_point(int i);
	int get_round();
	int get_tank_ind();
	int get_id();
};
class DExInEventCreateMine : public ExInEventCreateMine{
	GameEventCreateMine* e;
public:
	DExInEventCreateMine(GameEventCreateMine* event);
	~DExInEventCreateMine();
	int get_id();
	double get_x();
	double get_y();
	double get_ang();
	int get_round();
	int get_tank_ind();
};
class DExInEventRemoveMine : public ExInEventRemoveMine{
	GameEventRemoveMine* e;
public:
	DExInEventRemoveMine(GameEventRemoveMine* event);
	~DExInEventRemoveMine();
	int get_id();
};
class DExInEventStartMine : public ExInEventStartMine{
	GameEventStartMine* e;
public:
	DExInEventStartMine(GameEventStartMine* event);
	~DExInEventStartMine();
	int get_id();
};
class DExInEventActivateMine : public ExInEventActivateMine{
	GameEventActivateMine* e;
public:
	DExInEventActivateMine(GameEventActivateMine* event);
	~DExInEventActivateMine();
	int get_id();
};
#endif