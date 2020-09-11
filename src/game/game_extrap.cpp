#include "game_extrap.h"

#include "game_consts.h"

#include "../utils/geom.h"

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
	case GenShot::TYPE_GATLING:
		spd = STEP_GATLING;
		break;
	case GenShot::TYPE_LASER:
		break;
	case GenShot::TYPE_BOMB:
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

ExEventCreateUpgrade::ExEventCreateUpgrade(Upgrade up){
	u = up;
}
Upgrade ExEventCreateUpgrade::get_upg(){
	return u;
}

ExEventRemoveUpgrade::ExEventRemoveUpgrade(int ux, int uy){
	x=ux; y=ux;
}
int ExEventRemoveUpgrade::get_x(){
	return x;
}
int ExEventRemoveUpgrade::get_y(){
	return y;
}

ExEventCreateMine::ExEventCreateMine(MineExtrap* mine){
	m = mine;
}
MineExtrap* ExEventCreateMine::get_mine(){
	return m;
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
long long int GameExtrap::get_time(){
	return time;
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
			events.push(new ExEventEndGame(this));
			break;
		case ExInEvent::TYPE_SHT_CRT:
			create_shot((ExInEventCreateShot*)e);
			break;
		case ExInEvent::TYPE_SHT_RMV:
			remove_shot(((ExInEventRemoveShot*)e)->get_id());
			break;
		case ExInEvent::TYPE_FRG_CRT:
			create_frag((ExInEventCreateFragment*)e);
			break;
		case ExInEvent::TYPE_DTR_CRT:
			create_dtry((ExInEventCreateDeathRay*)e);
			break;
		case ExInEvent::TYPE_UPG_CRT:
			create_upgrade((ExInEventCreateUpgrade*)e);
			break;
		case ExInEvent::TYPE_UPG_RMV:
			remove_upgrade((ExInEventRemoveUpgrade*)e);
			break;
		case ExInEvent::TYPE_MIN_CRT:
			create_mine((ExInEventCreateMine*)e);
			break;
		case ExInEvent::TYPE_MIN_RMV:
			remove_mine((ExInEventRemoveMine*)e);
			break;
		case ExInEvent::TYPE_MIN_ACT:
			activate_mine((ExInEventActivateMine*)e);
			break;
		case ExInEvent::TYPE_MIN_STR:
			start_mine((ExInEventStartMine*)e);
			break;
		}
		delete e;
	}
	round->advance();
	for(int i = 0; i<get_tank_num(); i++) get_tank(i)->advance();
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
void GameExtrap::create_shot(ExInEventCreateShot* e){
	if(e->get_round() == round_num){
		ShotExtrap* shot = new ShotExtrap(this,e);
		round->add_shot(shot);
		events.push(new ExEventCreateShot(shot));
	}
}
void GameExtrap::remove_shot(int id){
	if(round->get_shot(id)){
		events.push(new ExEventRemoveShot(round->get_shot(id)));
		round->del_shot(id);
	}
}
void GameExtrap::create_frag(ExInEventCreateFragment* e){
	if(e->get_round() == round_num){
		FragmentExtrap* frag = new FragmentExtrap(this,e);
		round->add_shot(frag);
		events.push(new ExEventCreateShot(frag));
	}
}
void GameExtrap::create_dtry(ExInEventCreateDeathRay* e){
	if(e->get_round() == round_num){
		DeathRayExtrap* dtry = new DeathRayExtrap(this,e);
		round->add_shot(dtry);
		events.push(new ExEventCreateShot(dtry));
	}
}
void GameExtrap::create_upgrade(ExInEventCreateUpgrade* e){
	if(e->get_round() == round_num){
		Upgrade u = e->get_upg();
		round->add_upg(u);
		events.push(new ExEventCreateUpgrade(u));
	}
}
void GameExtrap::remove_upgrade(ExInEventRemoveUpgrade* e){
	if(e->get_round() == round_num && round->has_upg(e->get_x(),e->get_y())){
		round->del_upg(e->get_x(), e->get_y());
		events.push(new ExEventRemoveUpgrade(e->get_x(), e->get_y()));
	}
}
void GameExtrap::create_mine(ExInEventCreateMine* e){
	if(e->get_round() == round_num){
		MineExtrap* mine = new MineExtrap(this, e);
		round->add_mine(mine);
		events.push(new ExEventCreateMine(mine));
	}
}
void GameExtrap::remove_mine(ExInEventRemoveMine* e){
	round->del_mine(e->get_id());
}
void GameExtrap::activate_mine(ExInEventActivateMine* e){
	round->activate_mine(e->get_id());
}
void GameExtrap::start_mine(ExInEventStartMine* e){
	round->start_mine(e->get_id());
}

