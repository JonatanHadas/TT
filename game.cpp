#include "game.h"
#include "geom.h"

#include "game_consts.h"
#include "utils.h"

#include <stdio.h>
#include <vector>

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

Game::Game(GameConfig& cf){
	set = cf.set;
	for(int i = 0; i<cf.team_num; i++) teams.push_back(new Team(i));
	for(int i = 0; i<cf.tank_num; i++) tanks.push_back(new Tank(this, i, teams[cf.team_inds[i]]));
	round = NULL;
	round_num = 0;
	start_round();
	
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

Round::Round(Game* g){
	game=g;
	int min=10,max=20;
	maze = new Maze(rand_range(min,max), rand_range(min,max));
	
	
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

Tank::Tank(Game* g, int i, Team* t){
	team = t;
	team->num_tot++;
	game = g;
	x = y = ang = 0;
	dead = false;
	p_ctrl = {false,false,false,false,false};
	ind = i;
	shot_num = 0;
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
void Tank::step(){
	if(!is_dead()){
		double nx,ny,dp,px,py;
		
		double pa = ang;
		
		int turn = (ctrl.back().lt ? 1 : 0)-(ctrl.back().rt ? 1 : 0);
		ang += turn * STEP_ANG;
		
		if(check_wall_coll(nx,ny,px,py,dp)) ang = pa;
		
		double prx = x, pry = y;
		
		double step = STEP_DST * ((ctrl.back().fd ? 1 : 0) - (ctrl.back().bk ? REV_RAT : 0));
		rotate_add(ang, step, 0, x, y);

		if(check_wall_coll(nx,ny,px,py,dp)) {x=prx; y=pry;}
		
		
		if(ctrl.back().sht && !p_ctrl.sht){
			if(shot_num < MAX_SHOTS) game->get_round()->add_shot(new RegShot(game, this));
		}
	}
	
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
}
void Tank::kill(){
	dead = true;
	team->num_alive--;
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
	int iyy = vy<0 ? 1 : 0;
	int idx = vx>0 ? 1 : -1;
	int idy = vy<0 ? 1 : -1;
	int vsg = -idx*idy;
	double wxs[4],wys[4],nnx,nny;
	double t = -1;
	while(abs(ix - (int)xx)+abs(iy - (int)yy) <= sqrt(vx*vx+vy*vy)*(tm-col_t+1)){
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
