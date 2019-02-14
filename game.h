#ifndef _GAME_H
#define _GAME_H

#include <vector>

// a whole game
class Game;
// a single tank
class Tank;
// all round specific data
class Round;

class Game{
	std::vector<Tank*> tanks;
	Round* round;
public:
	Game(int tank_num);
	~Game();
	void start_round();
	Round* get_round();
	Tank* get_tank(int i);
	int get_tank_num();
};

class Round{
	Game* game;
public:
	Round(game* game);
	~Round();
}

class Tank{
	Game* game;
	double x,y,ang;
	bool dead;
public:
	Tank(Game* game);
	~Tank();
	
	double get_x();
	double get_y();
	double get_ang();
	bool is_dead();
}

#endif