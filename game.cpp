#include "game.h"
#include "geom.h"

#include "game_consts.h"
#include "utils.h"

#include <stdio.h>
#include <vector>

Upgrade::Type upg_types[UPG_NUM] = {
	Upgrade::GATLING,Upgrade::LASER,Upgrade::BOMB,
};

GameEventTankDeath::GameEventTankDeath(int i){
	ind = i;
}
int GameEventTankDeath::get_ind(){
	return ind;
}
GameEventScore::GameEventScore(int i, int d){
	ind = i; diff = d;
}
int GameEventScore::get_ind(){
	return ind;
}
int GameEventScore::get_diff(){
	return diff;
}
GameEventEndGame::GameEventEndGame(const std::vector<Team*>& teams){
	for(int i = 0; i<teams.size(); i++) scores.push_back(teams[i]->get_score());
}
std::vector<int>& GameEventEndGame::get_scores(){
		return scores;
}

GameEventCreateShot::GameEventCreateShot(GenShot* s){
	shot = s;
}
GenShot* GameEventCreateShot::get_shot(){
	return shot;
}

GameEventRemoveShot::GameEventRemoveShot(GenShot* s){
	id = s->get_id();
	x = s->get_x();
	y = s->get_y();
	double spd;
	switch(s->get_type()){
	case GenShot::TYPE_REG:
		spd = STEP_SHOT; break;
	}
	vx = cos(s->get_ang())*spd;
	vy = cos(s->get_ang())*spd;
	type = s->get_type();
}
int GameEventRemoveShot::get_id(){
	return id;
}
double GameEventRemoveShot::get_x(){
	return x;
}
double GameEventRemoveShot::get_y(){
	return y;
}
double GameEventRemoveShot::get_vx(){
	return vx;
}
double GameEventRemoveShot::get_vy(){
	return vy;
}
GenShot::Type GameEventRemoveShot::get_stype(){
	return type;
}

GameEventCreateUpgrade::GameEventCreateUpgrade(Upgrade up, int round){
	u = up;
	rnd = round;
}
Upgrade GameEventCreateUpgrade::get_upg(){
	return u;
}
int GameEventCreateUpgrade::get_round(){
	return rnd;
}

GameEventRemoveUpgrade::GameEventRemoveUpgrade(Upgrade u, int round){
	x = u.x;
	y = u.y;
	rnd = round;
}
int GameEventRemoveUpgrade::get_x(){
	return x;
}
int GameEventRemoveUpgrade::get_y(){
	return y;
}
int GameEventRemoveUpgrade::get_round(){
	return rnd;
}

Team::Team(int i){
	ind = i;
	num_tot = num_alive = score = 0;
}
void Team::reset(){
	num_alive = num_tot;
}
void Team::add_score(int diff){
	score += diff;
}
int Team::get_alive(){
	return num_alive;
}
int Team::get_score(){
	return score;
}
int Team::get_ind(){
	return ind;
}

Game::Game(GameConfig& cf) : teams(), tanks(){
	set = cf.set;
	for(int i = 0; i<cf.team_num; i++) teams.push_back(new Team(i));
	for(int i = 0; i<cf.tank_num; i++) tanks.push_back(new Tank(this, i, teams[cf.team_inds[i]]));
	
	for(int i = 0, mask=1; i<UPG_NUM; i++, mask<<=1) if(cf.upg_mask & mask) upgs.push_back(upg_types[i]);
	round = NULL;
	round_num = 0;
	start_round();

	time = 0;
	
	cid = 0;
}

