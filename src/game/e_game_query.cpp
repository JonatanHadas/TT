#include "e_game_query.h"

#include "../utils/geom.h"

GameEQEventStartRnd::GameEQEventStartRnd(ExEventStartRnd* event){ e = event; }
GameEQEventStartRnd::~GameEQEventStartRnd(){ delete e; }

GameEQEventTankDeath::GameEQEventTankDeath(ExEventTankDeath* event){ e = event; }
GameEQEventTankDeath::~GameEQEventTankDeath() { delete e; }
int GameEQEventTankDeath::get_ind(){ return e->get_ind(); }

GameEQEventScore::GameEQEventScore(ExEventScore* event){ e=event; }
GameEQEventScore::~GameEQEventScore(){ delete e; }
int GameEQEventScore::get_ind() { return e->get_ind(); }
int GameEQEventScore::get_diff() { return e->get_diff(); }

GameEQEventEndGame::GameEQEventEndGame(ExEventEndGame* event){ e = event; }
GameEQEventEndGame::~GameEQEventEndGame(){ delete e; }
std::vector<int>& GameEQEventEndGame::get_scores(){ return e->get_scores(); }

GameEQEventCreateShot::GameEQEventCreateShot(ExEventCreateShot* event){ e = event; }
GameEQEventCreateShot::~GameEQEventCreateShot(){ delete e; }
int GameEQEventCreateShot::get_id(){ return e->get_id(); }
GenShot::Type GameEQEventCreateShot::get_stype(){ return e->get_stype(); }
double GameEQEventCreateShot::get_x(){ return e->get_x(); }
double GameEQEventCreateShot::get_y(){ return e->get_y(); }
double GameEQEventCreateShot::get_ang(){ return e->get_ang(); }
int GameEQEventCreateShot::get_tank_ind(){ return e->get_tank_ind(); }

GameEQEventRemoveShot::GameEQEventRemoveShot(ExEventRemoveShot* event){ e = event; }
GameEQEventRemoveShot::~GameEQEventRemoveShot(){ delete e; }
int GameEQEventRemoveShot::get_id(){ return e->get_id(); }
double GameEQEventRemoveShot::get_x(){ return e->get_x(); }
double GameEQEventRemoveShot::get_y(){ return e->get_y(); }
double GameEQEventRemoveShot::get_vx(){ return e->get_vx(); }
double GameEQEventRemoveShot::get_vy(){ return e->get_vy(); }
GenShot::Type GameEQEventRemoveShot::get_stype(){ return e->get_stype(); }

GameEQEventCreateUpgrade::GameEQEventCreateUpgrade(ExEventCreateUpgrade* event){ e = event; }
GameEQEventCreateUpgrade::~GameEQEventCreateUpgrade(){ delete e; }
Upgrade GameEQEventCreateUpgrade::get_upg(){ return e->get_upg(); }

GameEQEventRemoveUpgrade::GameEQEventRemoveUpgrade(ExEventRemoveUpgrade* event){ e = event; }
GameEQEventRemoveUpgrade::~GameEQEventRemoveUpgrade(){ delete e; }
int GameEQEventRemoveUpgrade::get_x(){ return e->get_x(); }
int GameEQEventRemoveUpgrade::get_y(){ return e->get_y(); }

GameEQEventCreateMine::GameEQEventCreateMine(ExEventCreateMine* event){ e = event; }
GameEQEventCreateMine::~GameEQEventCreateMine(){ delete e; }
double GameEQEventCreateMine::get_x(){ return e->get_mine()->get_x(); }
double GameEQEventCreateMine::get_y(){ return e->get_mine()->get_y(); }
double GameEQEventCreateMine::get_ang(){ return e->get_mine()->get_ang(); }
int GameEQEventCreateMine::get_ind() { return e->get_mine()->get_tank()->get_ind(); }

GameEQEventTankStuck::GameEQEventTankStuck(ExEventTankStuck* event){ e = event; }
GameEQEventTankStuck::~GameEQEventTankStuck(){ delete e; }
int GameEQEventTankStuck::get_ind(){ return e->get_tank()->get_ind(); }
double GameEQEventTankStuck::get_spd(){ return e->get_spd(); }


