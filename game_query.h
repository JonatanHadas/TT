#ifndef _GAME_QUERY_H
#define _GAME_QUERY_H

#include "maze.h"
#include "game.h"

class TankQ;
class RoundQ;
class GameQ;

class GenShotQ;
class ShotQ;

class GameQEvent;
class GameQEventStartRnd;

class GameQEvent{
public:
	enum Type{
		TYPE_RND_START,
		TYPE_TANK_DEAD,
	};
	virtual Type get_type() = 0;
};

class GameQEventStartRnd : public GameQEvent{
	Type get_type(){return GameQEvent::TYPE_RND_START;}
};
class GameQEventTankDeath : public GameQEvent{
	Type get_type(){return GameQEvent::TYPE_TANK_DEAD;}
	virtual int get_ind() = 0;
};

class GameQ{
public:
	virtual int get_tank_num() = 0;
	virtual TankQ* get_tank(int i) = 0;
	virtual RoundQ* get_round() = 0;
	virtual GameQEvent* get_event() = 0;
	
	virtual void advance() = 0;
};
class TankQ{
public:
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	virtual double get_ang() = 0;
	virtual bool is_dead() = 0;
	
	virtual int get_ind() = 0;
	
	virtual void push_ctrl(ControlState ctrl) = 0;
};
class RoundQ{
public:
	virtual Maze* get_maze() = 0;
	virtual std::set<GenShotQ*> get_shots() = 0;
};
class GenShotQ{
public:
	virtual double get_x() = 0;
	virtual double get_y() = 0;
	virtual double get_ang() = 0;
	virtual GenShot::Type get_type() = 0;
};

class ShotQ : public GenShotQ{
public:
	virtual double get_r() = 0;
	virtual std::vector<std::pair<double,double>>& get_colls() = 0;
};

#endif