RoundExtrap::RoundExtrap(GameExtrap* g,Maze* m){
	game = g;
	maze = m;
}
Maze* RoundExtrap::get_maze(){
	return maze;
}
void RoundExtrap::advance(){
	for(auto it = get_shots(); it != end_shots(); it++){
		(*it).second->advance();
	}
}
std::map<int, GenShotExtrap*>::iterator RoundExtrap::get_shots(){
	return shots.begin();
}
std::map<int, GenShotExtrap*>::iterator RoundExtrap::end_shots(){
	return shots.end();
}
std::map<int, MineExtrap*>::iterator RoundExtrap::get_mines(){
	return mines.begin();
}
std::map<int, MineExtrap*>::iterator RoundExtrap::end_mines(){
	return mines.end();
}
std::map<std::pair<int,int>,Upgrade::Type>::iterator RoundExtrap::get_upgs(){
	return upgs.begin();
}
std::map<std::pair<int,int>,Upgrade::Type>::iterator RoundExtrap::end_upgs(){
	return upgs.end();
}

GenShotExtrap* RoundExtrap::get_shot(int id){
	return shots.count(id)>0 ? shots[id] : NULL;
}
void RoundExtrap::add_shot(GenShotExtrap* shot){
	shots.insert({shot->get_id(), shot});
}
void RoundExtrap::del_shot(int id){
	shots.erase(id);
}
void RoundExtrap::add_mine(MineExtrap* mine){
	mines.insert({mine->get_id(), mine});
}
void RoundExtrap::del_mine(int id){
	if(mines.count(id)>0) mines.erase(id);
}
void RoundExtrap::activate_mine(int id){
	if(mines.count(id)>0) mines[id]->active = true;
}
void RoundExtrap::start_mine(int id){
	if(mines.count(id)>0) mines[id]->started = true;
}
void RoundExtrap::add_upg(Upgrade u){
	upgs[{u.x,u.y}] = u.type;
}
void RoundExtrap::del_upg(int x, int y){
	upgs.erase({x,y});
}
bool RoundExtrap::has_upg(int x, int y){
	return upgs.count({x,y})>0;
}

TankExtrap::TankExtrap(GameExtrap* g,int i, TeamExtrap* t){
	game = g;
	team = t;
	ind = i;
	state = Tank::REG;
	missile = NULL;
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
		t = b_t;
		b_t = e->get_time();
		while(t < b_t){
			p_ctrl = ctrl.front();
			ctrl.pop_front();
			t++;
		}
		x = e->get_x();
		y = e->get_y();
		ang = e->get_ang();
		state = e->get_state();
		
		if(e->get_state() == Tank::WIFI_SHOOT || e->get_state() == Tank::MISSILE_SHOOT){
			if(missile && missile->get_id() != e->get_missile_id()){
				game->get_round()->del_shot(missile->get_id());
				missile = NULL;
			}
			if(!missile){
				GenShot::Type tp;
				int id = e->get_missile_id();
				switch(state){
				case Tank::WIFI_SHOOT:
					tp = GenShot::TYPE_WIFI;
					break;
				case Tank::MISSILE_SHOOT:
					tp = GenShot::TYPE_MISSILE;
					break;
				}
				missile = new MissileExtrap(game, this, tp, id);
				game->get_round()->add_shot(missile);
			}
			missile->update(e);
		}
		else if(missile){
			game->get_round()->del_shot(missile->get_id());
			missile = NULL;
		}
	}
}
void TankExtrap::reset(double xx, double yy, double a){
	x = xx;
	y = yy;
	ang = a;
	t = b_t = 0;
	dead = false;
	p_ctrl = {false,false,false,false,false};
	ctrl.clear();
	state = Tank::REG;
	missile = NULL;
}
int TankExtrap::get_ind(){
	return ind;
}