Game::~Game(){
	for(int i = 0; i<tanks.size(); i++) delete tanks[i];
	for(int i = 0; i<teams.size(); i++) delete teams[i];
	if(round) delete round;
}
void Game::end_game(){
	end_timer = -2;
	events.push(new GameEventEndGame(teams));
}
void Game::start_round(){
	time = 0;
	round_num++;
	end_timer = -1;
	if(round) delete round;
	for(int i = 0; i<get_team_num(); i++) get_team(i)->reset();
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
Team* Game::get_team(int i){
	return teams[i];
}
int Game::get_team_num(){
	return teams.size();
}
GameEvent* Game::get_event(){
	if(events.size()==0) return NULL;
	else{
		GameEvent* e = events.front();
		events.pop();
		return e;
	}
}

long long int Game::get_time(){
	return time;
}
void Game::step(){
	std::vector<int> dead(get_team_num(), 0);
	for(int i = 0; i<get_team_num(); i++) dead[i] += get_team(i)->get_alive(); // add all potential deaths
	round->step();
	for(int i = 0; i<get_tank_num(); i++){
		get_tank(i)->step();
	}
	for(int i = 0; i<get_team_num(); i++) dead[i] -= get_team(i)->get_alive(); // remove all those who stayed alive
	
	
	// scoring
	int cnt = 0;
	switch(set.scr_mth){
	case GameSettings::SCR_ORDER:
		for(int i = 0; i<get_team_num(); i++) if(get_team(i)->get_alive() == 0 && dead[i]>0) cnt++; // team wiped off at this step
		for(int i = 0; i<get_team_num(); i++) if(get_team(i)->get_alive()>0) add_score(i, cnt); // add to all remaining teams one point for each one that died
		break;
	case GameSettings::SCR_DEATH:
		for(int i = 0; i<get_team_num(); i++) cnt += dead[i]; // count dead
		for(int i = 0; i<get_team_num(); i++) if(get_team(i)->get_alive()>0) add_score(i, cnt-dead[i]); // add to all remaining teams one point for each tank that died
		break;
	}
	
	time++;
	
	//end round
	if(end_timer < 0){
		if(end_timer==-1){
			int alv = 0;
			for(int i = 0; i<get_team_num(); i++) if(get_team(i)->get_alive()>0) alv++;
			if(alv<=1) end_timer = END_TIME;
		}
	}
	else if(end_timer > 0) end_timer--;
	else{
		int i = -1;
		for(int j = 0; j<get_team_num(); j++) if(get_team(j)->get_alive()>0) i=j;
		//score
		switch(set.scr_mth){
		case GameSettings::SCR_LAST:
			if(i>=0) add_score(i,1);
			break;
		}
		//check end_game
		bool b_end_game = false;
		int max_scr=-1,sec_scr=-2;
		for(int j = 0; j<get_team_num(); j++){
			int scr = get_team(j)->get_score();
			if(scr > sec_scr){
				if(scr > max_scr){
					sec_scr = max_scr;
					max_scr = scr;
				}
				else sec_scr = scr;
			}
		}
		switch(set.end_mth){
		case GameSettings::END_ROUND:
			if(round_num >= set.lim) b_end_game = true;
			break;
		case GameSettings::END_SCORE:
			if(max_scr >= set.lim) b_end_game = true;
		}
		
		//check for ties
		if(max_scr - sec_scr < set.allow_dif) b_end_game = false;
		
		if(b_end_game){
			end_game();
		}
		else{
			start_round();
		}
	}
}

bool Game::can_step(){
	for(int i = 0; i<get_tank_num(); i++) if(! get_tank(i)->can_step()) return false;
	return true;
}

void Game::advance(){
	while(can_step()) step();
}
void Game::kill_tank(int i){
	tanks[i]->kill();
	events.push(new GameEventTankDeath(i));
}
void Game::add_score(int ind, int diff){
	teams[ind]->add_score(diff);
	events.push(new GameEventScore(ind, diff));
}
int Game::get_new_id(){
	return cid++;
}
int Game::get_round_num(){
	return round_num;
}
// for tests TODO: change to better value
#define UPG_MAX_TIME 61
#define UPG_MIN_TIME 60

Round::Round(Game* g){
	game=g;
	int min=10,max=20;
	maze = new Maze(rand_range(min,max), rand_range(min,max));
	
	upg_timer = rand_range(UPG_MIN_TIME, UPG_MAX_TIME);
	
	std::vector<std::pair<int,int>> tnks;
		
	for(int i = 0; i<game->get_tank_num(); i++){
		bool taken = true;
		std::pair<int,int> p;
		while(taken){
			p.first = rand_range(0, maze->get_w()-1);
			p.second = rand_range(0, maze->get_h()-1);
			taken = false;
			for(int i = 0; i<tnks.size(); i++) if(p == tnks[i]) taken = true;
		}
		tnks.push_back(p);
		
		game->get_tank(i)->reset(p.first+0.5, p.second+0.5, rand_range(0,TANK_TURN_N-1) * TANK_TURN);
		
	}
	
	maze->generate(tnks, Maze::GEN_WALL_REM);
}
Round::~Round(){
	delete maze;
	for(auto it = get_shots(); it != end_shots(); it++) delete (*it);
}
void Round::step(){
	
	for(auto it = get_shots(); it != end_shots(); it++){
		(*it)->advance();
		
		for(int i = 0; i<game->get_tank_num(); i++){
			Tank* t = game->get_tank(i);
			if(!t->is_dead() && (*it)->check_tank(t,true)){
				if(!(*it)->is_reusable()) delete_shot(*it);
				game->kill_tank(i);
			}
		}
	}
	for(auto it = shts_fd.begin(); it != shts_fd.end(); it++){
		delete (*it);
		shots.erase(*it);
	}
	shts_fd.clear();
	
	for(int i = 0; i<game->get_tank_num(); i++){
		Tank* t = game->get_tank(i);
		int x = t->get_x();
		int y = t->get_y();
		
		if(upgs.count({x,y})){
			Upgrade u = {x,y,upgs[{x,y}]};
			if(t->collide_upgrade(u)) {
				game->events.push(new GameEventRemoveUpgrade(u, game->round_num));
				upgs.erase({x,y});
			}
		}
	}	
	if(upg_timer == 0) create_upgrade();
	else upg_timer--;
}
Maze* Round::get_maze(){
		return maze;
}
void Round::add_shot(GenShot* shot){
	game->events.push(new GameEventCreateShot(shot));
	shots.insert(shot);
}
void Round::delete_shot(GenShot* shot){
	game->events.push(new GameEventRemoveShot(shot));
	shts_fd.insert(shot);
}
std::set<GenShot*>::iterator Round::get_shots(){
	return shots.begin();
}
std::set<GenShot*>::iterator Round::end_shots(){
	return shots.end();
}

void Round::create_upgrade(){
	int x = rand_range(0,maze->get_w());
	int y = rand_range(0,maze->get_h());
	
	upg_timer = rand_range(UPG_MIN_TIME, UPG_MAX_TIME);	
	
	if(upgs.count({x,y}) > 0) return;
	for(int i = 0; i<game->get_tank_num(); i++){
		int xx = game->get_tank(i)->get_x();
		int yy = game->get_tank(i)->get_y();
		
		if(x==xx && y==yy) return;
	}
	if(game->upgs.size()>0){
		upgs[{x,y}] = game->upgs[rand_range(0,game->upgs.size())];
		game->events.push(new GameEventCreateUpgrade({x,y,upgs[{x,y}]}, game->round_num));
	}
}
std::map<std::pair<int,int>,Upgrade::Type>::iterator Round::get_upgs(){
	return upgs.begin();
}
std::map<std::pair<int,int>,Upgrade::Type>::iterator Round::end_upgs(){
	return upgs.end();
}

void Round::explode(double x, double y){
	for(int i = 0; i<EXPLOSION_NUM; i++){
		add_shot(new Fragment(game, x,y));
	}
}



Tank::Tank(Game* g, int i, Team* t){
	team = t;
	team->num_tot++;
	game = g;
	x = y = ang = 0;
	dead = false;
	p_ctrl = {false,false,false,false,false};
	ind = i;
	shot_num = 0;
	state = Tank::REG;
	ctbl = NULL;
}
Tank::~Tank(){
	
}
Team* Tank::get_team(){
	return team;
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
Tank::State Tank::get_state(){
	return state;
}

#define GATLING_TIME 30
#define GATLING_INTER 10 

void Tank::step(){
	if(!is_dead()){
		double nx,ny,dp,px,py;
		
		switch(state){
		case Tank::REG:
		case Tank::GATLING:
		case Tank::LASER:
		case Tank::BOMB:
		case Tank::BOMB_SHOOT:
			double pa = ang;
			
			int turn = (ctrl.front().lt ? 1 : 0)-(ctrl.front().rt ? 1 : 0);
			ang += turn * STEP_ANG;
			
			if(check_wall_coll(nx,ny,px,py,dp)) ang = pa;
			break;
		}
		switch(state){
		case Tank::REG:
		case Tank::GATLING:
		case Tank::LASER:
		case Tank::BOMB:
		case Tank::BOMB_SHOOT:
			double prx = x, pry = y;
			
			double step = STEP_DST * ((ctrl.front().fd ? 1 : 0) - (ctrl.front().bk ? REV_RAT : 0));
			rotate_add(ang, step, 0, x, y);

			if(check_wall_coll(nx,ny,px,py,dp)) {x=prx; y=pry;}
			break;
		}
		switch(state){
		case Tank::REG:
			if(ctrl.front().sht && !p_ctrl.sht){
				if(shot_num < MAX_SHOTS) game->get_round()->add_shot(new RegShot(game, this));
			}
			break;
		case Tank::GATLING:
			if(ctrl.front().sht && !p_ctrl.sht){
				state = Tank::GATLING_WAIT;
				timer = GATLING_TIME;
			}
			break;
			
		case Tank::GATLING_WAIT:
		case Tank::GATLING_SHOOT:
			if(!ctrl.front().sht) state = Tank::REG;
			break;
		case Tank::LASER:
			if(ctrl.front().sht && !p_ctrl.sht) 
				game->get_round()->add_shot(new LaserShot(game,this));
			break;
		case Tank::BOMB:
			if(ctrl.front().sht && !p_ctrl.sht){
				BombShot* sht = new BombShot(game,this);
				ctbl = sht;
				game->get_round()->add_shot(sht);
			}
			break;
		}
	}
	
	if(timer > 0) timer--;
	else{
		switch(state){
		case Tank::GATLING_WAIT:
			state = GATLING_SHOOT;
		case Tank::GATLING_SHOOT:
			timer = GATLING_INTER;
			game->get_round()->add_shot(new GatShot(game, this));
			break;
		}
	}
	
	if(ctbl) ctbl->set_ctrl(ctrl.front());
	
	p_ctrl = ctrl.front();
	ctrl.pop();
}
void Tank::push_control(ControlState st){
	ctrl.push(st);
}
void Tank::clear_control(){
	if(ctrl.size()>0) p_ctrl = ctrl.back();
	while(ctrl.size()>0) ctrl.pop();
}
bool Tank::can_step(){
	return ctrl.size()>0;
}

bool Tank::check_wall_coll(double& nx, double& ny, double& px, double& py, double& dp){
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
int Tank::get_ind(){
	return ind;
}

void Tank::reset(double xx, double yy, double a){
	x = xx; y=yy; ang=a;
	clear_control();
	dead = false;
	state = Tank::REG;
	ctbl = NULL;
}
void Tank::kill(){
	dead = true;
	team->num_alive--;
}

ControlState Tank::get_ctrl(){
	return p_ctrl;
}

bool Tank::check_upg(Upgrade u){
	double txs[4],tys[4],uxs[4],uys[4];
	
	gen_rot_rect(x,y,TANK_H,TANK_W,ang, txs,tys);
	gen_rot_rect(u.x+0.5, u.y+0.5,UPG_SIZE, UPG_SIZE, DEG2RAD(UPG_ANG), uxs, uys);
	
	double nx,ny,dp,px,py;
	
	return poly_coll(txs,tys,4,uxs,uys,4,nx,ny,dp,px,py);
}

std::pair<Upgrade::Type, Tank::State> upg2stt_a[UPG_NUM]={
	{Upgrade::GATLING,Tank::GATLING},
	{Upgrade::LASER, Tank::LASER},
	{Upgrade::BOMB, Tank::BOMB},
};

std::map<Upgrade::Type, Tank::State> upg2stt(upg2stt_a, upg2stt_a+UPG_NUM);

bool Tank::collide_upgrade(Upgrade u){
	if(is_dead() || state != Tank::REG) return false;
	bool ret = check_upg(u);
	if(ret){
		state = upg2stt[u.type];
	}
	return ret;
}



GenShot::GenShot(Game* g, Tank* t){
	game = g;
	ctime = g->get_time();
	cround = g->get_round_num();
	tank = t;
	id = game->get_new_id();
}
GenShot::~GenShot(){}
Tank* GenShot::get_tank(){
	return tank;
}
Game* GenShot::get_game(){
	return game;
}
int GenShot::get_id(){
	return id;
}


bool GenShot::is_reusable(){
	return false;
}

long long int GenShot::get_ctime(){
	return ctime;
}
int GenShot::get_cround(){
	return cround;
}


Shot::Shot(Game* game, Tank* tank, double div, double spd) : GenShot(game, tank){
	vx=vy=0;
	x = tank->get_x();
	y = tank->get_y();
	rotate_add(tank->get_ang(), CANNON_L,0, x,y);
	rotate_add(tank->get_ang() + rand_range(-5,6)*div/5, spd,0, vx,vy);
	
	tm = 0;
	col_t = 0;
	found = false;
	out_of_tank = false;
}
double Shot::check_wall(){
	double xx = x + col_t*vx,yy = y + col_t*vy;
	int ix = xx;
	int iy = yy;
	int ixx = vx>0 ? 1 : 0;
	int iyy = vy>0 ? 1 : 0;
	int idx = vx>0 ? 1 : -1;
	int idy = vy>0 ? 1 : -1;
	int vsg = idx*idy;
	double wxs[4],wys[4],nnx,nny;
	double t = -1;
	while(abs(ix - (int)xx)+abs(iy - (int)yy) <= 2*sqrt(vx*vx+vy*vy)*(tm-col_t+1)){
		double tt;
		for(int i = -1; i<=1; i++){
			for(int j = -1; j<1; j++){
				gen_rect(ix+i-WALL_THK, iy+j-WALL_THK + 1, 2*WALL_THK + 1, 2*WALL_THK,wxs,wys);
				if(get_game()->get_round()->get_maze()->hwall(ix+i, iy+j)){
					tt = circ_poly_coltime(xx,yy,vx,vy,get_r(), wxs,wys,4,nnx,nny);
					if(tt+col_t <= tm && tt >= 0 && (tt < t || t < 0)){
						found = true;
						t = tt;
						nx = nnx; ny = nny;
					}
				}
				gen_rect(ix+j-WALL_THK + 1, iy+i-WALL_THK, 2*WALL_THK, 2*WALL_THK + 1,wxs,wys);
				if(get_game()->get_round()->get_maze()->vwall(ix+j, iy+i)){
					tt = circ_poly_coltime(xx,yy,vx,vy,get_r(), wxs,wys,4,nnx,nny);
					if(tt+col_t < tm && tt >= 0 && (tt < t || t < 0)){
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
void Shot::reflect(){
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
		out_of_tank = true;
	}
	
	found = false;
	
	// find maze collision
	double ctm = check_wall();
	
	double tx,ty;
	double txs[4],tys[4];
	
	for(int i = 0; i < get_game()->get_tank_num(); i++){
		Tank* tn = get_game()->get_tank(i);
		if(tn->is_dead()) continue;
		gen_rot_rect(tn->get_x(), tn->get_y(), TANK_W, TANK_H, tn->get_ang(), txs, tys);
		double ctt = circ_poly_coltime(col_x, col_y, vx,vy, get_r(), txs,tys,4, tx,ty);
		if(ctt >= 0 && (found ? (ctt<=ctm) : (ctt+col_t<=tm))){
			hits.insert(std::pair<Tank*, double>({tn,col_t+ctt}));
			double cx = col_x + vx*ctt, cy = col_y+vy*ctt;
			colls.push_back({cx,cy});
		}
	}
	gen_rot_rect(get_tank()->get_x(), get_tank()->get_y(), TANK_W, TANK_H, get_tank()->get_ang(), txs, tys);
	if(	out_of_tank && 
		!get_tank()->is_dead() &&
		circ_poly_colcheck(col_x, col_y, get_r(), txs,tys,4)){
			
		hits.insert(std::pair<Tank*, double>({get_tank(),col_t}));
		colls.push_back({col_x, col_y});
	}
	
	if(found) col_t += ctm;
	
	else{
		col_t = (double)tm;
		if(hits.size() == 0) colls.push_back({get_x(), get_y()});
	}
}
bool Shot::check_tank(Tank* t, bool igm){
	if(t == get_tank() && !out_of_tank && !igm) return false;
	return (hits.count(t)>0 && hits[t]<=tm);
}
void Shot::advance(){
	colls.clear();
	colls.push_back({get_x(), get_y()});
	tm++;
	while(col_t < tm && hits.size()==0) {
		reflect();
	}
	if(!check_tank(get_tank(),false)) out_of_tank = false;
	if(tm > get_ttl()) get_game()->get_round()->delete_shot(this);
}
double Shot::get_ang(){
	return atan2(vy,vx);
}
double Shot::get_x(){
	return x + tm*vx;
}
double Shot::get_y(){
	return y + tm*vy;
}

double Shot::get_vx(){
	return vx;
}
double Shot::get_vy(){
	return vy;
}

std::vector<std::pair<double,double>>& Shot::get_colls(){
	return colls;
}

RegShot::RegShot(Game* game, Tank* tank) : Shot(game, tank, 0, STEP_SHOT){
	get_tank()->shot_num++;
}
RegShot::~RegShot(){
	get_tank()->shot_num--;
}
double RegShot::get_r(){
	return SHOT_R;
}
int RegShot::get_ttl(){
	return SHOT_TTL;
}
GenShot::Type RegShot::get_type(){
	return GenShot::TYPE_REG;
}

GatShot::GatShot(Game* game, Tank* tank) : Shot(game, tank, GATLING_DIV, STEP_GATLING){
}
GatShot::~GatShot(){
}
double GatShot::get_r(){
	return GATLING_R;
}
int GatShot::get_ttl(){
	return GATLING_TTL;
}
GenShot::Type GatShot::get_type(){
	return GenShot::TYPE_GATLING;
}

LaserShot::LaserShot(Game* game, Tank* tank) : Shot(game, tank, 0, STEP_LASER){
	get_tank()->state = Tank::LASER_SHOOT;
}
LaserShot::~LaserShot(){
	get_tank()->state = Tank::REG;
}
double LaserShot::get_r(){
	return LASER_R;
}
int LaserShot::get_ttl(){
	return LASER_TTL;
}
GenShot::Type LaserShot::get_type(){
	return GenShot::TYPE_LASER;
}

BombShot::BombShot(Game* game, Tank* tank) : Shot(game, tank, 0, STEP_BOMB){
	get_tank()->state = Tank::BOMB_SHOOT;
	prs = true;
}
BombShot::~BombShot(){
	get_tank()->ctbl = NULL;
	get_tank()->state = Tank::REG;
	get_game()->get_round()->explode(get_x(), get_y());
}
double BombShot::get_r(){
	return BOMB_R;
}
int BombShot::get_ttl(){
	return BOMB_TTL;
}
GenShot::Type BombShot::get_type(){
	return GenShot::TYPE_BOMB;
}
void BombShot::set_ctrl(ControlState ctrl){
	if(!ctrl.sht) prs = false;
	else if(!prs) get_game()->get_round()->delete_shot(this);
}

Fragment::Fragment(Game* game, double xx,double yy) : GenShot(game, NULL){
	x=xx;
	y=yy;
	ang = DEG2RAD(rand()%360);
	time = FRAG_TTL;
	
	tot_dst = 1.0;
	check_wall();
}
Fragment::~Fragment(){
	
}

void Fragment::check_wall(){
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

bool Fragment::check_tank(Tank* t, bool ignore_me){
	
	double txs[4],tys[4];
	double x1 = get_x(time+1),x2 = get_x();
	double y1 = get_y(time+1),y2 = get_y();
	
	gen_rot_rect(t->get_x(), t->get_y(), TANK_W, TANK_H, t->get_ang(), txs, tys);
	
	double nx,ny;
	double tt = circ_poly_coltime(x1,y1,x2-x1,y2-y1,0.0,txs,tys,4,nx,ny);
	return tt >= 0 && tt<1.0;
}
void Fragment::advance(){
	if(time >0) time--;
	else get_game()->get_round()->delete_shot(this);
}
GenShot::Type Fragment::get_type(){
	return GenShot::TYPE_FRAGMENT;
}
	
double Fragment::get_ang(){
	return ang;
}
double Fragment::get_x(){
	return get_x(time);
}
double Fragment::get_y(){
	return get_y(time);
}
double Fragment::get_x(double t){
	return x + cos(ang) * get_dst(t);
}
double Fragment::get_y(double t){
	return y + sin(ang) * get_dst(t);
}
double Fragment::get_dst(){
	return get_dst(time);
}
double Fragment::get_dst(double t){
	return FRAG_DST*std::min(1.0 - pow(t / (double)FRAG_TTL, 2.6),tot_dst);
}
	
bool Fragment::is_reusable(){
	return true;
}
double Fragment::get_t(){
	return time / (double)FRAG_TTL;
}