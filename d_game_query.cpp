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

GameDQEventCreateShot::GameDQEventCreateShot(GameEventCreateShot* event){ e = event; }
GameDQEventCreateShot::~GameDQEventCreateShot(){ delete e; }
int GameDQEventCreateShot::get_id(){ return e->get_shot()->get_id(); }
GenShot::Type GameDQEventCreateShot::get_stype(){ return e->get_shot()->get_type(); }
double GameDQEventCreateShot::get_x(){ return e->get_shot()->get_x(); }
double GameDQEventCreateShot::get_y(){ return e->get_shot()->get_y(); }
double GameDQEventCreateShot::get_ang(){ return e->get_shot()->get_ang(); }
int GameDQEventCreateShot::get_tank_ind(){ return e->get_shot()->get_tank()->get_ind(); }

GameDQEventRemoveShot::GameDQEventRemoveShot(GameEventRemoveShot* event){ e = event; }
GameDQEventRemoveShot::~GameDQEventRemoveShot(){ delete e; }
int GameDQEventRemoveShot::get_id(){ return e->get_id(); }

GameDQEventCreateMine::GameDQEventCreateMine(GameEventCreateMine* event){ e = event; }
GameDQEventCreateMine::~GameDQEventCreateMine(){ delete e; }
double GameDQEventCreateMine::get_x(){ return e->get_mine()->get_x(); }
double GameDQEventCreateMine::get_y(){ return e->get_mine()->get_y(); }
double GameDQEventCreateMine::get_ang(){ return e->get_mine()->get_ang(); }

GameDQ::GameDQ(Game* g){
	game = g;
	round = new RoundDQ(game->get_round());
	for(int i = 0; i<get_team_num(); i++) teams.push_back(new TeamDQ(game->get_team(i)));
	for(int i = 0; i<get_tank_num(); i++) tanks.push_back(new TankDQ(game->get_tank(i), this));
}
GameDQ::~GameDQ(){
	delete round;
	for(int i = 0; i<get_tank_num(); i++) delete tanks[i];
	for(int i = 0; i<get_team_num(); i++) delete teams[i];
}
int GameDQ::get_tank_num(){
	return game->get_tank_num();
}
TankQ* GameDQ::get_tank(int i){
	return tanks[i];
}
int GameDQ::get_team_num(){
	return game->get_team_num();
}
TeamQ* GameDQ::get_team(int i){
	return teams[i];
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
		case GameEvent::TYPE_SHOT_CRT:
			return new GameDQEventCreateShot((GameEventCreateShot*)e);
		case GameEvent::TYPE_SHOT_RMV:
			return new GameDQEventRemoveShot((GameEventRemoveShot*)e);
		case GameEvent::TYPE_MIN_CRT:
			return new GameDQEventCreateMine((GameEventCreateMine*)e);
		case GameEvent::TYPE_MIN_ACT:
			return new GameQEventEtc(GameQEvent::TYPE_MIN_ACT);
		case GameEvent::TYPE_COLL:
			return new GameQEventEtc(GameQEvent::TYPE_COLL);
		case GameEvent::TYPE_EXPL:
			return new GameQEventEtc(GameQEvent::TYPE_EXPL);
		case GameEvent::TYPE_LOAD:
			return new GameQEventEtc(GameQEvent::TYPE_LOAD);
		}
	}
	return NULL;
}
void GameDQ::advance(){
	game->advance();
}
long long int GameDQ::get_time(){
	return game->get_time();
}

TeamDQ::TeamDQ(Team* t){
	team = t;
}
int TeamDQ::get_ind(){
	return team->get_ind();
}
int TeamDQ::get_score(){
	return team->get_score();
}
int TeamDQ::get_tank_num(){
	return tanks.size();
}
TankQ* TeamDQ::get_tank(int i){
	return tanks[i];
}