bool TankExtrap::is_dead(){
	return dead;
}
Tank::State TankExtrap::get_state(){
	return state;
}
void TankExtrap::kill(){
	dead = true;
}

void TankExtrap::push_control(ControlState ct){
	game->push_ctrl(ct, ind);
	ctrl.push_back(ct);
}

bool TankExtrap::check_wall_coll(double& nx, double& ny, double& px, double& py, double& dp){
	double txs[4],tys[4],wxs[4],wys[4];
	int ix = x, iy = y;
	gen_rot_rect(x,y,TANK_H, TANK_W, ang, txs,tys);
	for(int i = -1; i<=1; i++){
		for(int j = -1; j<1; j++){
			gen_rect(ix+i-WALL_THK, iy+j-WALL_THK + 1, 2*WALL_THK + 1, 2*WALL_THK,wxs,wys);
			if(game->get_round()->get_maze()->hwall(ix+i, iy+j) && poly_coll(txs,tys,4,wxs,wys,4,nx,ny,dp,px,py)){
				return true;
			}
			gen_rect(ix+j-WALL_THK + 1, iy+i-WALL_THK, 2*WALL_THK, 2*WALL_THK + 1,wxs,wys);
			if(game->get_round()->get_maze()->vwall(ix+j, iy+i) && poly_coll(txs,tys,4,wxs,wys,4,nx,ny,dp,px,py)){
				return true;
			}
		}
	}
	return false;
}
void TankExtrap::advance(){
	auto it = ctrl.rend();
	ControlState cpc = p_ctrl;
	while(t < game->get_time()){
		ControlState cur = it==ctrl.rbegin() ? cpc : (cpc=*(it++));
		double pa;
		int turn;
		if(!is_dead()){
			double nx,ny,dp,px,py;
			switch(state){
			case Tank::REG:
			case Tank::GATLING:
			case Tank::LASER:
			case Tank::BOMB:
			case Tank::BOMB_SHOOT:
			case Tank::DEATH_RAY:
			case Tank::WIFI:
			case Tank::MISSILE:
			case Tank::MISSILE_SHOOT:
			
				pa = ang;
				
				turn = (cur.lt ? 1 : 0)-(cur.rt ? 1 : 0);
				ang += turn * STEP_ANG;
				
				if(check_wall_coll(nx,ny,px,py,dp)) ang = pa;
				break;
			case Tank::WIFI_SHOOT:
				missile->rt = cur.rt;
				missile->lt = cur.lt;
				break;
			}
			switch(state){
			case Tank::REG:
			case Tank::GATLING:
			case Tank::LASER:
			case Tank::BOMB:
			case Tank::BOMB_SHOOT:
			case Tank::DEATH_RAY:
			case Tank::WIFI:
			case Tank::MISSILE:
			case Tank::MISSILE_SHOOT:
			
				double prx = x, pry = y;
				
				double step = STEP_DST * ((cur.fd ? 1 : 0) - (cur.bk ? REV_RAT : 0));
				rotate_add(ang, step, 0, x, y);

				if(check_wall_coll(nx,ny,px,py,dp)) {x=prx; y=pry;}		
				break;
			}
		}
		if(missile) missile->step();
	}
};

GenShotExtrap::GenShotExtrap(GameExtrap* g, TankExtrap* tk, int i, GenShot::Type tp){
	game = g; tank = tk; id = i; type = tp;
}

