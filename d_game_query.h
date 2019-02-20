#ifndef _D_GAME_QUERY_H
#define _D_GAME_QUERY_H

#include "game_query.h"
#include "game.h"

class TankDQ;
class RoundDQ;
class GameDQ;

class GameDQEventStartRnd;
class GameDQEventTankDeath;
class GameDQEventScore;
class GameDQEventEndGame;

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


class GameDQ : public GameQ{
	Game* game;
	RoundDQ* round;
	std::vector<TankDQ*> tanks;
public:
	GameDQ(Game* game);
	~GameDQ();
	int get_tank_num();
	TankQ* get_tank(int i);
	RoundQ* get_round();
	GameQEvent* get_event();
	
	void advance();
};
class TankDQ : public TankQ{
	Tank* tank;
public:
	TankDQ(Tank* tank);
	double get_x();
	double get_y();
	double get_ang();
	bool is_dead();
	
	int get_ind();
	
	void push_ctrl(ControlState ctrl);
};
class RoundDQ : public RoundQ{
	Round* round;
public:
	RoundDQ(Round* round);
	Maze* get_maze();
	std::set<GenShotQ*> get_shots();
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
#endif