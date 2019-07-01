#ifndef _GAME_H
#define _GAME_H

#include <vector>
#include <map>
#include <set>
#include <queue>

#include "maze.h"
#include "game_config.h"
#include "game_consts.h"

// a whole game
class Game;
// a single tank
class Tank;
// all round specific data
class Round;

//Shots
class GenShot;
class Shot;
class RegShot;
class GatShot;
class LaserShot;
class BombShot;

class Controlable;

//for scoring
class Team;

class GameEvent;
class GameEventStartRnd;
class GameEventTankDeath;
class GameEventScore;
class GameEventEndGame;
class GameEventCreateShot;
class GameEventRemoveShot;

class Team{
	int score;
	int num_alive;
	int num_tot;
	
	int ind;
	
	friend Tank;
public:
	Team(int i);
	void reset();
	void add_score(int diff);
	int get_alive();
	int get_score();
	int get_ind();
};

class Game{
	std::vector<Tank*> tanks;
	std::vector<Team*> teams;
	std::vector<Upgrade::Type> upgs;
	Round* round;
	std::queue<GameEvent*> events;
	friend Round;
	
	long long int time;
	int end_timer;
	
	bool can_step();
	void step();
	GameSettings set;
	int round_num;
	
	int cid;
public:
	Game(GameConfig& cf);
	~Game();
	void start_round();
	void end_game();
	Round* get_round();
	Tank* get_tank(int i);
	int get_tank_num();
	Team* get_team(int i);
	int get_team_num();
	GameEvent* get_event();
	
	long long int get_time();
	void advance();
	
	void kill_tank(int i);
	void add_score(int ind, int diff);
	
	int get_new_id();

	int get_round_num();
};

class Round{
	Game* game;
	Maze* maze;
	
	std::set<GenShot*> shots;
	std::set<GenShot*> shts_fd;
	
	std::map<std::pair<int,int>, Upgrade::Type> upgs;
	
	int upg_timer;
	
	void create_upgrade();
public:
	Round(Game* game);
	~Round();
	void step();
	Maze* get_maze();
	
	void add_shot(GenShot* shot);
	void delete_shot(GenShot* shot);
	std::set<GenShot*>::iterator get_shots();
	std::set<GenShot*>::iterator end_shots();
	
	std::map<std::pair<int,int>,Upgrade::Type>::iterator get_upgs();
	std::map<std::pair<int,int>,Upgrade::Type>::iterator end_upgs();
	
	void explode(double x, double y);
};


struct ControlState{
	bool rt,lt,bk,fd,sht;
};

class Controlable{
public:
	virtual void set_ctrl(ControlState ctrl) = 0;
};

class Tank{
public:
	enum State{
		REG,
		GATLING, GATLING_WAIT, GATLING_SHOOT,
		LASER, LASER_SHOOT,
		BOMB, BOMB_SHOOT,
	};
private:
	
	Game* game;
	Team* team;
	double x,y,ang;
	Tank::State state;
	std::queue<ControlState> ctrl;
	ControlState p_ctrl;
	bool dead;
	friend Round;
	friend Game;
	
	int ind;
	
	int shot_num;
	
	int timer;
	
	friend RegShot;
	friend LaserShot;
	friend BombShot;
		
	bool can_step();
	void clear_control();
	
	bool check_wall_coll(double& nx, double& ny, double& px, double& py, double& dp);
	
	void reset(double x, double y, double ang);
	
	bool check_upg(Upgrade u);	
	
	Controlable* ctbl;
public:
	Tank(Game* game, int i, Team* t);
	~Tank();
	
	Team* get_team();
	
	double get_x();
	double get_y();
	double get_ang();
	bool is_dead();
	Tank::State get_state();
	
	int get_ind();
	
	void push_control(ControlState st);
	
	ControlState get_ctrl();
	
	void step();
	
	void kill();

	bool collide_upgrade(Upgrade u);
};


class GenShot{
public:
	enum Type{
		TYPE_REG,
		TYPE_GATLING,
		TYPE_LASER,
		TYPE_BOMB,
		TYPE_FRAGMENT,
	};
private:
	Tank* tank;
	Game* game;
	int id;
	long long int ctime;
	int cround;
protected:
	bool out_of_tank;
	Game* get_game();
public:
	GenShot(Game* game, Tank* tank);
	virtual ~GenShot();
	virtual bool check_tank(Tank* tank, bool ignore_me) = 0;
	virtual void advance() = 0;
	virtual GenShot::Type get_type() = 0;
	Tank* get_tank();
	
	virtual double get_ang() = 0;
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	
	virtual bool is_reusable(); // destoyed by killing tank
	
