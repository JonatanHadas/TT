#include "game_draw.h"
#include "game_consts.h"

#include "gui_util.h"
#include "utils.h"

#include "sounds.h"

#include "geom.h"
#include <vector>

#include <stdio.h>

#define BLOCK_SIZE 100

#define DRC(x) ((int)(BLOCK_SIZE * (x)))

#define WALL_D_T DRC(WALL_THK)

#define TANK_D_W DRC(TANK_W)
#define TANK_D_H DRC(TANK_H)

#define TANKC_D_W TANK_D_W
#define TANKC_D_H DRC(TANK_H * 1.2)

#define DR_I_W 20
#define DR_I_H 5
#define DR_SPD 10
#define DR_LOOP 100

#define HOM_TIMER_MIN 20
#define HOM_TIMER_MAX 60
#define HOM_TIMER_STEP 3


#define LASER_T 10

#define SHARD_D_W DRC(TANK_W * 0.35)
#define SHARD_D_H DRC(TANK_H * 0.5)

#define SHARD_W 7
#define SHARD_H 15

#define SHARD_DAMP 0.1

#define SHARD_W_DMP 0.02

#define WIFI_TIME 120
#define BRDCST_S0 10
#define BRDCST_SF 150
#define BRDCST_T 30

#define AIM_LEN STEP_LASER*LASER_TTL/2
#define AIM_DENS 20.0
#define AIM_TIME 2