GameExtrap* GenShotExtrap::get_game(){ return game; }
TankExtrap* GenShotExtrap::get_tank(){ return tank; }
int GenShotExtrap::get_id(){ return id; }
GenShot::Type GenShotExtrap::get_type(){ return type; }

ShotExtrap::ShotExtrap(GameExtrap* game, ExInEventCreateShot* e) : GenShotExtrap(game, game->get_tank(e->get_tank_ind()), e->get_id(), e->get_stype()){
	x = e->get_x();
	y = e->get_y();
	vx = e->get_vx();
	vy = e->get_vy();
	ctime = e->get_time();
	
	found = false;
	col_t = 0;
		
	advance();
}
double ShotExtrap::get_r(){
	switch(get_type()){
	case GenShot::TYPE_REG:
		return SHOT_R;
	case GenShot::TYPE_GATLING:
		return GATLING_R;
	case GenShot::TYPE_LASER:
		return LASER_R;
	case GenShot::TYPE_BOMB:
		return BOMB_R;
	}

}
double ShotExtrap::get_x(){
	return (get_game()->get_time() - ctime)*vx + x;
}
double ShotExtrap::get_y(){
	return (get_game()->get_time() - ctime)*vy + y;
}
double ShotExtrap::get_ang(){
	return atan2(vy,vx);
}

std::vector<std::pair<double, double>>& ShotExtrap::get_colls(){
	return colls;
}
double ShotExtrap::check_wall(){
	double xx = x + col_t*vx,yy = y + col_t*vy;
	int ix = xx;
	int iy = yy;
	int ixx = vx>0 ? 1 : 0;
	int iyy = vy<0 ? 1 : 0;
	int idx = vx>0 ? 1 : -1;
	int idy = vy<0 ? 1 : -1;
	int vsg = -idx*idy;
	double wxs[4],wys[4],nnx,nny;
	double t = -1;
	while(abs(ix - (int)xx)+abs(iy - (int)yy) <= sqrt(vx*vx+vy*vy)*(get_game()->get_time()-ctime-col_t+1)){
		double tt;
		for(int i = -1; i<=1; i++){
			for(int j = -1; j<1; j++){
				gen_rect(ix+i-WALL_THK, iy+j-WALL_THK + 1, 2*WALL_THK + 1, 2*WALL_THK,wxs,wys);
				if(get_game()->get_round()->get_maze()->hwall(ix+i, iy+j)){
					tt = circ_poly_coltime(xx,yy,vx,vy,get_r(), wxs,wys,4,nnx,nny);
					if(tt+col_t <= get_game()->get_time()-ctime && tt >= 0 && (tt < t || t < 0)){
						found = true;
						t = tt;
						nx = nnx; ny = nny;
					}
				}
				gen_rect(ix+j-WALL_THK + 1, iy+i-WALL_THK, 2*WALL_THK, 2*WALL_THK + 1,wxs,wys);
				if(get_game()->get_round()->get_maze()->vwall(ix+j, iy+i)){
					tt = circ_poly_coltime(xx,yy,vx,vy,get_r(), wxs,wys,4,nnx,nny);
					if(tt+col_t < get_game()->get_time()-ctime && tt >= 0 && (tt < t || t < 0)){
						found = true;
						t = tt;
						nx = nnx; ny = nny;
					}
				}
			}
		}
		if( vsg * leftness(ix+ixx,iy+iyy,x,y,x+vx,y+vy) > 0) iy += idy;
		else ix += idx;
	}
	return t;
}
void ShotExtrap::reflect(){
	double col_x = x+col_t*vx;
	double col_y = y+col_t*vy;
	
	
	// reflect
	colls.push_back({col_x, col_y});
	if(found){
		double old_vx = vx, old_vy = vy;
		
		if(nx*vx<0) vx = -vx;
		if(ny*vy<0) vy = -vy;
		
		x = col_x - col_t*vx;
		y = col_y - col_t*vy;
	}
	
	found = false;
	
	// find maze collision
	double ctm = check_wall();
	
	if(found) col_t += ctm;
	
	else{
		col_t = (double)(get_game()->get_time()-ctime);
		colls.push_back({get_x(), get_y()});
	}
}
void ShotExtrap::advance(){
	colls.clear();
	colls.push_back({get_x(), get_y()});
	while(col_t < get_game()->get_time()-ctime) {
		reflect();
	}
}

