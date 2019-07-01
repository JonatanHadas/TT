#ifndef _D_GAME_QUERY_H
#define _D_GAME_QUERY_H

#include "game_query.h"
#include "game.h"

class TankDQ;
class RoundDQ;
class GameDQ;
class TeamDQ;

class GameDQEventStartRnd;
class GameDQEventTankDeath;
class GameDQEventScore;
class GameDQEventEndGame;
class GameDQEventCreateShot;
class GameDQEventRemoveShot;
class GameDQEventCreateUpgrade;
class GameDQEventRemoveUpgrade;

class GameDQEventStartRnd : public GameQEventStartRnd{
	GameEventStartRnd* e;
public:
	GameDQEventStartRnd(GameEventStartRnd* event);
	~GameDQEventStartRnd();
};
class GameDQEventTankDeath : public GameQEventTankDeath{
	GameEventTankDeath* e;
public:
	GameDQEventTankDeath(GameEventTankDeath* event);
	~GameDQEventTankDeath();
	int get_ind();
};
class GameDQEventScore : public GameQEventScore{
	GameEventScore* e;
public:
	GameDQEventScore(GameEventScore* event);
	~GameDQEventScore();
	int get_ind();
	int get_diff();
};
class GameDQEventEndGame : public GameQEventEndGame{
	GameEventEndGame* e;
public:
	GameDQEventEndGame(GameEventEndGame* event);
	~GameDQEventEndGame();
	std::vector<int>& get_scores();
};
class GameDQEventCreateShot : public GameQEventCreateShot{
	GameEventCreateShot* e;
public:
	GameDQEventCreateShot(GameEventCreateShot* event);
	~GameDQEventCreateShot();
	int get_id();
	GenShot::Type get_stype();
	double get_x();
	double get_y();
	double get_ang();
	int get_tank_ind();
};
class GameDQEventRemoveShot : public GameQEventRemoveShot{
	GameEventRemoveShot* e;
	double x,y,vx,vy;
public:
	GameDQEventRemoveShot(GameEventRemoveShot* event);
	~GameDQEventRemoveShot();
	int get_id();
	double get_x();
	double get_y();
	double get_vx();
	double get_vy();
	GenShot::Type get_stype();
};

class GameDQEventCreateUpgrade : public GameQEventCreateUpgrade{
	GameEventCreateUpgrade* e;
public:
	GameDQEventCreateUpgrade(GameEventCreateUpgrade* event);
	~GameDQEventCreateUpgrade();
	Upgrade get_upg();
};
class GameDQEventRemoveUpgrade : public GameQEventRemoveUpgrade{
	GameEventRemoveUpgrade* e;
public:
	GameDQEventRemoveUpgrade(GameEventRemoveUpgrade* event);
	~GameDQEventRemoveUpgrade();
	int get_x();
	int get_y();
};

class GameDQ : public GameQ{
	Game* game;
	RoundDQ* round;
	std::vector<TankDQ*> tanks;
	std::vector<TeamDQ*> teams;
public:
	GameDQ(Game* game);
	~GameDQ();
	int get_tank_num();
	TankQ* get_tank(int i);
	int get_team_num();
	TeamQ* get_team(int i);
	RoundQ* get_round();
	GameQEvent* get_event();
	
	void advance();

	long long int get_time();
};
class TeamDQ : public TeamQ{
	Team* team;
	std::vector<TankDQ*> tanks;
	friend TankDQ;
public:
	TeamDQ(Team* team);
	int get_ind();
	int get_score();
	int get_tank_num();
	TankQ* get_tank(int i);
};
class TankDQ : public TankQ{
	Tank* tank;
	TeamDQ* team;
public:
	TankDQ(Tank* tank, GameDQ* game);
	double get_x();
	double get_y();
	double get_ang();
	bool is_dead();
	
	int get_ind();
	TeamQ* get_team();
	
	void push_ctrl(ControlState ctrl);
	
	Tank::State get_state();
};
class RoundDQ : public RoundQ{
	Round* round;
public:
	RoundDQ(Round* round);
	Maze* get_maze();
	std::set<GenShotQ*> get_shots();
	std::set<Upgrade*> get_upgs();
};

class ShotDQ : public ShotQ{
	Shot* shot;
public:
	ShotDQ(Shot* shot);
	double get_r();
	std::vector<std::pair<double,double>>& get_colls();
	double get_x();
	double get_y();
	double get_ang();
	GenShot::Type get_type();
};

class FragmentDQ : public FragmentQ{
public:
	Fragment* frag;
	FragmentDQ(Fragment* f);
	double get_x();
	double get_y();
	double get_ang();
	double get_dst();
	double get_t();
	GenShot::Type get_type();
};
#endif