	int get_id();
	long long int get_ctime();
	int get_cround();
};
class Shot : public GenShot{
private:
	double x,y,vx,vy;
	std::vector<std::pair<double,double>> colls;
	
	std::map<Tank*, double> hits;
	
	bool found;
	double nx,ny;
	
	int tm;
	double col_t;
	void reflect();
	double check_wall();
public:
	Shot(Game* game, Tank* tank, double div, double spd);
	bool check_tank(Tank* tank, bool ignore_me);
	void advance();
	double get_ang();
	double get_x();
	double get_y();

	double get_vx();
	double get_vy();
	
	virtual double get_r() = 0;
	virtual int get_ttl() = 0;
	
	std::vector<std::pair<double,double>>& get_colls();
	
	
};

class RegShot : public Shot{
public:
	RegShot(Game* game, Tank* tank);
	~RegShot();
	double get_r();
	int get_ttl();
	GenShot::Type get_type();
};
class GatShot : public Shot{
public:
	GatShot(Game* game, Tank* tank);
	~GatShot();
	double get_r();
	int get_ttl();
	GenShot::Type get_type();
};
class LaserShot : public Shot{
public:
	LaserShot(Game* game, Tank* tank);
	~LaserShot();
	double get_r();
	int get_ttl();
	GenShot::Type get_type();
};
class BombShot : public Shot, public Controlable{
	bool prs;
public:
	BombShot(Game* game, Tank* tank);
	~BombShot();
	double get_r();
	int get_ttl();
	GenShot::Type get_type();
	void set_ctrl(ControlState ctrl);
};

class Fragment : public GenShot{
	double x,y,ang;
	int time;
	double tot_dst;
public:
	Fragment(Game* game, double x,double y);
	~Fragment();
	bool check_tank(Tank* tank, bool ignore_me);
	void advance();
	GenShot::Type get_type();
	
	void check_wall();
	
	double get_ang();
	double get_x();
	double get_y();
	double get_x(double t);
	double get_y(double t);
	double get_dst();
	double get_dst(double t);
	
	double get_t();
	
	bool is_reusable(); // destoyed by killing tank
};




class GameEvent{
public:
	enum Type{
		TYPE_START_RND,
		TYPE_TANK_DEAD,
		TYPE_SCORE,
		TYPE_END_GAME,
		TYPE_SHOT_CRT, TYPE_SHOT_RMV,
		TYPE_UPG_CRT, TYPE_UPG_RMV,
	};
	virtual Type get_type()=0;
};

class GameEventStartRnd : public GameEvent{
public:
	Type get_type(){return GameEvent::TYPE_START_RND;}
};
class GameEventTankDeath : public GameEvent{
	int ind;
public:
	GameEventTankDeath(int i);
	Type get_type(){return GameEvent::TYPE_TANK_DEAD;}
	int get_ind();
};
class GameEventScore : public GameEvent{
	int ind;
	int diff;
public:
	GameEventScore(int i, int diff);
	Type get_type(){return GameEvent::TYPE_SCORE;}
	int get_ind();
	int get_diff();
};
class GameEventEndGame : public GameEvent{
	std::vector<int> scores;
public:
	GameEventEndGame(const std::vector<Team*>& teams);
	Type get_type(){return GameEvent::TYPE_END_GAME;}
	std::vector<int>& get_scores();
};
class GameEventCreateShot : public GameEvent{
	GenShot* shot;
public:
	GameEventCreateShot(GenShot* s);
	Type get_type(){return GameEvent::TYPE_SHOT_CRT;}
	GenShot* get_shot();
};
class GameEventRemoveShot : public GameEvent{
	int id;
	double x,y,vx,vy;
	GenShot::Type type;
public:
	GameEventRemoveShot(GenShot* s);
	Type get_type(){return GameEvent::TYPE_SHOT_RMV;}
	int get_id();
	double get_x();
	double get_y();
	double get_vx();
	double get_vy();
	GenShot::Type get_stype();
};
class GameEventCreateUpgrade : public GameEvent{
	Upgrade u; int rnd;
public:
	GameEventCreateUpgrade(Upgrade u, int round);
	Type get_type(){return GameEvent::TYPE_UPG_CRT;}
	Upgrade get_upg();
	int get_round();
};
class GameEventRemoveUpgrade : public GameEvent{
	int x,y,rnd;
public:
	GameEventRemoveUpgrade(Upgrade u, int round);
	Type get_type(){return GameEvent::TYPE_UPG_RMV;}
	int get_x();
	int get_y();
	int get_round();
};


#endif