#ifndef _E_GAME_QUERY
#define _E_GAME_QUERY

#include "game_query.h"
#include "game_extrap.h"

class TankEQ;
class RoundEQ;
class GameEQ;
class TeamEQ;

class GameEQEventStartRnd;
class GameEQEventTankDeath;
class GameEQEventScore;
class GameEQEventEndGame;
class GameEQEventCreateShot;
class GameEQEventRemoveShot;

class GameEQEventStartRnd : public GameQEventStartRnd{
	ExEventStartRnd* e;
public:
	GameEQEventStartRnd(ExEventStartRnd* event);
	~GameEQEventStartRnd();
};
class GameEQEventTankDeath : public GameQEventTankDeath{
	ExEventTankDeath* e;
public:
	GameEQEventTankDeath(ExEventTankDeath* event);
	~GameEQEventTankDeath();
	int get_ind();
};
class GameEQEventScore : public GameQEventScore{
	ExEventScore* e;
public:
	GameEQEventScore(ExEventScore* event);
	~GameEQEventScore();
	int get_ind();
	int get_diff();
};
class GameEQEventEndGame : public GameQEventEndGame{
	ExEventEndGame* e;
public:
	GameEQEventEndGame(ExEventEndGame* event);
	~GameEQEventEndGame();
	std::vector<int>& get_scores();
};
class GameEQEventCreateShot : public GameQEventCreateShot{
	ExEventCreateShot* e;
public:
	GameEQEventCreateShot(ExEventCreateShot* event);
	~GameEQEventCreateShot();
	int get_id();
	GenShot::Type get_stype();
	double get_x();
	double get_y();
	double get_ang();
	int get_tank_ind();
};
class GameEQEventRemoveShot : public GameQEventRemoveShot{
	ExEventRemoveShot* e;
public:
	GameEQEventRemoveShot(ExEventRemoveShot* event);
	~GameEQEventRemoveShot();
	int get_id();
	double get_x();
	double get_y();
	double get_vx();
	double get_vy();
	GenShot::Type get_stype();
};


class GameEQ : public GameQ{
	GameExtrap* game;
	RoundEQ* round;
	std::vector<TankEQ*> tanks;
	std::vector<TeamEQ*> teams;
public:
	GameEQ(GameExtrap* game);
	~GameEQ();
	int get_tank_num();
	TankQ* get_tank(int i);
	int get_team_num();
	TeamQ* get_team(int i);
	RoundQ* get_round();
	GameQEvent* get_event();
	
	void advance();
};
class TeamEQ : public TeamQ{
	TeamExtrap* team;
	std::vector<TankEQ*> tanks;
	friend TankEQ;
public:
	TeamEQ(TeamExtrap* team);
	int get_ind();
	int get_score();
	int get_tank_num();
	TankQ* get_tank(int i);
};
class TankEQ : public TankQ{
	TankExtrap* tank;
	TeamEQ* team;
public:
	TankEQ(TankExtrap* tank, GameEQ* game);
	double get_x();
	double get_y();
	double get_ang();
	bool is_dead();
	
	int get_ind();
	TeamQ* get_team();
	
	void push_ctrl(ControlState ctrl);
};
class RoundEQ : public RoundQ{
	RoundExtrap* round;
public:
	RoundEQ(RoundExtrap* round);
	Maze* get_maze();
	std::set<GenShotQ*> get_shots();
};

class ShotEQ : public ShotQ{
	ShotExtrap* shot;
public:
	ShotEQ(ShotExtrap* shot);
	double get_r();
	std::vector<std::pair<double,double>>& get_colls();
	double get_x();
	double get_y();
	double get_ang();
	GenShot::Type get_type();
};

#endif