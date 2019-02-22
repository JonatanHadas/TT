#include "main_scr.h"

#define SCR_W 1280
#define SCR_H 960

#define SETT_W 560
#define SERV_H 200

#define TRANS_CHS 7
#define TRANS_SPD 0.1

#include <math.h>
#include "utf8.h"
#include <vector>

#include <stdio.h>

void follow(double& val, double tar, double spd){
	double dv = tar - val;
	int dir = dv>0 ? 1 : -1;
	dv = std::abs(dv);
	dv = std::min(dv, spd*sqrt(dv));
	val += dv*dir;
}

SubMenu::SubMenu(SDL_Renderer* r){
	rend = r;
	p = pt = 0;
	conn = false;
}
void SubMenu::set_conn(bool c){
	conn = c;
}
void SubMenu::draw_back(bool active){
	follow(p, pt, TRANS_SPD);
	SDL_SetRenderDrawColor(rend, 0, 0, 0, active ? (int)(p*TRANS_CHS) : 0);
	SDL_RenderClear(rend);
}
void SubMenu::event(SDL_Event& e){
	switch(e.type){
	case SDL_MOUSEMOTION:
		m_x = e.motion.x;
		m_y = e.motion.y;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		pt = 1;
	}
}
void SubMenu::lose_mfocus(){
	pt = 0;
}

#define ADDR_X 10
#define ADDR_XX 20
#define ADDR_Y 10

#define ADDR_H 50
#define ADDR_W 700

ConnectionMenu::ConnectionMenu(SDL_Renderer* r) : SubMenu(r){
	focus_addr = false;
	addr_m = NULL;
	reset_msg();
}
ConnectionMenu::~ConnectionMenu(){
	if(addr_m) delete addr_m;
}
bool ConnectionMenu::in_addr(int x, int y){
	return x>=ADDR_X && y>=ADDR_Y && x < ADDR_X+ADDR_W && y < ADDR_Y+ADDR_H;
}
void ConnectionMenu::reset_msg(){
	if(addr_m) delete addr_m;
	addr_m = new Msg(addr.c_str(), {0,0,0,255}, FONT_NRM, rend);
}
void ConnectionMenu::event(SDL_Event& e){
	SubMenu::event(e);
	switch(e.type){
	case SDL_MOUSEBUTTONDOWN:
		focus_addr = in_addr(e.button.x, e.button.y);
	case SDL_KEYDOWN:
		switch(e.key.keysym.sym){
		case SDLK_BACKSPACE:
			if(focus_addr && addr.size()>0){
				utf8_pop_back(addr);
				reset_msg();
			}
			break;
		}
		break;
	case SDL_TEXTINPUT:
		if(focus_addr){
			addr += e.text.text;
			reset_msg();
		}
		break;
	}
}
void ConnectionMenu::draw(){
	draw_back(true);
	
	SDL_Rect r;
	r.x = ADDR_X; r.y = ADDR_Y; r.w = ADDR_W; r.h = ADDR_H;
	int c = focus_addr ? 235 : 245;
	SDL_SetRenderDrawColor(rend, c,c,c, 192);
	SDL_RenderFillRect(rend, &r);
	
	if(in_addr(m_x, m_y)){
		SDL_SetRenderDrawColor(rend, 0,0,0,255);
		SDL_RenderDrawRect(rend, &r);
	}
	
	addr_m->render_centered(ADDR_XX, ADDR_Y + ADDR_H/2, AL_LEFT);
}
void ConnectionMenu::lose_mfocus(){
	SubMenu::lose_mfocus();
}
void ConnectionMenu::lose_kfocus(){
	focus_addr = false;
}
	
PlayerMenu::PlayerMenu(SDL_Renderer* r) : SubMenu(r){
}
PlayerMenu::~PlayerMenu(){
	
}
void PlayerMenu::event(SDL_Event& e){
	SubMenu::event(e);
}
void PlayerMenu::draw(){
	draw_back(conn);
}
void PlayerMenu::lose_mfocus(){
	SubMenu::lose_mfocus();
}
void PlayerMenu::lose_kfocus(){
	
}
	

SettingMenu::SettingMenu(SDL_Renderer* r) : SubMenu(r){
}
SettingMenu::~SettingMenu(){
	
}
void SettingMenu::draw(){
	draw_back(true);
}	
void SettingMenu::event(SDL_Event& e){
	SubMenu::event(e);
}
void SettingMenu::lose_mfocus(){
	SubMenu::lose_mfocus();
}
void SettingMenu::lose_kfocus(){
	
}

