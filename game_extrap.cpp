#include "game_extrap.h"

#include "game_consts.h"

#include "geom.h"

ExEventTankDeath::ExEventTankDeath(int i){
	ind = i;
}
int ExEventTankDeath::get_ind(){
	return ind;
}

ExEventScore::ExEventScore(int i, int d){
	ind = i;
	diff = d;
}
int ExEventScore::get_ind(){
	return ind;
}
int ExEventScore::get_diff(){
	return diff;
}

ExEventEndGame::ExEventEndGame(GameExtrap* game){
	for(int i = 0; i<game->get_team_num(); i++) scores.push_back(game->get_team(i)->get_score());
}
std::vector<int>& ExEventEndGame::get_scores(){
	return scores;
}

ExEventCreateShot::ExEventCreateShot(GenShotExtrap* s){
	id = s->get_id();
	type = s->get_type();
	tank = s->get_tank();
	x = s->get_x();
	y = s->get_y();
	ang = s->get_ang();
}
int ExEventCreateShot::get_id(){
	return id;
}
GenShot::Type ExEventCreateShot::get_stype(){
	return type;
}
double ExEventCreateShot::get_x(){
	return x;
}
double ExEventCreateShot::get_y(){
	return y;
}
double ExEventCreateShot::get_ang(){
	return ang;
}
int ExEventCreateShot::get_tank_ind(){
	return tank->get_ind();
}

ExEventRemoveShot::ExEventRemoveShot(GenShotExtrap* s){
	id = s->get_id();
	type = s->get_type();
	x = s->get_x();
	y = s->get_y();
	double spd;
	switch(type){
	case GenShot::TYPE_REG:
		spd = STEP_SHOT;
		break;
	}
	vx = cos(s->get_ang())*spd;
	vy = sin(s->get_ang())*spd;
}
int ExEventRemoveShot::get_id(){
	return id;
}
double ExEventRemoveShot::get_x(){
	return x;
}
double ExEventRemoveShot::get_y(){
	return y;
}
double ExEventRemoveShot::get_vx(){
	return vx;
}
double ExEventRemoveShot::get_vy(){
	return vy;
}
GenShot::Type ExEventRemoveShot::get_stype(){
	return type;
}

TeamExtrap::TeamExtrap(int i){
	ind = i;
	score = 0;
}
int TeamExtrap::get_score(){
	return score;
}
void TeamExtrap::add_score(int diff){
	score += diff;
}
int TeamExtrap::get_ind(){
	return ind;
}

GameExtrap::GameExtrap(GameConfig& cf, ExInEvents* i){
	in = i;
	set = cf.set;
	round_num = 0;
	time = 0;
	round = NULL;
	for(int i = 0; i<cf.team_num; i++) teams.push_back(new TeamExtrap(i));
	for(int i = 0; i<cf.tank_num; i++) tanks.push_back(new TankExtrap(this, i, teams[cf.team_inds[i]]));	
}
GameExtrap::~GameExtrap(){
	if(round) delete round;
	for(int i = 0; i<get_tank_num(); i++) delete tanks[i];
	for(int i = 0; i<get_team_num(); i++) delete teams[i];	
}
RoundExtrap* GameExtrap::get_round(){
	return round;
}
int GameExtrap::get_tank_num(){
	return tanks.size();
}
TankExtrap* GameExtrap::get_tank(int i){
	return tanks[i];
}
int GameExtrap::get_team_num(){
	return teams.size();
}
TeamExtrap* GameExtrap::get_team(int i){
	return teams[i];
}