GameEQ::GameEQ(GameExtrap* g){
	game = g;
	round = new RoundEQ(game->get_round());
	for(int i = 0; i<get_team_num(); i++) teams.push_back(new TeamEQ(game->get_team(i)));
	for(int i = 0; i<get_tank_num(); i++) tanks.push_back(new TankEQ(game->get_tank(i), this));
}
GameEQ::~GameEQ(){
	delete game;
	delete round;
	for(int i = 0; i<get_tank_num(); i++) delete tanks[i];
	for(int i = 0; i<get_team_num(); i++) delete teams[i];
}
int GameEQ::get_tank_num(){
	return game->get_tank_num();
}
TankQ* GameEQ::get_tank(int i){
	return tanks[i];
}
int GameEQ::get_team_num(){
	return game->get_team_num();
}
TeamQ* GameEQ::get_team(int i){
	return teams[i];
}
RoundQ* GameEQ::get_round(){
	return round;
}
GameQEvent* GameEQ::get_event(){
	ExEvent* e = game->get_event();
	if(e){
		switch(e->get_type()){
		case ExEvent::TYPE_START_RND:
			delete round;
			round = new RoundEQ(game->get_round());
			return new GameEQEventStartRnd((ExEventStartRnd*)e);
		case ExEvent::TYPE_TANK_DEAD:
			return new GameEQEventTankDeath((ExEventTankDeath*)e);
		case ExEvent::TYPE_SCORE:
			return new GameEQEventScore((ExEventScore*)e);
		case ExEvent::TYPE_END_GAME:
			return new GameEQEventEndGame((ExEventEndGame*)e);
		case ExEvent::TYPE_SHT_CRT:
			return new GameEQEventCreateShot((ExEventCreateShot*)e);
		case ExEvent::TYPE_SHT_RMV:
			return new GameEQEventRemoveShot((ExEventRemoveShot*)e);
		case ExEvent::TYPE_UPG_CRT:
			return new GameEQEventCreateUpgrade((ExEventCreateUpgrade*)e);
		case ExEvent::TYPE_UPG_RMV:
			return new GameEQEventRemoveUpgrade((ExEventRemoveUpgrade*)e);
		case ExEvent::TYPE_TANK_STUCK:
			return new GameEQEventTankStuck((ExEventTankStuck*)e);
		case ExEvent::TYPE_MIN_CRT:
			return new GameEQEventCreateMine((ExEventCreateMine*)e);
		case ExEvent::TYPE_MIN_ACT:
			return new GameQEventEtc(GameQEvent::TYPE_MIN_ACT);
		case ExEvent::TYPE_COLL:
			return new GameQEventEtc(GameQEvent::TYPE_COLL);
		case ExEvent::TYPE_EXPL:
			return new GameQEventEtc(GameQEvent::TYPE_EXPL);
		case ExEvent::TYPE_LOAD:
			return new GameQEventEtc(GameQEvent::TYPE_LOAD);
		}
	}
	return NULL;
}
void GameEQ::advance(){
	game->step();
}
long long int GameEQ::get_time(){
	return game->get_time();
}

void GameEQ::leave(){
	game->leave();
}

TeamEQ::TeamEQ(TeamExtrap* t){
	team = t;
}
int TeamEQ::get_ind(){
	return team->get_ind();
}
int TeamEQ::get_score(){
	return team->get_score();
}
int TeamEQ::get_tank_num(){
	return tanks.size();
}
TankQ* TeamEQ::get_tank(int i){
	return tanks[i];
}

TankEQ::TankEQ(TankExtrap* t, GameEQ* game){
	tank = t;
	team = (TeamEQ*)game->get_team(tank->get_team()->get_ind());
	team->tanks.push_back(this);
	this->game = game->game;
}
double TankEQ::get_x(){
	return tank->get_x();
}
double TankEQ::get_y(){
	return tank->get_y();
}
double TankEQ::get_ang(){
	return tank->get_ang();
}
bool TankEQ::is_dead(){
	return tank->is_dead();
}
Tank::State TankEQ::get_state(){
	return tank->get_state();
}
void TankEQ::push_ctrl(ControlState ctrl){
	tank->push_control(ctrl);
}
int TankEQ::get_ind(){
	return tank->get_ind();
}
TeamQ* TankEQ::get_team(){
	return team;
}

class ExInEventCreatePred : public ExInEventCreateShot{
	double x,y,vx,vy;
	int ind;
	long long int time;
	int round;
public:
	ExInEventCreatePred(TankExtrap* t, GameExtrap* g, double len){
		ind = t->get_ind();
		time = g->get_time()-1;
		round = g->get_round_num();
		x = t->get_x();
		y = t->get_y();
		double ang = t->get_ang();
		vx = 0; vy = 0;
		rotate_add(ang, CANNON_L, 0, x, y);
		rotate_add(ang, len, 0, vx, vy);
	}
	int get_id(){ return -1; }
	GenShot::Type get_stype(){ return GenShot::TYPE_LASER; }
	double get_x(){ return x; }
	double get_y(){ return y; }
	double get_vx(){ return vx; }
	double get_vy(){ return vy; }
	int get_tank_ind(){ return ind; }
	long long int get_time(){ return time; }
	int get_round(){ return round; }
};
class ExShotPred : public ShotExtrap{
	double r;
public:
	ExShotPred(GameExtrap* game, ExInEventCreatePred* e, double rr) : ShotExtrap(game, e){
		r = rr;
	}
	double get_r(){ return r; }
};

std::vector<std::pair<double, double>> TankEQ::predict_colls(double len, double r){
	ExInEventCreatePred e(tank, game, len);
	ExShotPred sht(game, &e, r);
	auto ret = sht.get_colls();
	return ret;
}

