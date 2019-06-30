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
		TYPE_UPG_CRT, TYPE_UPG_RMV,
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
class ExEventCreateUpgrade : public ExEvent{
	Upgrade u;
public:
	ExEventCreateUpgrade(Upgrade u);
	Type get_type(){ return TYPE_UPG_CRT; }
	Upgrade get_upg();
};
class ExEventRemoveUpgrade : public ExEvent{
	int x,y;
public:
	ExEventRemoveUpgrade(int x, int y);
	Type get_type(){ return TYPE_UPG_RMV; }
	int get_x();
	int get_y();
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
	
	long long int get_time();

private:
	void start_round(ExInEventStartRound* e);
	void kill_tank(int ind);
	void add_score(int ind, int diff);
	
	void create_shot(ExInEventCreateShot* e);
	void remove_shot(int id);
	void create_upgrade(ExInEventCreateUpgrade* e);
	void remove_upgrade(ExInEventRemoveUpgrade* e);
};

class RoundExtrap{
	GameExtrap* game;
	Maze* maze;
	
	
	std::map<int,GenShotExtrap*> shots;
	std::map<std::pair<int,int>,Upgrade::Type> upgs;
public:
	RoundExtrap(GameExtrap* game,Maze* maze);
	Maze* get_maze();
	
	void advance();
	
	std::map<int, GenShotExtrap*>::iterator get_shots();
	std::map<int, GenShotExtrap*>::iterator end_shots();
	std::map<std::pair<int,int>,Upgrade::Type>::iterator get_upgs();
	std::map<std::pair<int,int>,Upgrade::Type>::iterator end_upgs();
	
	GenShotExtrap* get_shot(int id);

	void add_shot(GenShotExtrap* shot);
	void del_shot(int id);
	void add_upg(Upgrade u);
	void del_upg(int x, int y);
	bool has_upg(int x, int y);
};

class TankExtrap{
	GameExtrap* game;
	TeamExtrap* team;
	int ind;
	
	long long int b_t,t;
	double x,y,ang;
	Tank::State state;
	
	ControlState p_ctrl;
	std::deque<ControlState> ctrl;
	
	bool dead;
	bool check_wall_coll(double& nx, double& ny, double& px, double& py, double& dp);
public:
	TankExtrap(GameExtrap* game,int ind, TeamExtrap* team);
	TeamExtrap* get_team();
	
	double get_x();
	double get_y();
	double get_ang();
	
	bool is_dead();
	void kill();
	
	Tank::State get_state();
	
	void push_control(ControlState ctrl);
	
	void update(ExInEventTankUpdate* e);
	void reset(double x, double y, double ang);
	
	void advance();
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
	virtual void advance() = 0;
};

class ShotExtrap : public GenShotExtrap{
	std::vector<std::pair<double, double>> colls;
	double x,y,vx,vy,nx,ny;
	long long int ctime; double col_t;
	bool found;
	double check_wall();
	void reflect();
public:
	ShotExtrap(GameExtrap* game, ExInEventCreateShot* e);
	double get_r();
	double get_x();
	double get_y();
	double get_ang();
	std::vector<std::pair<double, double>>& get_colls();
	
	void advance();
};

#endif