FragmentExtrap::FragmentExtrap(GameExtrap* game, ExInEventCreateFragment* e) : GenShotExtrap(game, NULL,e->get_id(),GenShot::TYPE_FRAGMENT) {
	x = e->get_x();
	y = e->get_y();
	ang = e->get_ang();
	
	ctime = e->get_time();
	
	tot_dst = 1.0;
		
	check_wall();
}
double FragmentExtrap::get_x(){
	return get_x(get_time());
}
double FragmentExtrap::get_y(){
	return get_y(get_time());
}
double FragmentExtrap::get_x(double t){
	return x + cos(ang)*get_dst(t);
}
double FragmentExtrap::get_y(double t){
	return y + sin(ang)*get_dst(t);
}
double FragmentExtrap::get_dst(double t){
	return FRAG_DST*std::min(1.0 - pow(t / (double)FRAG_TTL, 2.6),tot_dst);
}
double FragmentExtrap::get_dst(){
	return get_dst(get_time());
}

double FragmentExtrap::get_ang(){
	return ang;
}

double FragmentExtrap::get_t(){
	return get_time()/(double)FRAG_TTL;
}

long long int FragmentExtrap::get_time(){
	long long int t = ctime + FRAG_TTL - get_game()->get_time();
	return t<0 ? 0 : t;
}


void FragmentExtrap::check_wall(){
	double xx = x,yy = y;
	double x2 = get_x(0), y2 = get_y(0);
	double vx = x2-xx, vy=y2-yy;
	int ix = xx;
	int iy = yy;
	int ixx = vx>0 ? 1 : 0;
	int iyy = vy>0 ? 1 : 0;
	int idx = vx>0 ? 1 : -1;
	int idy = vy>0 ? 1 : -1;
	int vsg = idx*idy;
	double wxs[4],wys[4],nnx,nny;
	bool col = false;
	while(abs(ix - (int)xx)<= FRAG_DST && abs(iy - (int)yy) <= FRAG_DST && !col){
		double tt;
		for(int i = -1; i<=1; i++){
			for(int j = -1; j<1; j++){
				gen_rect(ix+i-WALL_THK, iy+j-WALL_THK + 1, 2*WALL_THK + 1, 2*WALL_THK,wxs,wys);
				if(get_game()->get_round()->get_maze()->hwall(ix+i, iy+j)){
					double nx,ny;
					double d = circ_poly_coltime(xx,yy,vx,vy,0.0,wxs,wys,4,nx,ny);
					if(d >= 0){
						col = true;
						if(d < tot_dst) tot_dst = d;
					}
				}
				gen_rect(ix+j-WALL_THK + 1, iy+i-WALL_THK, 2*WALL_THK, 2*WALL_THK + 1,wxs,wys);
				if(get_game()->get_round()->get_maze()->vwall(ix+j, iy+i)){
					double nx,ny;
					double d = circ_poly_coltime(xx,yy,vx,vy,0.0,wxs,wys,4,nx,ny);
					if(d >= 0){
						if(d < tot_dst) tot_dst = d;
					}
				}
			}
		}
		if( vsg * leftness(ix+ixx,iy+iyy,x,y,x+vx,y+vy) > 0) iy += idy;
		else ix += idx;
	}
}
void FragmentExtrap::advance(){}

