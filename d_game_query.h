#ifndef _D_GAME_QUERY_H
#define _D_GAME_QUERY_H

#include "game_query.h"
#include "game.h"

class TankDQ;
class RoundDQ;
class GameDQ;

class GameDQEventStartRnd;

class GameDQEventStartRnd : public GameQEventStartRnd{
	GameEventStartRnd* e;
public:
	GameDQEventStartRnd(GameEventStartRnd* event);
	~GameDQEventStartRnd();
};

class GameDQ : public GameQ{
	Game* game;
public:
	GameDQ(Game* game);
	int get_tank_num();
	TankQ* get_tank(int i);
	RoundQ* get_round();
	GameQEvent* get_event();
};
class TankDQ : public TankQ{
	Tank* tank;
public:
	TankDQ(Tank* tank);
	double get_x();
	double get_y();
	double get_ang();
	bool is_dead();
};
class RoundDQ : public RoundQ{
	Round* round;
public:
	RoundDQ(Round* round);
	Maze* get_maze();
};

#endif