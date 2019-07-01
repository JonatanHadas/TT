#include "direct_ex.h"

DirectEx::DirectEx(Game* g){
	game = g;
	t_ind = -1;
}
DirectEx::~DirectEx(){
	
}
ExInEvent* DirectEx::get_event(){
	GameEvent* e = game->get_event();
	if(e){
		switch(e->get_type()){
		case GameEvent::TYPE_START_RND:
			return new DExInEventStartRound((GameEventStartRnd*)e, game);
		case GameEvent::TYPE_TANK_DEAD:
			return new DExInEventTankDeath((GameEventTankDeath*)e);
		case GameEvent::TYPE_SCORE:
			return new DExInEventScore((GameEventScore*)e);
		case GameEvent::TYPE_END_GAME:
			return new DExInEventEndGame((GameEventEndGame*)e);
		case GameEvent::TYPE_SHOT_CRT:
			switch(((GameEventCreateShot*)e)->get_shot()->get_type()){
			case GenShot::TYPE_REG:
			case GenShot::TYPE_GATLING:
			case GenShot::TYPE_LASER:
			case GenShot::TYPE_BOMB:
				return new DExInEventCreateShot((GameEventCreateShot*)e);
			case GenShot::TYPE_FRAGMENT:
				return new DExInEventCreateFragment((GameEventCreateShot*)e);
				break;
			}
		case GameEvent::TYPE_SHOT_RMV:
			return new DExInEventRemoveShot((GameEventRemoveShot*)e);
		case GameEvent::TYPE_UPG_CRT:
			return new DExInEventCreateUpgrade((GameEventCreateUpgrade*)e);
		case GameEvent::TYPE_UPG_RMV:
			return new DExInEventRemoveUpgrade((GameEventRemoveUpgrade*)e);
		}
	}
	if(t_ind >= 0){
		return new DExInEventTankUpdate(game->get_tank(t_ind--), game);
	}
	return NULL;
}
void DirectEx::push_ctrl(ControlState ctrl, int ind, int rnd){
	if(rnd == game->get_round_num()){
		game->get_tank(ind)->push_control(ctrl);
		game->advance();
		t_ind = game->get_tank_num()-1;
	}
}

DExInEventStartRound::DExInEventStartRound(GameEventStartRnd* event, Game* game){ e = event; g = game; }
DExInEventStartRound::~DExInEventStartRound(){ delete e; }
Maze* DExInEventStartRound::get_maze(){ return g->get_round()->get_maze(); }
int DExInEventStartRound::get_x(int i){ return (int)(g->get_tank(i)->get_x()); }
int DExInEventStartRound::get_y(int i){ return (int)(g->get_tank(i)->get_y()); }
double DExInEventStartRound::get_ang(int i){ return g->get_tank(i)->get_ang(); }

DExInEventTankDeath::DExInEventTankDeath(GameEventTankDeath* event){ e = event; }
DExInEventTankDeath::~DExInEventTankDeath(){ delete e; }
int DExInEventTankDeath::get_ind(){ return e->get_ind(); }

DExInEventScore::DExInEventScore(GameEventScore* event){ e = event; }
DExInEventScore::~DExInEventScore(){ delete e; }
int DExInEventScore::get_ind(){ return e->get_ind(); }
int DExInEventScore::get_diff(){ return e->get_diff(); }

DExInEventEndGame::DExInEventEndGame(GameEventEndGame* event){ e = event; }
DExInEventEndGame::~DExInEventEndGame(){ delete e; }

DExInEventCreateShot::DExInEventCreateShot(GameEventCreateShot* event){ e = event; }
DExInEventCreateShot::~DExInEventCreateShot(){ delete e; }
int DExInEventCreateShot::get_id(){ return e->get_shot()->get_id(); }
GenShot::Type DExInEventCreateShot::get_stype(){ return e->get_shot()->get_type(); }
double DExInEventCreateShot::get_x(){ return e->get_shot()->get_x(); }
double DExInEventCreateShot::get_y(){ return e->get_shot()->get_y(); }
double DExInEventCreateShot::get_vx(){ return ((Shot*)(e->get_shot()))->get_vx(); }
double DExInEventCreateShot::get_vy(){ return ((Shot*)(e->get_shot()))->get_vy(); }
int DExInEventCreateShot::get_tank_ind(){ return e->get_shot()->get_tank()->get_ind(); }
long long int DExInEventCreateShot::get_time(){ return e->get_shot()->get_ctime(); }
int DExInEventCreateShot::get_round(){ return e->get_shot()->get_cround(); }

DExInEventRemoveShot::DExInEventRemoveShot(GameEventRemoveShot* event){ e = event; }
DExInEventRemoveShot::~DExInEventRemoveShot(){ delete e; }
int DExInEventRemoveShot::get_id(){ return e->get_id(); }

DExInEventTankUpdate::DExInEventTankUpdate(Tank* tank, Game* game){ t = tank; g = game; }
DExInEventTankUpdate::~DExInEventTankUpdate(){}
int DExInEventTankUpdate::get_ind(){ return t->get_ind(); }
double DExInEventTankUpdate::get_x(){ return t->get_x(); }
double DExInEventTankUpdate::get_y(){ return t->get_y(); }
double DExInEventTankUpdate::get_ang(){ return t->get_ang(); }
long long int DExInEventTankUpdate::get_time(){ return g->get_time(); }
ControlState DExInEventTankUpdate::get_ctrl(){ return t->get_ctrl(); }
Tank::State DExInEventTankUpdate::get_state(){ return t->get_state(); }

DExInEventCreateUpgrade::DExInEventCreateUpgrade(GameEventCreateUpgrade* event){ e = event; }
DExInEventCreateUpgrade::~DExInEventCreateUpgrade(){ delete e; }
Upgrade DExInEventCreateUpgrade::get_upg(){ return e->get_upg(); }
int DExInEventCreateUpgrade::get_round(){ return e->get_round(); }

DExInEventRemoveUpgrade::DExInEventRemoveUpgrade(GameEventRemoveUpgrade* event){ e = event; }
DExInEventRemoveUpgrade::~DExInEventRemoveUpgrade(){ delete e; }
int DExInEventRemoveUpgrade::get_x(){ return e->get_x(); }
int DExInEventRemoveUpgrade::get_y(){ return e->get_y(); }
int DExInEventRemoveUpgrade::get_round(){ return e->get_round(); }

DExInEventCreateFragment::DExInEventCreateFragment(GameEventCreateShot* event){ e = event; } 
DExInEventCreateFragment::~DExInEventCreateFragment(){ delete e; }
double DExInEventCreateFragment::get_x(){ return e->get_shot()->get_x(); }
double DExInEventCreateFragment::get_y(){ return e->get_shot()->get_y(); }
double DExInEventCreateFragment::get_ang(){ return e->get_shot()->get_ang(); }
long long int DExInEventCreateFragment::get_time(){ return e->get_shot()->get_ctime(); }
int DExInEventCreateFragment::get_round(){ return e->get_shot()->get_cround(); }
int DExInEventCreateFragment::get_id(){ return e->get_shot()->get_id(); }
