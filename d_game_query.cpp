#include "d_game_query.h"

GameDQEventStartRnd::GameDQEventStartRnd(GameEventStartRnd* event){ e = event; }
GameDQEventStartRnd::~GameDQEventStartRnd(){ delete e; }

GameDQEventTankDeath::GameDQEventTankDeath(GameEventTankDeath* event){ e = event; }
GameDQEventTankDeath::~GameDQEventTankDeath() { delete e; }
int GameDQEventTankDeath::get_ind(){ return e->get_ind(); }

GameDQEventScore::GameDQEventScore(GameEventScore* event){ e=event; }
GameDQEventScore::~GameDQEventScore(){ delete e; }
int GameDQEventScore::get_ind() { return e->get_ind(); }
int GameDQEventScore::get_diff() { return e->get_diff(); }

GameDQEventEndGame::GameDQEventEndGame(GameEventEndGame* event){ e = event; }
GameDQEventEndGame::~GameDQEventEndGame(){ delete e; }
std::vector<int>& GameDQEventEndGame::get_scores(){ return e->get_scores(); }

GameDQ::GameDQ(Game* g){
	game = g;
	round = new RoundDQ(game->get_round());
	for(int i = 0; i<get_tank_num(); i++) tanks.push_back(new TankDQ(game->get_tank(i)));
}
GameDQ::~GameDQ(){
	delete round;
	for(int i = 0; i<get_tank_num(); i++) delete tanks[i];
}
int GameDQ::get_tank_num(){
	return game->get_tank_num();
}
TankQ* GameDQ::get_tank(int i){
	return tanks[i];
}
RoundQ* GameDQ::get_round(){
	return round;
}
GameQEvent* GameDQ::get_event(){
	GameEvent* e = game->get_event();
	if(e){
		switch(e->get_type()){
		case GameEvent::TYPE_START_RND:
			delete round;
			round = new RoundDQ(game->get_round());
			return new GameDQEventStartRnd((GameEventStartRnd*)e);
		case GameEvent::TYPE_TANK_DEAD:
			return new GameDQEventTankDeath((GameEventTankDeath*)e);
		case GameEvent::TYPE_SCORE:
			return new GameDQEventScore((GameEventScore*)e);
		case GameEvent::TYPE_END_GAME:
			return new GameDQEventEndGame((GameEventEndGame*)e);
		}
	}
	return NULL;
}
void GameDQ::advance(){
	game->advance();
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
void TankDQ::push_ctrl(ControlState ctrl){
	tank->push_control(ctrl);
}
int TankDQ::get_ind(){
	return tank->get_ind();
}

RoundDQ::RoundDQ(Round* r){
	round = r;
}
Maze* RoundDQ::get_maze(){
	return round->get_maze();
}
std::set<GenShotQ*> RoundDQ::get_shots(){
	std::set<GenShotQ*> ret;
	for(auto it = round->get_shots(); it!=round->end_shots(); it++){
		switch((*it)->get_type()){
		case GenShot::TYPE_REG:
			ret.insert(new ShotDQ((Shot*)(*it)));
			break;
		}
	}
	return ret;
}

ShotDQ::ShotDQ(Shot* s){
	shot = s;
}
double ShotDQ::get_r(){
	return ((Shot*)shot)->get_r();
}
std::vector<std::pair<double,double>>& ShotDQ::get_colls(){
	return ((Shot*)shot)->get_colls();
}
double ShotDQ::get_x(){
	return shot->get_x();
}
double ShotDQ::get_y(){
	return shot->get_y();
}
double ShotDQ::get_ang(){
	return shot->get_ang();
}
GenShot::Type ShotDQ::get_type(){
	return shot->get_type();
}
