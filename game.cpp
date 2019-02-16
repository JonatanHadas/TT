#include "game.h"
#include "geom.h"

#include "game_consts.h"
#include "utils.h"

#include <stdio.h>

Game::Game(int tank_num){
	for(int i = 0; i<tank_num; i++) tanks.push_back(new Tank(this));
	round = NULL;
	start_round();
	
	time = 0;
}

Game::~Game(){
	for(int i = 0; i<tanks.size(); i++) delete tanks[i];
	if(round) delete round;
}
void Game::start_round(){
	for(int i = 0; i<get_tank_num(); i++) get_tank(i)->clear_control();
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

long long int Game::get_time(){
	return time;
}
void Game::step(){
	round->step();
	for(int i = 0; i<get_tank_num(); i++){
		get_tank(i)->step();
	}
	time++;
}

bool Game::can_step(){
	for(int i = 0; i<get_tank_num(); i++) if(! get_tank(i)->can_step()) return false;
	return true;
}

void Game::advance(){
	while(can_step()) step();
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
		
		game->get_tank(i)->x = p.first+0.5;
		game->get_tank(i)->y = p.second+0.5;
		game->get_tank(i)->ang = rand_range(0,TANK_TURN_N-1) * TANK_TURN;
	}
	
	maze->generate(tnks, Maze::GEN_WALL_REM);
}
Round::~Round(){
	delete maze;
}
void Round::step(){
	
}
Maze* Round::get_maze(){
		return maze;
}

Tank::Tank(Game* g){
	game = g;
	x = y = ang = 0;
	dead = false;
	p_ctrl = {false,false,false,false,false};
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
void Tank::step(){
	double nx,ny,dp,px,py;
	
	double pa = ang;
	
	int turn = (ctrl.back().lt ? 1 : 0)-(ctrl.back().rt ? 1 : 0);
	ang += turn * STEP_ANG;
	
	if(check_wall_coll(nx,ny,px,py,dp)) ang = pa;
	
	double prx = x, pry = y;
	
	double step = STEP_DST * ((ctrl.back().fd ? 1 : 0) - (ctrl.back().bk ? REV_RAT : 0));
	rotate_add(ang, step, 0, x, y);

	if(check_wall_coll(nx,ny,px,py,dp)) {x=prx; y=pry;}
	
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
			gen_rect(ix+i-WALL_THK + 1, iy+j-WALL_THK, 2*WALL_THK, 2*WALL_THK + 1,wxs,wys);
			if(game->get_round()->get_maze()->vwall(ix+i, iy+j) && poly_coll(txs,tys,4,wxs,wys,4,nx,ny,dp,px,py)){
				return true;
			}
		}
	}
	return false;
}