RoundEQ::RoundEQ(RoundExtrap* r){
	round = r;
}
Maze* RoundEQ::get_maze(){
	return round->get_maze();
}
std::set<GenShotQ*> RoundEQ::get_shots(){
	std::set<GenShotQ*> ret;
	for(auto it = round->get_shots(); it!=round->end_shots(); it++){
		switch((*it).second->get_type()){
		case GenShot::TYPE_REG:
		case GenShot::TYPE_GATLING:
		case GenShot::TYPE_LASER:
		case GenShot::TYPE_BOMB:
			ret.insert(new ShotEQ((ShotExtrap*)((*it).second)));
			break;
		case GenShot::TYPE_FRAGMENT:
			ret.insert(new FragmentEQ((FragmentExtrap*)((*it).second)));
			break;
		case GenShot::TYPE_DEATH_RAY:
			ret.insert(new DeathRayEQ((DeathRayExtrap*)((*it).second)));
			break;
		case GenShot::TYPE_WIFI:
		case GenShot::TYPE_MISSILE:
			ret.insert(new MissileEQ((MissileExtrap*)((*it).second)));
			break;
		}
	}
	return ret;
}
std::set<MineQ*> RoundEQ::get_mines(){
	std::set<MineQ*> ret;
	for(auto it = round->get_mines(); it!=round->end_mines(); it++){
		ret.insert(new MineEQ(it->second));
	}
	return ret;
}
std::set<Upgrade*> RoundEQ::get_upgs(){
	std::set<Upgrade*> ret;
	for(auto it = round->get_upgs(); it!=round->end_upgs(); it++){
		Upgrade* u = new Upgrade({(*it).first.first,(*it).first.second, (*it).second});
		ret.insert(u);
	}
	return ret;
}

ShotEQ::ShotEQ(ShotExtrap* s){
	shot = s;
}
double ShotEQ::get_r(){
	return shot->get_r();
}
std::vector<std::pair<double,double>>& ShotEQ::get_colls(){
	return shot->get_colls();
}
double ShotEQ::get_x(){
	return shot->get_x();
}
double ShotEQ::get_y(){
	return shot->get_y();
}
double ShotEQ::get_ang(){
	return shot->get_ang();
}
GenShot::Type ShotEQ::get_type(){
	return shot->get_type();
}
int ShotEQ::get_tank_ind(){
	return shot->get_tank()->get_ind();
}

FragmentEQ::FragmentEQ(FragmentExtrap* f){
	frag = f;
}
double FragmentEQ::get_x(){
	return frag->get_x();
}
double FragmentEQ::get_y(){
	return frag->get_y();
}
double FragmentEQ::get_ang(){
	return frag->get_ang();
}
double FragmentEQ::get_dst(){
	return frag->get_dst();
}
double FragmentEQ::get_t(){
	return frag->get_t();
}
GenShot::Type FragmentEQ::get_type(){
	return frag->get_type();
}
int FragmentEQ::get_tank_ind(){
	return -1;
}

DeathRayEQ::DeathRayEQ(DeathRayExtrap* d){
	dr = d;
}
int DeathRayEQ::get_point_num(){
	return dr->get_point_num();
}
double DeathRayEQ::get_x(int i){
	return dr->get_x(i);
}
double DeathRayEQ::get_y(int i){
	return dr->get_y(i);
}
int DeathRayEQ::get_coll_num(){
	return dr->get_coll_num();
}
double DeathRayEQ::get_coll_x(int i){
	return dr->get_coll(i).first;
}
double DeathRayEQ::get_coll_y(int i){
	return dr->get_coll(i).second;
}
double DeathRayEQ::get_coll_nx(int i){
	return dr->get_coll_norm(i).first;
}
double DeathRayEQ::get_coll_ny(int i){
	return dr->get_coll_norm(i).second;
}
double DeathRayEQ::get_x(){
	return dr->get_x();
}
double DeathRayEQ::get_y(){
	return dr->get_y();
}
double DeathRayEQ::get_ang(){
	return dr->get_ang();
}
GenShot::Type DeathRayEQ::get_type(){
	return dr->get_type();
}
int DeathRayEQ::get_tank_ind(){
	return dr->get_tank()->get_ind();
}


MissileEQ::MissileEQ(MissileExtrap* m){
	mis = m;
}
int MissileEQ::get_tank_ind(){
	return mis->get_tank()->get_ind();
}
int MissileEQ::get_tar_ind(){
	TankExtrap* tar = mis->get_target();
	return tar ? tar->get_ind() : -1;
}
double MissileEQ::get_x(){
	return mis->get_x();
}
double MissileEQ::get_y(){
	return mis->get_y();
}
double MissileEQ::get_ang(){
	return mis->get_ang();
}
int MissileEQ::get_time(){
	return mis->get_time();
}
	
GenShot::Type MissileEQ::get_type(){
	return mis->get_type();
}

MineEQ::MineEQ(MineExtrap* m){
	mine = m;
}
double MineEQ::get_x(){
	return mine->get_x();
}
double MineEQ::get_y(){
	return mine->get_y();
}
double MineEQ::get_ang(){
	return mine->get_ang();
}
bool MineEQ::get_started(){
	return mine->get_started();
}
bool MineEQ::get_active(){
	return mine->get_active();
}
int MineEQ::get_tank_ind(){
	return mine->get_tank()->get_ind();
}