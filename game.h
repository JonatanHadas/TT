#ifndef _GAME_H
#define _GAME_H

#include <vector>
#include <queue>

#include "maze.h"

// a whole game
class Game;
// a single tank
class Tank;
// all round specific data
class Round;


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
public:
	Game(int tank_num);
	~Game();
	void start_round();
	Round* get_round();
	Tank* get_tank(int i);
	int get_tank_num();
	GameEvent* get_event();
	
	long long int get_time();
	void step();
};

class Round{
	Game* game;
	Maze* maze;
public:
	Round(Game* game);
	~Round();
	void step();
	Maze* get_maze();
};

class Tank{
	Game* game;
	double x,y,ang;
	bool dead;
	friend Round;
public:
	Tank(Game* game);
	~Tank();
	
	double get_x();
	double get_y();
	double get_ang();
	bool is_dead();
	
	void step();
};

#endif