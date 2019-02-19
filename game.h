#ifndef _GAME_H
#define _GAME_H

#include <vector>
#include <map>
#include <set>
#include <queue>

#include "maze.h"

// a whole game
class Game;
// a single tank
class Tank;
// all round specific data
class Round;

class GenShot;
class Shot;
class RegShot;


class GameEvent{
public:
	enum Type{
		TYPE_START_RND,
	};
	virtual Type get_type()=0;
};

class GameEventStartRnd : public GameEvent{
	Type get_type(){return GameEvent::TYPE_START_RND;}
};

class Game{
	std::vector<Tank*> tanks;
	Round* round;
	std::queue<GameEvent*> events;
	
	long long int time;
	
	bool can_step();
	void step();
public:
	Game(int tank_num);
	~Game();
	void start_round();
	Round* get_round();
	Tank* get_tank(int i);
	int get_tank_num();
	GameEvent* get_event();
	
	long long int get_time();
	void advance();
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
	Tank(Game* game, int i);
	~Tank();
	
	double get_x();
	double get_y();
	double get_ang();
	bool is_dead();
	
	int get_ind();
	
	void push_control(ControlState st);
	
	void step();
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