void GameExtrap::step(){
	ExInEvent* e;
	time++;
	while(e = in->get_event()){
		switch(e->get_type()){
		case ExInEvent::TYPE_TANK_UPDATE:
			get_tank(((ExInEventTankUpdate*)e)->get_ind())->update(((ExInEventTankUpdate*)e));
			break;
		case ExInEvent::TYPE_START_RND:
			start_round((ExInEventStartRound*)e);
			break;
		case ExInEvent::TYPE_TANK_DEAD:
			kill_tank(((ExInEventTankDeath*)e)->get_ind());
			break;
		case ExInEvent::TYPE_SCORE:
			add_score(((ExInEventScore*)e)->get_ind(), ((ExInEventScore*)e)->get_diff());
			break;
		case ExInEvent::TYPE_END_GAME:
			break;
		case ExInEvent::TYPE_SHT_CRT:
			break;
		case ExInEvent::TYPE_SHT_RMV:
			break;
		}
		delete e;
	}
}
ExEvent* GameExtrap::get_event(){
	ExEvent* ret = NULL;
	if(events.size()>0){
		ret = events.front();
		events.pop();
	}
	return ret;
}

void GameExtrap::start_round(ExInEventStartRound* e){
	round_num++;
	time = 0;
	if(round) delete round;
	round = new RoundExtrap(this, e->get_maze());
	for(int i = 0; i<get_tank_num(); i++) get_tank(i)->reset(e->get_x(i)+0.5, e->get_y(i)+0.5, e->get_ang(i));
	events.push(new ExEventStartRnd());
}
void GameExtrap::kill_tank(int ind){
	get_tank(ind)->kill();
	events.push(new ExEventTankDeath(ind));
}
void GameExtrap::add_score(int ind, int diff){
	get_team(ind)->add_score(diff);
	events.push(new ExEventScore(ind, diff));
}
void GameExtrap::push_ctrl(ControlState ctrl, int ind){
	in->push_ctrl(ctrl, ind, round_num);
}

RoundExtrap::RoundExtrap(GameExtrap* g,Maze* m){
	game = g;
	maze = m;
}
Maze* RoundExtrap::get_maze(){
	return maze;
}
std::map<int, GenShotExtrap*>::iterator RoundExtrap::get_shots(){
	return shots.begin();
}
std::map<int, GenShotExtrap*>::iterator RoundExtrap::end_shots(){
	return shots.end();
}

TankExtrap::TankExtrap(GameExtrap* g,int i, TeamExtrap* t){
	game = g;
	team = t;
	ind = i;
}
TeamExtrap* TankExtrap::get_team(){
	return team;
}

double TankExtrap::get_x(){
	return x;
}
double TankExtrap::get_y(){
	return y;
}
double TankExtrap::get_ang(){
	return ang;
}

void TankExtrap::update(ExInEventTankUpdate* e){
	if(e->get_time()>b_t){
		t = b_t = e->get_time();
		x = e->get_x();
		y = e->get_y();
		ang = e->get_ang();
	}
}
void TankExtrap::reset(double xx, double yy, double a){
	x = xx;
	y = yy;
	ang = a;
	t = b_t = 0;
	dead = false;
}
int TankExtrap::get_ind(){
	return ind;
}

bool TankExtrap::is_dead(){
	return dead;
}
void TankExtrap::kill(){
	dead = true;
}

void TankExtrap::push_control(ControlState ctrl){
	game->push_ctrl(ctrl, ind);
}

GenShotExtrap::GenShotExtrap(GameExtrap* g, TankExtrap* tk, int i, GenShot::Type tp){
	game = g; tank = tk; id = i; type = tp;
}

GameExtrap* GenShotExtrap::get_game(){ return game; }
TankExtrap* GenShotExtrap::get_tank(){ return tank; }
int GenShotExtrap::get_id(){ return id; }
GenShot::Type GenShotExtrap::get_type(){ return type; }

ShotExtrap::ShotExtrap(GameExtrap* game, TankExtrap* tank, int id, GenShot::Type type) : GenShotExtrap(game, tank, id, type){
}
double ShotExtrap::get_r(){
	switch(get_type()){
	case GenShot::TYPE_REG:
		return SHOT_R;
	}

}
std::vector<std::pair<double, double>>& ShotExtrap::get_colls(){
	return colls;
}