Shard::Shard(TankImg* tank_img, double xx, double yy, EffectManager* smkm, SDL_Texture* smk_tex){
	smk = smkm;
	tex = smk_tex;

	img = tank_img;
	x=xx;y=yy;
	
	cx = rand_range(0,2);
	cy = rand_range(0,1);
	
	flip = rand_range(0,1)==0;
	
	ang = rand_range(-100,100)*M_PI/100;
	double v = rand_range(0, 101) * 0.002; 
	vx = v * cos(ang);
	vy = v * sin(ang);
	
	ang = rand_range(-100,100)*M_PI/100;
	
	w = rand_range(-100,101)*0.001;
	
	timer = time = rand_range(20,30);
}
void Shard::step(){
	SDL_Rect dst,src;
	
	dst.x = (int)((x + WALL_THK)*BLOCK_SIZE - SHARD_D_W/2.0);
	dst.y = (int)((y + WALL_THK)*BLOCK_SIZE - SHARD_D_H/2.0);
	dst.w = SHARD_D_W;
	dst.h = SHARD_D_H;
	
	src.w = SHARD_W; src.h = SHARD_H;
	src.x = cx*SHARD_W; src.y = cy*SHARD_H;
		
	double a = sqrt((timer)/(float)time);
	
	SDL_SetTextureAlphaMod(img->shards, (int)(a*255));
	SDL_RenderCopyEx(get_manager()->get_renderer(), img->shards, &src, &dst, RAD2DEG(ang), NULL, flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	
	if(rand_range(0,1000) / 100.0< (timer/(float)time)*4){
		int size = 8 - (int)(4 * (timer)/(float)time);
		
	
		double v = rand_range(0,101) * 0.0003 * (timer)/(float)time;
		double ang = rand_range(-100,100)*M_PI/100;
	
		double svx = v*cos(ang);
		double svy = v*sin(ang);
		FadeOut* fo = new FadeOut(	tex,
									(x+WALL_THK)*BLOCK_SIZE, (y+WALL_THK)*BLOCK_SIZE,
									(vx+svx)*BLOCK_SIZE, (vy+svy)*BLOCK_SIZE,
									size, size,
									0.0,
									SDL_FLIP_NONE,
									rand_range(20,30),0.5);
		
		fo->set_color(0,0,0);
		fo->set_damp(0.4);
									
		smk->add_effect(fo);
	}

	x += vx; y += vy; ang += w;
	
	vx *= (1-SHARD_DAMP);
	vy *= (1-SHARD_DAMP);
	w *= (1-SHARD_W_DMP);
	
	timer--;
	if(timer == 0) get_manager()->remove_effect(this);
}


std::pair<Upgrade::Type, Img> upg2img_a[UPG_NUM] = {
	{Upgrade::GATLING, IMG_GATLING_SYM},
	{Upgrade::LASER, IMG_LASER_SYM},
	{Upgrade::BOMB, IMG_BOMB_SYM},
	{Upgrade::DEATH_RAY, IMG_DEATH_RAY_SYM},
	{Upgrade::WIFI, IMG_WIFI_SYM},
	{Upgrade::MISSILE, IMG_MISSILE_SYM},
	{Upgrade::MINE, IMG_MINE_SYM},
	};
std::map<Upgrade::Type, Img> upg2img(upg2img_a,upg2img_a+UPG_NUM);

BoardDrawer::BoardDrawer(GameQ* q, SDL_Renderer* r, GameConfig& cf) : back_fx(r), mid_fx(r), front_fx(r){
	renderer = r;
	game = q;
	tank_images = new TankImg[q->get_tank_num()];
	for(int i = 0; i<q->get_tank_num(); i++) {
		generate_tank(cf.colors[i], r, tank_images + i);
	}
	SDL_SetRenderDrawColor(r, 255,255,255,255);
	circ = gen_circle(r,20.0);
	rect = gen_uniform(r,20,20, {255,255,255,255});
	
}
BoardDrawer::~BoardDrawer(){
	delete[] tank_images;
	
	SDL_DestroyTexture(circ);
	SDL_DestroyTexture(rect);
}
void BoardDrawer::draw(){
	SDL_SetRenderDrawColor(renderer, 240,240,240,255);
	SDL_RenderClear(renderer);
	
	SDL_SetRenderDrawColor(renderer, 150,150,150,255);
	Maze* maze = game->get_round()->get_maze();
	for(int i = -1; i<maze->get_w(); i++){
		for(int j = -1; j<maze->get_h(); j++){
			SDL_Rect r;
			
			r.x = BLOCK_SIZE*(i);
			r.y = BLOCK_SIZE*(j+1);
			r.w = 2*WALL_D_T + BLOCK_SIZE;
			r.h = 2*WALL_D_T;
			if(maze->hwall(i,j)) SDL_RenderFillRect(renderer, &r);
			
			r.x = BLOCK_SIZE*(i+1);
			r.y = BLOCK_SIZE*(j);
			r.w = 2*WALL_D_T;
			r.h = 2*WALL_D_T + BLOCK_SIZE;
			if(maze->vwall(i,j)) SDL_RenderFillRect(renderer, &r);
		}
	}
	
	
	auto mines = game->get_round()->get_mines();
	for(auto it = mines.begin(); it!=mines.end(); it++){
		if((*it)->get_started()){
			SDL_Rect r;
			SDL_Point p;
			r.w = r.h = DRC(2*MINE_SIZE);
			p.x = DRC((*it)->get_x());
			p.y = DRC((*it)->get_y());

			r.x = p.x - r.w/2;
			r.y = p.y - r.h/2;
			
			p.x -= r.x; p.y -= r.y;
			
			int i = (*it)->get_tank_ind();
			
			SDL_Texture* img = (*it)->get_active() ? tank_images[i].mine_on : tank_images[i].mine_off;
			
			SDL_RenderCopyEx(renderer, img, NULL, &r, RAD2DEG((*it)->get_ang())+90, &p, SDL_FLIP_NONE);
		}
	}
	
	back_fx.step();

	auto upgs = game->get_round()->get_upgs();
	for(auto it = upgs.begin(); it!=upgs.end(); it++){
		SDL_Rect r;
		r.w = r.h = DRC(UPG_SIZE);
		r.x = DRC((*it)->x + 0.5)-r.w/2;
		r.y = DRC((*it)->y + 0.5)-r.h/2;
		
		SDL_RenderCopyEx(renderer, get_img(upg2img[(*it)->type]), NULL, &r, UPG_ANG, NULL, SDL_FLIP_NONE);
		
		delete (*it);
	}
		
	auto shots = game->get_round()->get_shots();
	for(auto it = shots.begin(); it != shots.end(); it++){
		SDL_Rect r;
		ShotQ* sht;
		FragmentQ* frg;
		DeathRayQ* dtr;
		MissileQ* mis;
		
		int sy;
		
		double ang;
		SDL_Point* ps;
		SDL_Point p;
		SDL_Color ctk,ctr;
		switch((*it)->get_type()){
		case GenShot::TYPE_REG:
		case GenShot::TYPE_GATLING:
		case GenShot::TYPE_BOMB:
			sht = (ShotQ*)(*it);
			r.w = r.h = DRC(2*sht->get_r());
			r.x = WALL_D_T + DRC(sht->get_x()) - r.w/2;
			r.y = WALL_D_T + DRC(sht->get_y()) - r.w/2;
			SDL_SetTextureColorMod(circ, 0,0,0);
			SDL_RenderCopy(renderer, circ, NULL, &r);
			break;
		case GenShot::TYPE_LASER:
			sht = (ShotQ*)(*it);

			ctk = get_tank_col(sht->get_tank_ind());
			
			for(int i = 1; i<sht->get_colls().size(); i++){
				double x1 = sht->get_colls()[i-1].first;
				double y1 = sht->get_colls()[i-1].second;
				double x2 = sht->get_colls()[i].first;
				double y2 = sht->get_colls()[i].second;

				double x = (x1+x2)/2, y = (y1+y2)/2, dx=x2-x1, dy=y2-y1;
				double ang = RAD2DEG(atan2(y2-y1,x2-x1)) + 90.0;	
				
				int len = DRC(sqrt(dx*dx+dy*dy));
				
				FadeOut* fo;
				
				fo = new FadeOut(	circ,
									(x2+WALL_THK)*BLOCK_SIZE, (y2+WALL_THK)*BLOCK_SIZE,
									0.0,0.0,
									DRC(LASER_R), DRC(LASER_R),
									0.0,
									SDL_FLIP_NONE,
									LASER_T);
				fo->set_color(ctk.r,ctk.g,ctk.b);
				
				mid_fx.add_effect(fo);

				fo = new FadeOut(	rect,
									(x+WALL_THK)*BLOCK_SIZE, (y+WALL_THK)*BLOCK_SIZE,
									0.0,0.0,
									DRC(LASER_R), len,
									ang,
									SDL_FLIP_NONE,
									LASER_T);
									
				fo->set_color(ctk.r,ctk.g,ctk.b);
				
				mid_fx.add_effect(fo);

			}
				
			
			break;
		case GenShot::TYPE_FRAGMENT:
			frg = (FragmentQ*)(*it);
			ang = 180.0 * frg->get_dst() + RAD2DEG(RAD2DEG(frg->get_ang()));
			r.x = WALL_D_T + DRC(frg->get_x());
			r.y = WALL_D_T + DRC(frg->get_y());
			r.w = r.h = 8;
			r.x -= r.w/2; r.y -= r.h/2;
			
			SDL_SetTextureAlphaMod(get_img(IMG_FRAGMENT), (int)(255*(1.0-frg->get_t())));
			SDL_RenderCopyEx(renderer, get_img(IMG_FRAGMENT), NULL, &r, ang, NULL, SDL_FLIP_NONE);
			
			break;
		case GenShot::TYPE_DEATH_RAY:
			dtr = (DeathRayQ*)(*it);
			
			r.w = DRC(2*DR_W);
			r.h = DRC(DR_STEP);
			sy = (game->get_time() * DR_SPD) % DR_LOOP;
			for(int i = 1; i<dtr->get_point_num(); i++, sy -= DR_I_H){
				double x1 = dtr->get_x(i-1);
				double y1 = dtr->get_y(i-1);
				double x2 = dtr->get_x(i);
				double y2 = dtr->get_y(i);
				
				r.x = WALL_D_T + DRC((x1+x2)/2) - r.w/2;
				r.y = WALL_D_T + DRC((y1+y2)/2) - r.h/2;
				
				SDL_Rect s;
				s.w = DR_I_W;
				s.h = DR_I_H;
				s.x = 0; s.y = sy+DR_LOOP;
				
				sy %= DR_LOOP;
				if(sy<0) sy+=DR_LOOP;
				
				double ang = RAD2DEG(atan2(y2-y1,x2-x1)) + 90;
				
				SDL_RenderCopyEx(	renderer, tank_images[dtr->get_tank_ind()].deathray, 
									&s, &r, ang, NULL, SDL_FLIP_VERTICAL);
									
			}
			
			break;
		case GenShot::TYPE_WIFI:
		case GenShot::TYPE_MISSILE:
			mis = (MissileQ*)(*it);
			
			p.x = WALL_D_T + DRC(mis->get_x());
			p.y = WALL_D_T + DRC(mis->get_y());
			
			r.w = DRC(MISSILE_W); r.h = DRC(MISSILE_L);
			r.x = p.x - r.w/2;
			r.y = p.y - r.h/2;
			
			p.x -= r.x; p.y -= r.y;
			
			SDL_RenderCopyEx(renderer, tank_images[mis->get_tank_ind()].missile, NULL, &r, RAD2DEG(mis->get_ang())+90, &p, SDL_FLIP_NONE);
			
			ctr.a = 255;
			if(mis->get_tar_ind() >= 0) ctr = get_tank_col(mis->get_tar_ind());
			else ctr.r = ctr.g = ctr.b = 128;
			
			double rnd_ang = rand_range(-100,101) * M_PI/100;
			double rnd = rand_range(0,101)/10000.0;
			
			double x = mis->get_x();
			double y = mis->get_y();
			double ang = mis->get_ang();
			double vx = STEP_MISSILE * cos(ang) + rnd * cos(rnd_ang);
			double vy = STEP_MISSILE * sin(ang) + rnd * sin(rnd_ang);
			
			
			FadeOut* fo = new FadeOut(	circ,
										(x+WALL_THK)*BLOCK_SIZE, (y+WALL_THK)*BLOCK_SIZE,
										vx*BLOCK_SIZE, vy*BLOCK_SIZE,
										8,8,
										0,
										SDL_FLIP_NONE,
										30);
										
			fo->set_color(ctr.r,ctr.g,ctr.b);
			fo->set_damp(0.1);
			back_fx.add_effect(fo);
						
			break;
		}
		int tt,timer,ind;
		switch((*it)->get_type()){
		case GenShot::TYPE_MISSILE:
			mis = (MissileQ*)(*it);
			
			ind = mis->get_tar_ind();
			timer = mis->get_time();
			tt = HOM_TIMER_MAX;
			if(ind>=0 && timer>HOMING_TIME){
				TankQ* t = game->get_tank(ind);
				
				int dx,dy,xt = t->get_x(), yt = t->get_y(), xm = mis->get_x(), ym = mis->get_y();
				
				tt = HOM_TIMER_MIN + HOM_TIMER_STEP * game->get_round()->get_maze()->dist(xt,yt,xm,ym,dx,dy);
				tt = tt > HOM_TIMER_MAX ? HOM_TIMER_MAX : tt;
				
			}
			if(timer % tt == 0) play(SND_BEEP);
			break;
		case GenShot::TYPE_WIFI:
			mis = (MissileQ*)(*it);
			
			TankQ* t = game->get_tank(mis->get_tank_ind());
			if(!t->is_dead() && mis->get_time() % WIFI_TIME == 1){
				double x = t->get_x(), y = t->get_y();
				double dx = mis->get_x() - t->get_x();
				double dy = mis->get_y() - t->get_y();
				double ang = RAD2DEG(atan2(dy,dx));
				
				back_fx.add_effect(new FadeSize(get_tank_img(t->get_ind())->broadcast,
												(x+WALL_THK)*BLOCK_SIZE,(y+WALL_THK)*BLOCK_SIZE,
												BRDCST_S0, BRDCST_S0,
												BRDCST_SF, BRDCST_SF,
												ang,
												SDL_FLIP_NONE,
												BRDCST_T));		
			}
			break;
		}
		delete (*it);
	}
	

	for(int i = 0; i < game->get_tank_num(); i++){
		TankQ* t = game->get_tank(i);
		SDL_Color col = get_tank_col(i);
		SDL_SetRenderDrawColor(renderer,col.r,col.g,col.b,255);
		if(!t->is_dead()){
			double x = t->get_x(), y = t->get_y(), ang = t->get_ang();
			
			SDL_Rect r;
			SDL_Point p;
			p.x = WALL_D_T + DRC(x);
			p.y = WALL_D_T + DRC(y);
			r.w = TANK_D_W; r.h = TANK_D_H;
			r.x = p.x - r.w/2;
			r.y = p.y - r.h/2;
			
			p.x -= r.x; p.y -= r.y;
			
			SDL_RenderCopyEx(renderer,tank_images[i].body, NULL, &r, RAD2DEG(ang)+90, &p, SDL_FLIP_NONE);
			
			p.x += r.x; p.y += r.y;
			
			if(t->get_state() == Tank::WIFI || t->get_state() == Tank::MISSILE){
				r.w = DRC(MISSILE_W); r.h = DRC(MISSILE_L);
				p.x = WALL_D_T + DRC(t->get_x()); p.y = WALL_D_T + DRC(t->get_y());
				r.x = p.x - r.w/2;
				r.y = p.y - DRC(MISSILE_DST) - r.h/2;
				p.x -= r.x; p.y -= r.y;
				
				SDL_RenderCopyEx(renderer, tank_images[i].missile, NULL, &r, RAD2DEG(ang)+90, &p, SDL_FLIP_NONE);

			}
			
			p.x = WALL_D_T + DRC(x);
			p.y = WALL_D_T + DRC(y);
			r.w = TANKC_D_W; r.h = TANKC_D_H;
			
			SDL_Texture* cannon;
			int count;
			SDL_Color col;
			std::vector<std::pair<double,double>> ps;
			switch(t->get_state()){
			case Tank::REG:
				cannon = tank_images[i].cannon;
				break;
			case Tank::GATLING:
				cannon = tank_images[i].gatling[0];
				break;
			case Tank::GATLING_WAIT:
			case Tank::GATLING_SHOOT:
				cannon = tank_images[i].gatling[game->get_time()%3];
				break;
			case Tank::LASER:
				ps = t->predict_colls(AIM_LEN, LASER_R);
				count = ps.size();
				col = get_tank_col(i);
				
				
				
				for(int j = 1; j<count; j++){
					double x1 = ps[j].first;
					double y1 = ps[j].second;
					double x2 = ps[j-1].first;
					double y2 = ps[j-1].second;
					
					double dx = x2-x1, dy=y2-y1;
					double am = sqrt(dx*dx + dy*dy)*AIM_DENS;
					int num = (int)am;
					
					for(int k = 0; k<=num; k++){
						if(k < num || rand_range(0,1000)/1000.0 < am-num){
							double t = rand_range(0,1000)/1000.0;
						
							FadeOut* fo = new FadeOut(	circ,
														(WALL_THK + x1 + t*dx)*BLOCK_SIZE, (WALL_THK + y1 + t*dy)*BLOCK_SIZE,
														0.0,0.0,
														DRC(LASER_R),DRC(LASER_R),
														0.0,
														SDL_FLIP_NONE,
														AIM_TIME);
														
							fo->set_color(col.r,col.g,col.b);
							mid_fx.add_effect(fo);
						}
					}
				}
			case Tank::LASER_SHOOT:
				cannon = tank_images[i].laser;
				break;
			case Tank::BOMB:
			case Tank::BOMB_SHOOT:
				cannon = tank_images[i].thick_cannon;
				break;
			case Tank::DEATH_RAY:
			case Tank::DEATH_RAY_WAIT1:
				cannon = tank_images[i].ray_gun0;
				break;
			case Tank::DEATH_RAY_WAIT2:
				cannon = tank_images[i].ray_gun1;
				break;
			case Tank::DEATH_RAY_WAIT3:
				cannon = tank_images[i].ray_gun2;
				break;
			case Tank::DEATH_RAY_SHOOT:
				cannon = tank_images[i].ray_gun3;
				break;
			case Tank::WIFI:
			case Tank::WIFI_SHOOT:
			case Tank::MISSILE:
			case Tank::MISSILE_SHOOT:
				cannon = tank_images[i].launcher;
				break;
			case Tank::MINE:
				cannon = tank_images[i].mine_off;
				r.w = r.h = DRC(2*MINE_SIZE);
				break;
			}
			
			r.x = p.x - r.w/2;
			r.y = p.y - r.h/2;
			
			p.x -= r.x; p.y -= r.y;

			SDL_RenderCopyEx(renderer,cannon, NULL, &r, RAD2DEG(ang)+90, &p, SDL_FLIP_NONE);

		}
	}
	mid_fx.step();
	front_fx.step();
}
TankImg* BoardDrawer::get_tank_img(int i){
	return tank_images+i;
}

void BoardDrawer::start_round(){
	back_fx.clear();
	front_fx.clear();
	mid_fx.clear();
}

void BoardDrawer::tank_death(int ind){
	double x = game->get_tank(ind)->get_x();
	double y = game->get_tank(ind)->get_y();
	
	int num = rand_range(15,20);
	
	for(int i = 0; i<num; i++){
		front_fx.add_effect(new Shard(get_tank_img(ind), x, y, &mid_fx, circ));
	}
	
	num = rand_range(20,30);
	
	for(int i = 0; i<num; i++){
		
		double ang = rand_range(-100,100)*M_PI/100;
		double v = rand_range(0,11) * 0.006;
		double vx = v*cos(ang);
		double vy = v*sin(ang);
	
		FadeOut* fo = new FadeOut(	circ,
									(x+WALL_THK)*BLOCK_SIZE, (y+WALL_THK)*BLOCK_SIZE,
									vx*BLOCK_SIZE, vy*BLOCK_SIZE,
									30,30,
									0.0,
									SDL_FLIP_NONE,
									30,0.75);
		fo->set_color(0,0,0);
		fo->set_damp(0.3);
		mid_fx.add_effect(fo);
	}
}

void BoardDrawer::place_mine(GameQEventCreateMine* e){
	double dx = MINE_DST * cos((e->get_ang()));
	double dy = MINE_DST * sin((e->get_ang()));
	double d = 0.2;
	double t = 30;
	
	double tdr = d/(1-pow(1-d,t));
	
	FadeOut* fo = new FadeOut(	get_tank_img(e->get_ind())->mine_off,
								(e->get_x()+dx+WALL_THK)*BLOCK_SIZE, (e->get_y()+dy+WALL_THK)*BLOCK_SIZE,
								-dx * tdr * BLOCK_SIZE, -dy * tdr * BLOCK_SIZE,
								DRC(MINE_SIZE*2), DRC(MINE_SIZE*2),
								RAD2DEG(e->get_ang()) + 90.0,
								SDL_FLIP_NONE,
								t);
								
	fo->set_damp(d);
	back_fx.add_effect(fo);
}

void BoardDrawer::tank_stuck(GameQEventTankStuck* e){
	TankQ* tank = game->get_tank(e->get_ind());
	double spd = e->get_spd();
	
	double x = tank->get_x(), y = tank->get_y(), ang = tank->get_ang();
	
	double rx = -0.4*TANK_H * (spd > 0 ? 1 : -1);

	if(rand_range(0,2)<1){
		double ry = rand_range(-100,101)*TANK_W * 0.003;
			
		double rnd_ang = rand_range(-100,100)*M_PI/100;
		double rnd = rand_range(0,101)*0.0002;

		double vx = -spd * cos(ang) + rnd * cos(rnd_ang);
		double vy = -spd * sin(ang) + rnd * sin(rnd_ang);
		
		rotate_add(ang, rx, ry, x, y);
		
		FadeOut* fo = new FadeOut(	circ,
									(x+WALL_THK)*BLOCK_SIZE, (y+WALL_THK)*BLOCK_SIZE,
									vx * BLOCK_SIZE, vy * BLOCK_SIZE,
									20, 20,
									0.0,
									SDL_FLIP_NONE,
									15, 0.5);
		fo->set_color(150,150,150);
		fo->set_damp(0.2);
		
		back_fx.add_effect(fo);
	}
	
	x = tank->get_x(); y = tank->get_y();
	
	if(rand_range(0,9)<1){
		double ry = rand_range(-100,101)*TANK_W * 0.003;
		
		double rnd_ang = rand_range(-100,100)*M_PI/100;
		double rnd = rand_range(0,101)*0.00005;

		double vx = -spd * 1.5 * cos(ang) + rnd * cos(rnd_ang);
		double vy = -spd * 1.5 * sin(ang) + rnd * sin(rnd_ang);

		rotate_add(ang, rx, ry, x, y);
		
		FadeOut* fo = new FadeOut(	circ,
									(x+WALL_THK)*BLOCK_SIZE, (y+WALL_THK)*BLOCK_SIZE,
									vx * BLOCK_SIZE, vy * BLOCK_SIZE,
									4,4,
									0.0,
									SDL_FLIP_NONE,
									10);
		fo->set_color(0,0,0);
		fo->set_damp(0.1);
		
		back_fx.add_effect(fo);
	}
}

GameDrawer::GameDrawer(GameQ* q, SDL_Renderer* r, GameConfig& cf){
	game = q;
	renderer = r;
	board = new BoardDrawer(q,r,cf);
	board_t = NULL;
	for(int i = 0; i<game->get_team_num(); i++){
		scores.push_back(NULL);
		update_score(i);
	}
}
GameDrawer::~GameDrawer(){
	if(board) delete board;
	if(board_t) SDL_DestroyTexture(board_t);
	for(int i = 0; i<game->get_team_num(); i++){
		if(scores[i]) delete scores[i];
	}
}
void GameDrawer::update_score(int i){
	if(scores[i]) delete scores[i];
	char txt[100];
	sprintf(txt,"%d",game->get_team(i)->get_score());
	scores[i] = new Msg(txt, {0,0,0,255}, FONT_MID, renderer);
}

#define BOARD_Y 45
#define BOARD_H 810
#define BOARD_W 1010


#define SCORE_MAR 0
#define SCORE_Y 930

#define TKIMG_W 60
#define TKIMG_H 33
#define TKIMG_R 50
#define TKIMG_A (M_PI/4)

void GameDrawer::draw(){
	GameQEvent* event;
	while(event = game->get_event()){
		Maze* maze;
		switch(event->get_type()){
		case GameQEvent::TYPE_RND_START:
			if(board_t) SDL_DestroyTexture(board_t);
			maze = game->get_round()->get_maze();
			w = WALL_D_T*2 + BLOCK_SIZE*maze->get_w();
			h = WALL_D_T*2 + BLOCK_SIZE*maze->get_h();
			board_t = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_UNKNOWN,SDL_TEXTUREACCESS_TARGET, w,h);
			board->start_round();
			silence();
			break;
		case GameQEvent::TYPE_SCORE:
			update_score(((GameQEventScore*)event)->get_ind());
			break;
		case GameQEvent::TYPE_TANK_DEAD:
			board->tank_death(((GameQEventTankDeath*)event)->get_ind());
			play(SND_EXPLOSION);
			break;
		case GameQEvent::TYPE_SHOT_CRT:
			switch(((GameQEventCreateShot*)event)->get_stype()){
			case GenShot::TYPE_REG:
			case GenShot::TYPE_GATLING:
			case GenShot::TYPE_BOMB:
				//play(SND_);
				break;
			case GenShot::TYPE_LASER:
				play(SND_LASER);
				break;
			case GenShot::TYPE_DEATH_RAY:
				play(SND_RAYGUN);
				break;
			}
			break;
		case GameQEvent::TYPE_SHOT_RMV:
			break;
		case GameQEvent::TYPE_UPG_CRT:
			play(SND_POPUP);
			break;
		case GameQEvent::TYPE_UPG_RMV:
			play(SND_TAKE);
			break;
		case GameQEvent::TYPE_MIN_CRT:
			board->place_mine((GameQEventCreateMine*)event);
			play(SND_OFF);
			break;
		case GameQEvent::TYPE_MIN_ACT:
			play(SND_ON);
			break;
		case GameQEvent::TYPE_COLL:
			play(SND_COLLISION);
			break;
		case GameQEvent::TYPE_EXPL:
			play(SND_EXPLOSION_SMALL);
			break;
		case GameQEvent::TYPE_LOAD:
			play(SND_RAYLOAD);
			break;
		case GameQEvent::TYPE_TANK_STUCK:
			board->tank_stuck((GameQEventTankStuck*)event);
			break;
		}
		delete event;
	}
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
	SDL_RenderClear(renderer);
	
	int num = 0;
	std::vector<int> nums;
	for(int i = 0; i<game->get_team_num(); i++){
		nums.push_back(num);
		num += game->get_team(i)->get_tank_num();
	}
	nums.push_back(num);
	
	double dx = (GSCR_W - 2*SCORE_MAR)/num;
	
	for(int i = 0; i<game->get_team_num(); i++){
		scores[i]->render_centered(SCORE_MAR + (int)(0.5*dx*(nums[i]+nums[i+1])),SCORE_Y,AL_CENTER);
		
		int nd = (game->get_team(i)->get_tank_num()-1);
		double da = nd==0 ? 0 : (M_PI - TKIMG_A*2)/nd;
		double ia = nd==0 ? M_PI/2 : M_PI - TKIMG_A;
		for(int j = 0; j<game->get_team(i)->get_tank_num(); j++){
			double x = SCORE_MAR + dx * (nums[i]+j+0.5), y = SCORE_Y;
			
			rotate_add(ia - da*j, -TKIMG_R, 0, x,y);
			SDL_Rect r;
			r.w = TKIMG_W; r.h = TKIMG_H;
			r.x = x-r.w/2; r.y = y-r.h/2;
			SDL_RenderCopyEx(	renderer, board->get_tank_img(game->get_team(i)->get_tank(j)->get_ind())->image,
								NULL, &r,
								0,NULL,
								(j > game->get_team(i)->get_tank_num()/2) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
		}
	}
	
	if(board_t){
		SDL_Texture* tex = SDL_GetRenderTarget(renderer);
		SDL_SetRenderTarget(renderer, board_t);
		board->draw();
		SDL_SetRenderTarget(renderer,tex);
		
		SDL_Rect r;
		r.w = std::min(BOARD_W, BOARD_H * w / h);
		r.h = std::min(BOARD_H, BOARD_W * h / w);
		
		r.x = (GSCR_W - r.w)/2;
		r.y = BOARD_Y + (BOARD_H-r.h)/2;
		
		SDL_RenderCopy(renderer, board_t, NULL, &r);
	}
}
