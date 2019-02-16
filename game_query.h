#ifndef _GAME_QUERY_H
#define _GAME_QUERY_H

#include "maze.h"
#include "game.h"

class TankQ;
class RoundQ;
class GameQ;

class GameQEvent;
class GameQEventStartRnd;

class GameQEvent{
public:
	enum Type{
		TYPE_RND_START,
	};
	virtual Type get_type() = 0;
};

class GameQEventStartRnd : public GameQEvent{
	Type get_type(){return GameQEvent::TYPE_RND_START;}
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
	
	virtual void push_ctrl(ControlState ctrl) = 0;
};
class RoundQ{
public:
	virtual Maze* get_maze() = 0;
};

#endif