#ifndef _GAME_EXTRAP_H
#define _GAME_EXTRAP_H

#include "game.h"

#include "ex_in_events.h"

class GameExtrap;
class RoundExtrap;
class TankExtrap;
class TeamExtrap;

class GenShotExtrap;
class ShotExtrap;

class ExEvent{
public:
	enum Type{
		TYPE_START_RND,
		TYPE_TANK_DEAD,
		TYPE_SCORE,
		TYPE_END_GAME,
		TYPE_SHT_CRT, TYPE_SHT_RMV,
	};
	virtual Type get_type()=0;
};
class ExEventStartRnd : public ExEvent{
public:
	Type get_type(){ return TYPE_START_RND; }
};
class ExEventTankDeath : public ExEvent{
	int ind;
public:
	ExEventTankDeath(int i);
	Type get_type(){ return TYPE_TANK_DEAD; }
	int get_ind();
};
class ExEventScore : public ExEvent{
	int ind,diff;
public:
	ExEventScore(int i, int d);
	Type get_type(){ return TYPE_SCORE; }
	int get_ind();
	int get_diff();
};
class ExEventEndGame : public ExEvent{
	std::vector<int> scores;
public:
	ExEventEndGame(GameExtrap* game);
	Type get_type(){ return TYPE_END_GAME; }
	std::vector<int>& get_scores();
};
class ExEventCreateShot : public ExEvent{
	int id;
	TankExtrap* tank;
	GenShot::Type type;
	double x,y,ang;
public:
	ExEventCreateShot(GenShotExtrap* s);
	Type get_type(){ return TYPE_SHT_CRT; }
	int get_id();
	GenShot::Type get_stype();
	double get_x();
	double get_y();
	double get_ang();
	int get_tank_ind();
};
class ExEventRemoveShot : public ExEvent{
	int id;
	double x,y,vx,vy;
	GenShot::Type type;
public:
	ExEventRemoveShot(GenShotExtrap* s);
	Type get_type(){ return TYPE_SHT_CRT; }
	int get_id();
	double get_x();
	double get_y();
	double get_vx();
	double get_vy();
	GenShot::Type get_stype();
};




class TeamExtrap{
	int score;
	int ind;
public:
	TeamExtrap(int i);
	int get_score();
	void add_score(int diff);
	int get_ind();
};

class GameExtrap{
	ExInEvents* in;
	RoundExtrap* round;
	std::vector<TankExtrap*> tanks;
	std::vector<TeamExtrap*> teams;
	GameSettings set;
	
	std::queue<ExEvent*> events;
	
	int round_num;
	long long int time;
public:
	GameExtrap(GameConfig& cf, ExInEvents* i);
	~GameExtrap();
	RoundExtrap* get_round();
	int get_tank_num();
	TankExtrap* get_tank(int i);
	int get_team_num();
	TeamExtrap* get_team(int i);
	
	void step();
	ExEvent* get_event();
	
	void push_ctrl(ControlState ctrl, int ind);

private:
	void start_round(ExInEventStartRound* e);
	void kill_tank(int ind);
	void add_score(int ind, int diff);
};

class RoundExtrap{
	GameExtrap* game;
	Maze* maze;
	
	
	std::map<int,GenShotExtrap*> shots;
public:
	RoundExtrap(GameExtrap* game,Maze* maze);
	Maze* get_maze();
	
	std::map<int, GenShotExtrap*>::iterator get_shots();
	std::map<int, GenShotExtrap*>::iterator end_shots();
};

class TankExtrap{
	GameExtrap* game;
	TeamExtrap* team;
	int ind;
	
	long long int b_t,t;
	double x,y,ang;
	
	bool dead;
public:
	TankExtrap(GameExtrap* game,int ind, TeamExtrap* team);
	TeamExtrap* get_team();
	
	double get_x();
	double get_y();
	double get_ang();
	
	bool is_dead();
	void kill();
	
	void push_control(ControlState ctrl);
	
	void update(ExInEventTankUpdate* e);
	void reset(double x, double y, double ang);
	int get_ind();
};

class GenShotExtrap{
	GameExtrap* game;
	TankExtrap* tank;
	int id;
	GenShot::Type type;
protected:
	GameExtrap* get_game();
public:
	GenShotExtrap(GameExtrap* game, TankExtrap* tank, int id, GenShot::Type type);
	
	TankExtrap* get_tank();
	int get_id();
	GenShot::Type get_type();
	
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	virtual double get_ang() = 0;
};

class ShotExtrap : public GenShotExtrap{
	std::vector<std::pair<double, double>> colls;
public:
	ShotExtrap(GameExtrap* game, TankExtrap* tank, int id, GenShot::Type type);
	double get_r();
	std::vector<std::pair<double, double>>& get_colls();
};

#endif