#include "d_game_query.h"

GameDQEventStartRnd::GameDQEventStartRnd(GameEventStartRnd* event){ e = event; }
GameDQEventStartRnd::~GameDQEventStartRnd(){ delete e; }

GameDQ::GameDQ(Game* g){
	game = g;
}
int GameDQ::get_tank_num(){
	return game->get_tank_num();
}
TankQ* GameDQ::get_tank(int i){
	return new TankDQ(game->get_tank(i));
}
RoundQ* GameDQ::get_round(){
	return new RoundDQ(game->get_round());
}
GameQEvent* GameDQ::get_event(){
	GameEvent* e = game->get_event();
	if(e){
		switch(e->get_type()){
		case GameEvent::TYPE_START_RND:
			return new GameDQEventStartRnd((GameEventStartRnd*)e);
		}
	}
	return NULL;
}

TankDQ::TankDQ(Tank* t){
	tank = t;
}
double TankDQ::get_x(){
	return tank->get_x();
}
double TankDQ::get_y(){
	return tank->get_y();
}
double TankDQ::get_ang(){
	return tank->get_ang();
}
bool TankDQ::is_dead(){
	return tank->is_dead();
}

RoundDQ::RoundDQ(Round* r){
	round = r;
}
Maze* RoundDQ::get_maze(){
	return round->get_maze();
}