MainScr::MainScr(Main* up) : State(up), conn(up->get_renderer()), play(up->get_renderer()), sett(up->get_renderer()){
	conn_t = SDL_CreateTexture(	upper->get_renderer(), SDL_PIXELFORMAT_UNKNOWN,
								SDL_TEXTUREACCESS_TARGET, SCR_W-SETT_W, SERV_H);
	play_t = SDL_CreateTexture(	upper->get_renderer(), SDL_PIXELFORMAT_UNKNOWN,
								SDL_TEXTUREACCESS_TARGET, SCR_W-SETT_W, SCR_H - SERV_H);
	sett_t = SDL_CreateTexture(	upper->get_renderer(), SDL_PIXELFORMAT_UNKNOWN,
								SDL_TEXTUREACCESS_TARGET, SETT_W, SCR_H);
								
	SDL_SetTextureBlendMode(conn_t, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(play_t, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(sett_t, SDL_BLENDMODE_BLEND);
	
	upper->set_screen_size(SCR_W, SCR_H);
	
	SDL_GetMouseState(&m_x, &m_y);
	
	mfocus = kfocus = NULL;

	SDL_StartTextInput();
}
MainScr::~MainScr(){
	SDL_DestroyTexture(conn_t);
	SDL_DestroyTexture(play_t);
	SDL_DestroyTexture(sett_t);
	SDL_StopTextInput();
}

void MainScr::set_mfocus(){
	SubMenu* s;
	
	if(m_x < SCR_W - SETT_W){
		if(m_y < SERV_H) s = &conn;
		else s = &play;
	}
	else s = &sett;

	
	if(mfocus && s!=mfocus) mfocus->lose_mfocus();
	mfocus = s;
}
	
void MainScr::set_kfocus(){
	if(kfocus && kfocus!=mfocus) kfocus->lose_kfocus();
	kfocus = mfocus;
}
void MainScr::set_conn(bool c){
	conn.set_conn(c);
	play.set_conn(c);
	sett.set_conn(c);
}
void MainScr::m_correct(int& x, int& y){
	if(x >= SCR_W - SETT_W) x-= SCR_W - SETT_W;
	else if(y >= SERV_H) y -= SERV_H;
}
	
bool MainScr::step(){
	SDL_Event e;
	
	
	
	while(SDL_PollEvent(&e)){
		switch(e.type){
		case SDL_QUIT:
			return true;
		case SDL_MOUSEMOTION:
			m_x = e.motion.x;
			m_y = e.motion.y;
			set_mfocus();
			m_correct(e.motion.x, e.motion.y);
			mfocus->event(e);
			break;
		case SDL_MOUSEBUTTONDOWN:
			set_kfocus();
		case SDL_MOUSEBUTTONUP:
			m_correct(e.button.x, e.button.y);
		case SDL_MOUSEWHEEL:
			mfocus->event(e);
			break;
		case SDL_KEYDOWN:
			switch(e.key.keysym.sym){
			case SDLK_ESCAPE:
				return true;
			}
		case SDL_KEYUP:
		case SDL_TEXTINPUT:
			kfocus->event(e);
		}
	}
	
	upper->draw_back();
	
	SDL_Rect r;
	
	
	SDL_SetRenderTarget(upper->get_renderer(), conn_t);
	conn.draw();
	r.x = 0; r.y = 0; r.w = SCR_W-SETT_W; r.h = SERV_H;
	SDL_SetRenderTarget(upper->get_renderer(), NULL);
	SDL_RenderCopy(upper->get_renderer(), conn_t, NULL, &r);
	
	SDL_SetRenderTarget(upper->get_renderer(), play_t);
	play.draw();
	r.x = 0; r.y = SERV_H; r.w = SCR_W-SETT_W; r.h = SCR_H-SERV_H;
	SDL_SetRenderTarget(upper->get_renderer(), NULL);
	SDL_RenderCopy(upper->get_renderer(), play_t, NULL, &r);
	
	SDL_SetRenderTarget(upper->get_renderer(), sett_t);
	sett.draw();
	r.x = SCR_W-SETT_W; r.y = 0; r.w = SETT_W; r.h = SCR_H;
	SDL_SetRenderTarget(upper->get_renderer(), NULL);
	SDL_RenderCopy(upper->get_renderer(), sett_t, NULL, &r);
	
	return false;
}