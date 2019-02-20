#ifndef _GAME_H
#define _GAME_H

#include <vector>
#include <map>
#include <set>
#include <queue>

#include "maze.h"
#include "game_config.h"

// a whole game
class Game;
// a single tank
class Tank;
// all round specific data
class Round;

class GenShot;
class Shot;
class RegShot;

//for scoring
class Team;

class GameEvent{
public:
	enum Type{
		TYPE_START_RND,
		TYPE_TANK_DEAD,
		TYPE_SCORE,
		TYPE_END_GAME,
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
	Round* round;
	std::queue<GameEvent*> events;
	
	long long int time;
	int end_timer;
	
	bool can_step();
	void step();
	GameSettings set;
	int round_num;
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
};

class Round{
	Game* game;
	Maze* maze;
	
	std::set<GenShot*> shots;
	std::set<GenShot*> shts_fd;
public:
	Round(Game* game);
	~Round();
	void step();
	Maze* get_maze();
	
	void add_shot(GenShot* shot);
	void delete_shot(GenShot* shot);
	std::set<GenShot*>::iterator get_shots();
	std::set<GenShot*>::iterator end_shots();
};

struct ControlState{
	bool rt,lt,bk,fd,sht;
};

class Tank{
	Game* game;
	Team* team;
	double x,y,ang;
	std::queue<ControlState> ctrl;
	ControlState p_ctrl;
	bool dead;
	friend Round;
	friend Game;
	
	int ind;
	
	int shot_num;
	
	friend RegShot;
		
	bool can_step();
	void clear_control();
	
	bool check_wall_coll(double& nx, double& ny, double& px, double& py, double& dp);
	
	void reset(double x, double y, double ang);
public:
	Tank(Game* game, int i, Team* t);
	~Tank();
	
	double get_x();
	double get_y();
	double get_ang();
	bool is_dead();
	
	int get_ind();
	
	void push_control(ControlState st);
	
	void step();
	
	void kill();
};


class GenShot{
public:
	enum Type{
		TYPE_REG,
	};
private:
	Tank* tank;
	Game* game;
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

#endif