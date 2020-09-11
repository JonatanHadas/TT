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
class GameEQEventCreateUpgrade;
class GameEQEventRemoveUpgrade;

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
class GameEQEventCreateUpgrade : public GameQEventCreateUpgrade{
	ExEventCreateUpgrade* e;
public:
	GameEQEventCreateUpgrade(ExEventCreateUpgrade* event);
	~GameEQEventCreateUpgrade();
	Upgrade get_upg();
};
class GameEQEventRemoveUpgrade : public GameQEventRemoveUpgrade{
	ExEventRemoveUpgrade* e;
public:
	GameEQEventRemoveUpgrade(ExEventRemoveUpgrade* event);
	~GameEQEventRemoveUpgrade();
	int get_x();
	int get_y();
};
class GameEQEventCreateMine : public GameQEventCreateMine{
	ExEventCreateMine* e;
public:
	GameEQEventCreateMine(ExEventCreateMine* event);
	~GameEQEventCreateMine();
	double get_x();
	double get_y();
	double get_ang();
	int get_ind();
};
class GameEQEventTankStuck : public GameQEventTankStuck{
	ExEventTankStuck* e;
public:
	GameEQEventTankStuck(ExEventTankStuck* event);
	~GameEQEventTankStuck();
	int get_ind();
	double get_spd();
};

class GameEQ : public GameQ{
	GameExtrap* game;
	RoundEQ* round;
	std::vector<TankEQ*> tanks;
	std::vector<TeamEQ*> teams;
	friend TankEQ;
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
	
	void leave();
	long long int get_time();
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
	GameExtrap* game;
public:
	TankEQ(TankExtrap* tank, GameEQ* game);
	double get_x();
	double get_y();
	double get_ang();
	bool is_dead();
	Tank::State get_state();
	
	int get_ind();
	TeamQ* get_team();
	
	void push_ctrl(ControlState ctrl);
	
	std::vector<std::pair<double, double>> predict_colls(double len, double r);
};
class RoundEQ : public RoundQ{
	RoundExtrap* round;
public:
	RoundEQ(RoundExtrap* round);
	Maze* get_maze();
	std::set<GenShotQ*> get_shots();
	std::set<MineQ*> get_mines();
	std::set<Upgrade*> get_upgs();
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
	int get_tank_ind();
};

class FragmentEQ : public FragmentQ{
	FragmentExtrap* frag;
public:
	FragmentEQ(FragmentExtrap* f);
	double get_x();
	double get_y();
	double get_ang();
	double get_dst();
	double get_t();
	GenShot::Type get_type();
	int get_tank_ind();
};

class DeathRayEQ : public DeathRayQ{
	DeathRayExtrap* dr;
public:
	DeathRayEQ(DeathRayExtrap* d);
	int get_point_num();
	double get_x(int i);
	double get_y(int i);
	int get_coll_num();
	double get_coll_x(int i);
	double get_coll_y(int i);
	double get_coll_nx(int i);
	double get_coll_ny(int i);
	double get_x();
	double get_y();
	double get_ang();
	GenShot::Type get_type();
	int get_tank_ind();
};

class MissileEQ : public MissileQ{
	MissileExtrap* mis;
public:
	MissileEQ(MissileExtrap* m);
	int get_tank_ind();
	int get_tar_ind();
	double get_x();
	double get_y();
	double get_ang();
	
	GenShot::Type get_type();
	
	int get_time();
};

class MineEQ : public MineQ{
	MineExtrap* mine;
public:
	MineEQ(MineExtrap* m);
	double get_x();
	double get_y();
	double get_ang();
	bool get_started();
	bool get_active();
	int get_tank_ind();
};

#endif