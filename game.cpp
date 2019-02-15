#include "game.h"
#include "geom.h"

Game::Game(int tank_num){
	for(int i = 0; i<tank_num; i++) tanks.push_back(new Tank(this));
	round = NULL;
	start_round();
}

Game::~Game(){
	for(int i = 0; i<tanks.size(); i++) delete tanks[i];
	if(round) delete round;
}
void Game::start_round(){
	if(round) delete round;
	round = new Round(this);
	events.push(new GameEventStartRnd());
}
Round* Game::get_round(){
	return round;
}
Tank* Game::get_tank(int i){
	return tanks[i];
}
int Game::get_tank_num(){
	return tanks.size();
}
GameEvent* Game::get_event(){
	if(events.size()==0) return NULL;
	else{
		GameEvent* e = events.front();
		events.pop();
		return e;
	}
}

Round::Round(Game* g){
	game=g;
}
Round::~Round(){
	
}

Tank::Tank(Game* g){
	game = g;
	x = y = ang = 0;
	dead = false;
}
Tank::~Tank(){
	
}
double Tank::get_x(){
	return x;
}
double Tank::get_y(){
	return y;
}
double Tank::get_ang(){
	return ang;
}
bool Tank::is_dead(){
	return dead;
}