DeathRayExtrap::DeathRayExtrap(GameExtrap* game, ExInEventCreateDeathRay* e) : GenShotExtrap(game, game->get_tank(e->get_tank_ind()), e->get_id(), GenShot::TYPE_DEATH_RAY){
	for(int i = 0; i<e->get_point_num(); i++){
		ps.push_back(e->get_point(i));
	}
}
int DeathRayExtrap::get_point_num(){
	return ps.size();
}
double DeathRayExtrap::get_x(int i){
	return ps[i].first;
}
double DeathRayExtrap::get_y(int i){
	return ps[i].second;
}

double DeathRayExtrap::get_x(){
	return get_tank()->get_x();
}
double DeathRayExtrap::get_y(){
	return get_tank()->get_y();
}
double DeathRayExtrap::get_ang(){
	return get_tank()->get_ang();
}
void DeathRayExtrap::advance(){}


MissileExtrap::MissileExtrap(GameExtrap* g, TankExtrap* t, GenShot::Type tp, int id) : GenShotExtrap(g, t, id, tp){
	target = NULL;
};
	
double MissileExtrap::get_x(){
	return x;
}
double MissileExtrap::get_y(){
	return y;
}
double MissileExtrap::get_ang(){
	return atan2(vy,vx);
}
	
void MissileExtrap::step(){
	x += vx;
	y += vy;
	
	double vvx=vx,vvy=vy;
	double turn = MISSILE_TURN * ((lt ? 1 : 0)-(rt ? 1 : 0));
	vx=vy=0;
	rotate_add(turn, vvx,vvy,vx,vy);
	
	double nx,ny;
	if(check_wall(nx,ny)){
		if(nx*vx>0) vx = -vx;
		if(ny*vy>0) vy = -vy;
		
	}
	
}

bool MissileExtrap::check_wall(double& nx, double& ny){
	double mxs[4],mys[4],wxs[4],wys[4];
	double px,py,dp;
	
	int ix = x;
	int iy = y;
	gen_rot_rect(x,y,MISSILE_W,MISSILE_L,get_ang(),mxs,mys);
	for(int i = -1; i<=1; i++){
		for(int j = -1; j<1; j++){
			gen_rect(ix+i-WALL_THK, iy+j-WALL_THK + 1, 2*WALL_THK + 1, 2*WALL_THK,wxs,wys);
			if(get_game()->get_round()->get_maze()->hwall(ix+i, iy+j)){
				if(poly_coll(mxs,mys,4,wxs,wys,4,nx,ny,dp,px,py)) return true;
			}
			gen_rect(ix+j-WALL_THK + 1, iy+i-WALL_THK, 2*WALL_THK, 2*WALL_THK + 1,wxs,wys);
			if(get_game()->get_round()->get_maze()->vwall(ix+j, iy+i)){
				if(poly_coll(mxs,mys,4,wxs,wys,4,nx,ny,dp,px,py)) return true;
			}
		}
	}
	return false;
}
void MissileExtrap::advance(){}
void MissileExtrap::update(ExInEventTankUpdate* e){
	x = e->get_missile_x();
	y = e->get_missile_y();
	double ang = e->get_missile_ang();
	vx = STEP_MISSILE * cos(ang);
	vy = STEP_MISSILE * sin(ang);
	rt = e->get_missile_rt();
	lt = e->get_missile_lt();
	target = get_game()->get_tank(e->get_missile_target());
}
TankExtrap* MissileExtrap::get_target(){
	return target;
}

MineExtrap::MineExtrap(GameExtrap* g, ExInEventCreateMine* e){
	game = g;
	x = e->get_x();
	y = e->get_y();
	ang = e->get_ang();
	id = e->get_id();
	started = active = false;
	
	tank = g->get_tank(e->get_tank_ind());
}
double MineExtrap::get_x(){
	return x;
}
double MineExtrap::get_y(){
	return y;
}
double MineExtrap::get_ang(){
	return ang;
}

int MineExtrap::get_id(){
	return id;
}

bool MineExtrap::get_started(){
	return started;
}
bool MineExtrap::get_active(){
	return active;
}

TankExtrap* MineExtrap::get_tank(){
	return tank;
}