TankDQ::TankDQ(Tank* t, GameDQ* game){
	tank = t;
	team = (TeamDQ*)game->get_team(tank->get_team()->get_ind());
	team->tanks.push_back(this);
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
Tank::State TankDQ::get_state(){
	return tank->get_state();
}

void TankDQ::push_ctrl(ControlState ctrl){
	tank->push_control(ctrl);
}
int TankDQ::get_ind(){
	return tank->get_ind();
}
TeamQ* TankDQ::get_team(){
	return team;
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
		case GenShot::TYPE_GATLING:
		case GenShot::TYPE_LASER:
		case GenShot::TYPE_BOMB:
			ret.insert(new ShotDQ((Shot*)(*it)));
			break;
		case GenShot::TYPE_FRAGMENT:
			ret.insert(new FragmentDQ((Fragment*)(*it)));
			break;
		case GenShot::TYPE_DEATH_RAY:
			ret.insert(new DeathRayDQ((DeathRay*)(*it)));
			break;
		case GenShot::TYPE_WIFI:
		case GenShot::TYPE_MISSILE:
			ret.insert(new MissileDQ((Missile*)(*it)));
			break;
		}
	}
	return ret;
}
std::set<MineQ*> RoundDQ::get_mines(){
	std::set<MineQ*> ret;
	for(auto it = round->get_mines(); it!=round->end_mines(); it++){
		ret.insert(new MineDQ(*it));
	}
	return ret;
}
std::set<Upgrade*> RoundDQ::get_upgs(){
	std::set<Upgrade*> ret;
	for(auto it = round->get_upgs(); it!=round->end_upgs(); it++){
		Upgrade* u = new Upgrade({(*it).first.first,(*it).first.second, (*it).second});
		ret.insert(u);
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

FragmentDQ::FragmentDQ(Fragment* f){
	frag = f;
}
double FragmentDQ::get_x(){
	return frag->get_x();
}
double FragmentDQ::get_y(){
	return frag->get_y();
}
double FragmentDQ::get_ang(){
	return frag->get_ang();
}
double FragmentDQ::get_dst(){
	return frag->get_dst();
}
double FragmentDQ::get_t(){
	return frag->get_t();
}
GenShot::Type FragmentDQ::get_type(){
	return frag->get_type();
}

DeathRayDQ::DeathRayDQ(DeathRay* d){
	dr = d;
}
int DeathRayDQ::get_point_num(){
	return dr->get_point_num();
}
double DeathRayDQ::get_x(int i){
	return dr->get_point(i).first;
}
double DeathRayDQ::get_y(int i){
	return dr->get_point(i).second;
}
double DeathRayDQ::get_x(){
	return dr->get_x();
}
double DeathRayDQ::get_y(){
	return dr->get_y();
}
double DeathRayDQ::get_ang(){
	return dr->get_ang();
}
GenShot::Type DeathRayDQ::get_type(){
	return dr->get_type();
}
int DeathRayDQ::get_tank_ind(){
	return dr->get_tank()->get_ind();
}

MissileDQ::MissileDQ(Missile* m){
	mis = m;
}
int MissileDQ::get_tank_ind(){
	return mis->get_tank()->get_ind();
}
int MissileDQ::get_tar_ind(){
	return mis->get_target()->get_ind();
}
double MissileDQ::get_x(){
	return mis->get_x();
}
double MissileDQ::get_y(){
	return mis->get_y();
}
double MissileDQ::get_ang(){
	return mis->get_ang();
}
	
GenShot::Type MissileDQ::get_type(){
	return mis->get_type();
}

MineDQ::MineDQ(Mine* m){
	mine = m;
}
double MineDQ::get_x(){
	return mine->get_x();
}
double MineDQ::get_y(){
	return mine->get_y();
}
double MineDQ::get_ang(){
	return mine->get_ang();
}
bool MineDQ::get_started(){
	return mine->get_started();
}
bool MineDQ::get_active(){
	return mine->get_active();
}
int MineDQ::get_tank_ind(){
	return mine->get_tank()->get_ind();
}