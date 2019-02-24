#include "main_scr.h"

#define SCR_W 1280
#define SCR_H 960

#define SETT_W 560
#define SERV_H 200

#define TRANS_CHS 7
#define TRANS_SPD 0.1

#include <math.h>
#include "utf8.h"

#include "encoding.h"

#include "gui_util.h"

#include <stdio.h>

void follow(double& val, double tar, double spd){
	double dv = tar - val;
	int dir = dv>0 ? 1 : -1;
	dv = std::abs(dv);
	dv = std::min(dv, spd*sqrt(dv));
	val += dv*dir;
}

bool in_rect(SDL_Rect r, int x, int y){
	return x>=r.x && y>=r.y && x < r.x+r.w && y < r.y+r.h;
}

SubMenu::SubMenu(SDL_Renderer* r, MainScr* m){
	rend = r;
	p = pt = 0;
	main = m;
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
	m_x = m_y = -1;
	pt = 0;
}

#define ADDR_X 20
#define ADDR_XX 40
#define ADDR_Y 20

#define ADDR_H 50
#define ADDR_W 680

#define BUT_Y 80
#define BUT_H 100
#define BUT_W 100
#define START_X 20
#define LEAVE_X 140
#define BUT_OUT_X 260

#define ERR_X 30
#define ERR_Y 130
#define ERR_HX -500

#define LOAD_T 30

ConnectionMenu::ConnectionMenu(SDL_Renderer* r, MainScr* m) : SubMenu(r, m){
	focus_addr = false;
	addr_m = NULL;
	reset_msg();
	nums[0] = new Msg("Go!", {0,0,0,255},FONT_NRM, rend);
	nums[1] = new Msg("1", {0,0,0,255},FONT_NRM, rend);
	nums[2] = new Msg("2", {0,0,0,255},FONT_NRM, rend);
	nums[3] = new Msg("3", {0,0,0,255},FONT_NRM, rend);
	start = new Msg("Start", {0,0,0,255},FONT_NRM, rend);
	leave = new Msg("Leave", {0,0,0,255},FONT_NRM, rend);
	wait = new Msg("Wait", {0,0,0,255},FONT_NRM, rend);
	addr_fil = new Msg("Enter Server Address", {0,0,0,64},FONT_NRM, rend);
	err_msg = NULL; set_err("");
	loading[0] = new Msg("Loading", {0,0,0,255},FONT_NRM, rend);
	loading[1] = new Msg("Loading.", {0,0,0,255},FONT_NRM, rend);
	loading[2] = new Msg("Loading..", {0,0,0,255},FONT_NRM, rend);
	loading[3] = new Msg("Loading...", {0,0,0,255},FONT_NRM, rend);
	load_p = err_p = ERR_HX;
	num = -1;
	but_p = -1;
	st_prs = lv_prs = false;
}
ConnectionMenu::~ConnectionMenu(){
	if(addr_m) delete addr_m;
	delete err_msg;
	delete addr_fil;
	delete nums[0];
	delete nums[1];
	delete nums[2];
	delete nums[3];
	delete loading[0];
	delete loading[1];
	delete loading[2];
	delete loading[3];
	delete start; delete leave; delete wait;
}
const char* ConnectionMenu::get_address(){
	return addr.c_str();
}
void ConnectionMenu::set_err(const char* msg){
	if(err_msg) delete err_msg;
	err_msg = new Msg(msg, {128,0,0,255}, FONT_NRM, rend);
}
SDL_Rect ConnectionMenu::r_addr(){
	SDL_Rect r;
	r.x = ADDR_X; r.y = ADDR_Y; r.w = ADDR_W; r.h = ADDR_H;
	return r;
}
SDL_Rect ConnectionMenu::r_start(){
	SDL_Rect r;
	r.x = START_X + but_p*BUT_OUT_X; r.y = BUT_Y; r.w = BUT_W; r.h = BUT_H;
	return r;
}
SDL_Rect ConnectionMenu::r_leave(){
	SDL_Rect r;
	r.x = LEAVE_X + but_p*BUT_OUT_X; r.y = BUT_Y; r.w = BUT_W; r.h = BUT_H;
	return r;
}
void ConnectionMenu::reset_msg(){
	if(addr_m) delete addr_m;
	addr_m = new Msg(addr.c_str(), {0,0,0,255}, FONT_NRM, rend);
}
void ConnectionMenu::event(SDL_Event& e){
	SubMenu::event(e);
	switch(e.type){
	case SDL_MOUSEBUTTONDOWN:
		if(e.button.button == SDL_BUTTON_LEFT){
			focus_addr = in_rect(r_addr(), e.button.x, e.button.y) && !main->get_conn_try();
			if(in_rect(r_leave(), e.button.x, e.button.y) && main->get_conn()) lv_prs = true;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if(e.button.button == SDL_BUTTON_LEFT){
			if(in_rect(r_leave(), e.button.x, e.button.y) && main->get_conn() && lv_prs) {main->leave(); lv_prs = false;}
		}
	case SDL_KEYDOWN:
		switch(e.key.keysym.sym){
		case SDLK_BACKSPACE:
		case SDLK_KP_BACKSPACE:
			if(focus_addr && addr.size()>0){
				utf8_pop_back(addr);
				reset_msg();
			}
			break;
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
			if(focus_addr){
				focus_addr = false;
				main->connect();
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
	
	follow(err_p, main->get_conn_try() ? ERR_HX : ERR_X, 1.5);
	follow(load_p, main->get_conn_try() && !main->get_conn() ? ERR_X : ERR_HX, 1.5);
	follow(but_p, main->get_conn() ? 0 : -1 ,0.3);
	
	if(!((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && main->get_conn())) lv_prs = st_prs = false;
	
	SDL_Rect r = r_addr();
	int c = focus_addr ? 235 : 245;
	SDL_SetRenderDrawColor(rend, c,c,c, 192);
	SDL_RenderFillRect(rend, &r);
	
	if(in_rect(r, m_x, m_y)){
		SDL_SetRenderDrawColor(rend, 0,0,0,255);
		SDL_RenderDrawRect(rend, &r);
	}
	
	(addr.size()>0 ? addr_m : addr_fil)->render_centered(ADDR_XX, ADDR_Y + ADDR_H/2, AL_LEFT);
	
	r = r_start();
	c = st_prs ? 235 : 245;
	SDL_SetRenderDrawColor(rend, c,c,c, 192);
	SDL_RenderFillRect(rend, &r);
	if(in_rect(r, m_x, m_y)){
		SDL_SetRenderDrawColor(rend, 0,0,0,255);
		SDL_RenderDrawRect(rend, &r);
	}
	(num >= 0 ? nums[num] : (main->get_host() ? start : wait))->render_centered(r.x + r.w/2, r.y+r.h/2, AL_CENTER);

	r = r_leave();
	c = lv_prs ? 235 : 245;
	SDL_SetRenderDrawColor(rend, c,c,c, 192);
	SDL_RenderFillRect(rend, &r);
	if(in_rect(r, m_x, m_y)){
		SDL_SetRenderDrawColor(rend, 0,0,0,255);
		SDL_RenderDrawRect(rend, &r);
	}
	leave->render_centered(r.x + r.w/2, r.y+r.h/2, AL_CENTER);
	
	loading[(time/LOAD_T)%4]->render_centered((int)load_p, ERR_Y, AL_LEFT);
	err_msg->render_centered((int)err_p, ERR_Y, AL_LEFT);
	
	time++;
}
void ConnectionMenu::lose_mfocus(){
	SubMenu::lose_mfocus();
}
void ConnectionMenu::lose_kfocus(){
	focus_addr = false;
}

#define PLR_X 30
#define PLR_Y 30
#define PLR_S 40
#define SEP_S 20
#define PLR_SY 900

#define PLR_TKW 60
#define PLR_TKH 33
#define PLR_TXX 70
	
CPlayerData::CPlayerData(int c, int t, SDL_Renderer* r){
	rend = r;
	img = NULL; name_m = NULL;
	set_name("");
	set_col(c);
	team = t;
}
CPlayerData::~CPlayerData(){
	if(img) delete img;
	if(name_m) delete name_m;
}
void CPlayerData::set_col(int i){
	col = i;
	if(img) delete img;
	img = new TankImg();
	generate_tank(col, rend, img);
}
int CPlayerData::get_col(){
	return col;
}
void CPlayerData::set_name(const char* n){
	name = n;
	if(name_m) delete name_m;
	name_m = new Msg(name.size()>0 ? name.c_str() : "---", {0,0,0,255}, FONT_NRM, rend);
}
std::string CPlayerData::get_name(){
	return name;
}
void CPlayerData::draw(int y){
	SDL_Rect r;
	r.w = PLR_TKW; r.h = PLR_TKH;
	r.x = PLR_X;
	r.y = y - r.h/2;
	SDL_RenderCopy(rend, img->image, NULL, &r);
	
	name_m->render_centered(PLR_X + PLR_TXX, y, AL_LEFT);
}

PlayerMenu::PlayerMenu(SDL_Renderer* r, MainScr* m) : SubMenu(r, m){
}
PlayerMenu::~PlayerMenu(){
	
}
void PlayerMenu::event(SDL_Event& e){
	SubMenu::event(e);
}
void PlayerMenu::draw(){
	draw_back(main->get_conn());
	
	if(!main->get_conn()){
		players.clear();
		ys.clear();
		return ;
	}
	
	int y = PLR_Y;
	double spd = 3;
	
	for(auto it = players.begin(); it != players.end(); it++){
		int id = it->first;
		follow(ys[id], y, spd);
		players[id]->draw(ys[id]);
		y += PLR_S;
	}
	
}
void PlayerMenu::lose_mfocus(){
	SubMenu::lose_mfocus();
}
void PlayerMenu::lose_kfocus(){
	
}
void PlayerMenu::add_player(int id, int c, int t){
	players.insert({id, new CPlayerData(c,t,rend)});
	ys.insert({id, PLR_SY});
}
void PlayerMenu::remove_player(int id){
	delete players[id];
	players.erase(id);
	ys.erase(id);
}
void PlayerMenu::update_name(int id, const char* name){
	players[id]->set_name(name);
}
void PlayerMenu::update_col(int id, int c){
	players[id]->set_col(c);
}

#define PLST_W 480
#define PLST_MAR 20

#define PLST_H 200

#define X_S 10
#define X_W 6
#define X_X PLST_W - 20
#define X_Y 20



#define K_W 50
#define K_H 50

#define K_M 5
#define KA_X 70
#define KS_X 200
#define K_Y 140

#define NAME_X 10
#define NAME_M 10
#define NAME_Y 10
#define NAME_H 40
#define NAME_W 300

#define COL_W 32
#define COL_H 32
#define COL_X 340
#define COL_Y 20
#define COL_ROW 3
	
PlayerSetting::PlayerSetting(SDL_Renderer* rn, int i, SettingMenu* upp){
	up = upp;
	ind = i;
	rend = rn;
	
	SDL_Texture* tar = SDL_GetRenderTarget(rend);
	for(int i=0;i<2;i++){
		cross[i] = SDL_CreateTexture(rend, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, X_S*2, X_S*2);
		SDL_SetRenderTarget(rend, cross[i]);
		SDL_SetTextureBlendMode(cross[i], SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(rend, 0,0,0,0);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, 192 - i*64,0,0,255);
		SDL_Rect r;
		r.x = X_S - X_W/2; r.y = 0; r.w = X_W; r.h = X_S*2;
		SDL_RenderFillRect(rend, &r);
		r.x = 0; r.y = X_S - X_W/2; r.w = X_S*2; r.h = X_W;
		SDL_RenderFillRect(rend, &r);
	}
	SDL_SetRenderTarget(rend, tar);
	
	x_prs = false;
	
	k_fd = k_bk = k_lt = k_rt = k_sht = NULL;
	update_keys();
	name_m = NULL;
	update_msg();
	
	col = 0;
	
	for(int i = 0; i<get_tex_num(); i++){
		cols.push_back(get_tex(i, rend));
		if(cols.back() == NULL) cols.back() = gen_uniform(rend, COL_W, COL_H, get_tank_col(i));
	}
	
	focus = FOCUS_NONE;
}
PlayerSetting::~PlayerSetting(){
	SDL_DestroyTexture(cross[0]);
	SDL_DestroyTexture(cross[1]);
	if(k_fd) delete k_fd;
	if(k_bk) delete k_bk;
	if(k_lt) delete k_lt;
	if(k_rt) delete k_rt;
	if(k_sht) delete k_sht;
	if(name_m) delete name_m;
	for(int i = 0; i<cols.size(); i++) SDL_DestroyTexture(cols[i]);
}
void PlayerSetting::update_msg(){
	if(name_m) delete name_m;
	name_m = new Msg(name.c_str(), {0,0,0,255}, FONT_NRM, rend);
}
bool PlayerSetting::get_msg_upd(){
	bool ret = msg_upd;
	msg_upd = false;
	return ret;
}

void PlayerSetting::update_keys(){
	if(k_fd == NULL || p.fd != get_keyset(ind).fd){
		if(k_fd) delete k_fd;
		k_fd = new Msg(get_sym(get_keyset(ind).fd), {0,0,0,255}, FONT_NRM, rend);
	}
	if(k_bk == NULL || p.bk != get_keyset(ind).bk){
		if(k_bk) delete k_bk;
		k_bk = new Msg(get_sym(get_keyset(ind).bk), {0,0,0,255}, FONT_NRM, rend);
	}
	if(k_lt == NULL || p.lt != get_keyset(ind).lt){
		if(k_lt) delete k_lt;
		k_lt = new Msg(get_sym(get_keyset(ind).lt), {0,0,0,255}, FONT_NRM, rend);
	}
	if(k_rt == NULL || p.rt != get_keyset(ind).rt){
		if(k_rt) delete k_rt;
		k_rt = new Msg(get_sym(get_keyset(ind).rt), {0,0,0,255}, FONT_NRM, rend);
	}
	if(k_sht == NULL || p.sht != get_keyset(ind).sht){
		if(k_sht) delete k_sht;
		k_sht = new Msg(get_sym(get_keyset(ind).sht), {0,0,0,255}, FONT_NRM, rend);
	}
}
SDL_Rect PlayerSetting::get_rect(Focus f){
	SDL_Rect r;
	r.w = K_W; r.h = K_H;
	r.x = KA_X; r.y = K_Y;
	switch(f){
	case FOCUS_FD:
		r.y -= K_M + K_H;
		break;
	case FOCUS_BK:
		break;
	case FOCUS_RT:
		r.x -= K_M + K_W;
		break;
	case FOCUS_LT:
		r.x += K_M + K_W;
		break;
	case FOCUS_SHT:
		r.x = KS_X;
		break;
	case FOCUS_NONE:
		r.w = r.h = r.x = r.y = 0;
		break;
	case FOCUS_NAME:
		r.x = NAME_X; r.y = NAME_Y; r.w = NAME_W; r.h = NAME_H;
		break;
	case FOCUS_COL:
		r.x = COL_X; r.y = COL_Y;
		r.w = COL_W * (focus == FOCUS_COL ? COL_ROW : 1);
		r.h = COL_H * (focus == FOCUS_COL ? (get_tex_num()-1)/COL_ROW+1 : 1);
	}
	return r;
}
SDL_Rect PlayerSetting::get_xr(){
	SDL_Rect r;
	r.w = r.h = X_S*10/7;
	r.x = X_X - r.w/2; r.y = X_Y - r.h/2;
	return r;
}
bool PlayerSetting::event(SDL_Event& e){
	switch(e.type){
	case SDL_MOUSEMOTION:
		m_x = e.motion.x; m_y = e.motion.y;
		break;
	case SDL_MOUSEBUTTONDOWN:
		if(e.button.button == SDL_BUTTON_LEFT){
			if(in_rect(get_rect(FOCUS_COL), m_x, m_y) && up->get_main()->get_conn()){
				if(focus == FOCUS_COL){
					SDL_Rect r = get_rect(FOCUS_COL);
					int i = (m_x-r.x)/COL_W + ((m_y-r.y)/COL_H)*COL_ROW;
					if(i<get_tex_num()){
						up->get_main()->update_col(up->get_ind(this), i>0 ? (i<=col ? i-1 : i) : col);
						focus = FOCUS_NONE;
					}
				}
				else focus = FOCUS_COL;
			}
			else{
				focus = FOCUS_NONE;
				if(in_rect(get_xr(), m_x, m_y)) x_prs = true;
				if(in_rect(get_rect(FOCUS_BK), m_x, m_y)) focus = FOCUS_BK;
				if(in_rect(get_rect(FOCUS_FD), m_x, m_y)) focus = FOCUS_FD;
				if(in_rect(get_rect(FOCUS_RT), m_x, m_y)) focus = FOCUS_RT;
				if(in_rect(get_rect(FOCUS_LT), m_x, m_y)) focus = FOCUS_LT;
				if(in_rect(get_rect(FOCUS_SHT), m_x, m_y)) focus = FOCUS_SHT;
				if(in_rect(get_rect(FOCUS_NAME), m_x, m_y)) focus = FOCUS_NAME;
			}
			msg_upd = true;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if(e.button.button == SDL_BUTTON_LEFT && in_rect(get_xr(), m_x, m_y) && x_prs) return true;
		break;
	case SDL_KEYDOWN:
		switch(focus){
		case FOCUS_BK:
			get_keyset(ind).bk = e.key.keysym.sym;
			update_keys();
			focus = FOCUS_NONE;
			break;
		case FOCUS_FD:
			get_keyset(ind).fd = e.key.keysym.sym;
			update_keys();
			focus = FOCUS_NONE;
			break;
		case FOCUS_RT:
			get_keyset(ind).rt = e.key.keysym.sym;
			update_keys();
			focus = FOCUS_NONE;
			break;
		case FOCUS_LT:
			get_keyset(ind).lt = e.key.keysym.sym;
			update_keys();
			focus = FOCUS_NONE;
			break;
		case FOCUS_SHT:
			get_keyset(ind).sht = e.key.keysym.sym;
			update_keys();
			focus = FOCUS_NONE;
			break;
		case FOCUS_NAME:
			switch(e.key.keysym.sym){
			case SDLK_BACKSPACE:
			case SDLK_KP_BACKSPACE:
				if(name.size()>0){ utf8_pop_back(name); update_msg(); }
				break;
			case SDLK_RETURN:
			case SDLK_KP_ENTER:
				focus = FOCUS_NONE;
				msg_upd = true;
				break;
			}
			break;
		}
		break;
	case SDL_TEXTINPUT:
		if(focus == FOCUS_NAME){
			name += e.text.text;
			update_msg();
		}
	}
	return false;
}
void PlayerSetting::draw(){
	SDL_SetRenderDrawColor(rend, 192,192,192,192);
	SDL_RenderClear(rend);
	
	if(!(SDL_GetMouseState(NULL,NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))) x_prs = false;
	
	SDL_Rect r;
	r.x = X_X-X_S; r.y = X_Y-X_S;
	r.w = r.h = X_S*2;
	SDL_RenderCopyEx(rend, cross[x_prs ? 1 : 0], NULL, &r, 45, NULL, SDL_FLIP_NONE);
	
	r.w = K_W; r.h = K_H;
	int c;
	
	r = get_rect(FOCUS_BK);
	c = focus != FOCUS_BK ? 128 : 100;
	SDL_SetRenderDrawColor(rend, c,c,c,192);
	SDL_RenderFillRect(rend, &r);
	k_bk->render_centered(r.x + r.w/2, r.y + r.h/2, AL_CENTER);
	SDL_SetRenderDrawColor(rend, 0,0,0,255);
	if(in_rect(r,m_x,m_y)) SDL_RenderDrawRect(rend, &r);

	r = get_rect(FOCUS_FD);
	c = focus != FOCUS_FD ? 128 : 100;
	SDL_SetRenderDrawColor(rend, c,c,c,192);
	SDL_RenderFillRect(rend, &r);
	k_fd->render_centered(r.x + r.w/2, r.y + r.h/2, AL_CENTER);
	SDL_SetRenderDrawColor(rend, 0,0,0,255);
	if(in_rect(r,m_x,m_y)) SDL_RenderDrawRect(rend, &r);

	r = get_rect(FOCUS_RT);
	c = focus != FOCUS_RT ? 128 : 100;
	SDL_SetRenderDrawColor(rend, c,c,c,192);
	SDL_RenderFillRect(rend, &r);
	k_rt->render_centered(r.x + r.w/2, r.y + r.h/2, AL_CENTER);
	SDL_SetRenderDrawColor(rend, 0,0,0,255);
	if(in_rect(r,m_x,m_y)) SDL_RenderDrawRect(rend, &r);

	r = get_rect(FOCUS_LT);
	c = focus != FOCUS_LT ? 128 : 100;
	SDL_SetRenderDrawColor(rend, c,c,c,192);
	SDL_RenderFillRect(rend, &r);
	k_lt->render_centered(r.x + r.w/2, r.y + r.h/2, AL_CENTER);
	SDL_SetRenderDrawColor(rend, 0,0,0,255);
	if(in_rect(r,m_x,m_y)) SDL_RenderDrawRect(rend, &r);

	r = get_rect(FOCUS_SHT);
	c = focus != FOCUS_SHT ? 128 : 100;
	SDL_SetRenderDrawColor(rend, c,c,c,192);
	SDL_RenderFillRect(rend, &r);
	k_sht->render_centered(r.x + r.w/2, r.y + r.h/2, AL_CENTER);
	SDL_SetRenderDrawColor(rend, 0,0,0,255);
	if(in_rect(r,m_x,m_y)) SDL_RenderDrawRect(rend, &r);

	r = get_rect(FOCUS_NAME);
	c = focus != FOCUS_NAME ? 128 : 100;
	SDL_SetRenderDrawColor(rend, c,c,c,192);
	SDL_RenderFillRect(rend, &r);
	name_m->render_centered(r.x + NAME_M, r.y + r.h/2, AL_LEFT);
	SDL_SetRenderDrawColor(rend, 0,0,0,255);
	if(in_rect(r,m_x,m_y)) SDL_RenderDrawRect(rend, &r);
	
	if(up->get_main()->get_conn()){
		r = get_rect(FOCUS_COL);
		SDL_SetRenderDrawColor(rend, 128,128,128,192);
		SDL_RenderFillRect(rend, &r);
		SDL_Rect rr,ir;
		ir.x = ir.y = 0; ir.w = COL_W; ir.h = COL_H;
		rr.w = COL_W; rr.h = COL_H;
		rr.x = r.x; rr.y = r.y;
		SDL_RenderCopy(rend, cols[col], &ir, &rr);
		SDL_SetRenderDrawColor(rend, 0,0,0,255);
		if(focus == FOCUS_COL){
			if(in_rect(rr, m_x, m_y)) SDL_RenderDrawRect(rend, &rr);
			for(int i = 1; i<get_tex_num(); i++){
				rr.x = r.x + (COL_W)*(i%COL_ROW);
				rr.y = r.y + (COL_H)*(i/COL_ROW);
				SDL_RenderCopy(rend, cols[i <= col ? i-1 : i], &ir, &rr);
				if(in_rect(rr, m_x, m_y)) SDL_RenderDrawRect(rend, &rr);
			}
		}
	}
}
	
void PlayerSetting::lose_mfocus(){
	m_x = m_y = -1;
}
void PlayerSetting::lose_kfocus(){
	focus = FOCUS_NONE;
	msg_upd = true;
}
	
KeySet PlayerSetting::get_keys(){
	return get_keyset(ind);
}
	
int PlayerSetting::get_ind(){
	return ind;
}
const char* PlayerSetting::get_name(){
	return name.c_str();
}
int SettingMenu::get_player_num(){
	return players.size();
}
const char* SettingMenu::get_name(int i){
	return players[i]->get_name();
}

void PlayerSetting::update_col(int c){
	col = c;
}

#define CROSS_S 50
#define CROSS_W 20

SettingMenu::SettingMenu(SDL_Renderer* ren, MainScr* m) : SubMenu(ren, m){
	players_t = SDL_CreateTexture(	rend, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, 
									PLST_W + 2*PLST_MAR, SCR_H - 2*PLST_MAR);
									
	SDL_SetTextureBlendMode(players_t, SDL_BLENDMODE_BLEND);
	playera_t[0] = SDL_CreateTexture(	rend, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, 
										PLST_W, PLST_H);
	SDL_SetTextureBlendMode(playera_t[0], SDL_BLENDMODE_BLEND);
	playera_t[1] = SDL_CreateTexture(	rend, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, 
										PLST_W, PLST_H);
	SDL_SetTextureBlendMode(playera_t[1], SDL_BLENDMODE_BLEND);
									
	SDL_Texture* tar = SDL_GetRenderTarget(rend);
	SDL_SetRenderTarget(rend, playera_t[0]);
	SDL_SetRenderDrawColor(rend, 192,192,192,192);
	SDL_RenderClear(rend);
	SDL_SetRenderDrawColor(rend, 0, 128, 0, 255);
	
	SDL_Rect r;
	r.x = PLST_W/2 - CROSS_S; r.y = PLST_H/2 - CROSS_W/2; r.w = CROSS_S*2; r.h = CROSS_W;
	SDL_RenderFillRect(rend, &r);
	r.x = PLST_W/2 - CROSS_W/2; r.y = PLST_H/2 - CROSS_S; r.w = CROSS_W; r.h = CROSS_S*2;
	SDL_RenderFillRect(rend, &r);

	SDL_SetRenderTarget(rend, playera_t[1]);
	SDL_SetRenderDrawColor(rend, 128,128,128,192);
	SDL_RenderClear(rend);
	SDL_SetRenderDrawColor(rend, 0, 128, 0, 255);
	
	r.x = PLST_W/2 - CROSS_S; r.y = PLST_H/2 - CROSS_W/2; r.w = CROSS_S*2; r.h = CROSS_W;
	SDL_RenderFillRect(rend, &r);
	r.x = PLST_W/2 - CROSS_W/2; r.y = PLST_H/2 - CROSS_S; r.w = CROSS_W; r.h = CROSS_S*2;
	SDL_RenderFillRect(rend, &r);
	
	SDL_SetRenderTarget(rend, tar);
	
	a_prs = false;
	
	mfocus = kfocus = NULL;

	add_player();
}
SettingMenu::~SettingMenu(){
	for(int i = 0; i<players.size(); i++) {
		delete players[i];
		SDL_DestroyTexture(player_ts[i]);
	}
	SDL_DestroyTexture(players_t);
	SDL_DestroyTexture(playera_t[0]);
	SDL_DestroyTexture(playera_t[1]);
}
SDL_Rect SettingMenu::get_players_rect(){
	SDL_Rect r;
	r.x = (SETT_W - PLST_W - 2*PLST_MAR)/2;
	r.y = PLST_MAR;
	r.w = PLST_W + 2*PLST_MAR;
	r.h = SCR_H - 2*PLST_MAR;
	return r;
}
void SettingMenu::set_mfocus(){
	
	PlayerSetting* s = NULL;
	SDL_Rect pr = get_players_rect();
	if(in_rect(pr,m_x, m_y)){
		SDL_Rect r;
		r.w = PLST_W; r.h = PLST_H; r.x = PLST_MAR+pr.x; r.y = PLST_MAR+pr.y;
		for(int i = 0; i<players.size(); i++, r.y += PLST_H + PLST_MAR){
			if(in_rect(r, m_x, m_y)){
				s = players[i];
				mfi = i;
				break;
			}
		}
	}
	
	if(mfocus && s!=mfocus) mfocus->lose_mfocus();
	mfocus = s;
}
	
void SettingMenu::set_kfocus(){
	if(kfocus && kfocus->get_msg_upd()) main->update_name(get_ind(kfocus));
	
	
	if(kfocus && kfocus!=mfocus) kfocus->lose_kfocus();
	kfocus = mfocus;
}
void SettingMenu::draw(){
	draw_back(true);
	
	if(!(SDL_GetMouseState(NULL,NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))) a_prs = false; 
	
	SDL_Texture* tar = SDL_GetRenderTarget(rend);
	SDL_SetRenderTarget(rend, players_t);
	SDL_SetRenderDrawColor(rend, 255,255,255,0);
	SDL_RenderClear(rend);
	SDL_Rect r;
	r.x = PLST_MAR; r.y = PLST_MAR; r.w = PLST_W; r.h = PLST_H;
	for(int i = 0; i<players.size(); i++, r.y += PLST_H + PLST_MAR){
		SDL_SetRenderTarget(rend, player_ts[i]);
		players[i]->draw();
		
		SDL_SetRenderTarget(rend, players_t);
		SDL_RenderCopy(rend, player_ts[i], NULL, &r);
	}
	SDL_RenderCopy(rend, playera_t[a_prs ? 1 : 0], NULL, &r);
	SDL_SetRenderDrawColor(rend, 0,0,0,255);
	if(in_rect(r, m_x, m_y)) SDL_RenderDrawRect(rend, &r);
	
	SDL_SetRenderTarget(rend, tar);
	r = get_players_rect();
	SDL_RenderCopy(rend, players_t, NULL, &r);
	
}	
void SettingMenu::event(SDL_Event& e){
	SubMenu::event(e);
	SDL_Rect pr = get_players_rect();
	
	// event propagation
	switch(e.type){
		case SDL_MOUSEMOTION:
			set_mfocus();
			if(mfocus){
				e.motion.x -= pr.x + PLST_MAR;
				e.motion.y -= pr.y + PLST_MAR + (PLST_MAR+PLST_H)*mfi;
				mfocus->event(e);
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			set_kfocus();
		case SDL_MOUSEBUTTONUP:
			if(mfocus){
				e.button.x -= pr.x + PLST_MAR;
				e.button.y -= pr.y + PLST_MAR + (PLST_MAR+PLST_H)*mfi;
			}
		case SDL_MOUSEWHEEL:
			if(mfocus) if(mfocus->event(e)){
				non_used_inds.insert(players[mfi]->get_ind());
				delete players[mfi];
				main->remove_player(mfi);
				if(kfocus == players[mfi]) kfocus = NULL;
				SDL_DestroyTexture(player_ts[mfi]);
				players.erase(players.begin() + mfi);
				player_ts.erase(player_ts.begin() + mfi);
				set_mfocus();
			}
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_TEXTINPUT:
			if(kfocus) kfocus->event(e);
	}
	
	// evnet handling
	SDL_Rect ar;
	ar.x = pr.x + PLST_MAR;
	ar.y = pr.y + PLST_MAR + (PLST_MAR+PLST_H)*players.size();
	ar.w = PLST_W; ar.h = PLST_H;
	switch(e.type){
		case SDL_MOUSEBUTTONDOWN:
			if(e.button.button == SDL_BUTTON_LEFT && in_rect(ar,m_x,m_y)){
				a_prs = true;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if(e.button.button == SDL_BUTTON_LEFT && in_rect(ar, m_x, m_y) && a_prs){
				a_prs = false;
				add_player();
			}
			break;
	}
	if(kfocus && kfocus->get_msg_upd()) main->update_name(get_ind(kfocus));
}
void SettingMenu::lose_mfocus(){
	SubMenu::lose_mfocus();
	if(mfocus) mfocus->lose_mfocus();
	mfocus = NULL;
}
void SettingMenu::lose_kfocus(){
	if(kfocus) kfocus->lose_kfocus();
	kfocus = NULL;
}
int SettingMenu::get_ind(PlayerSetting* pl){
	for(int i = 0; i<players.size(); i++) if(players[i] == pl) return i;
	return -1;
}

void SettingMenu::add_player(){
	players.push_back(new PlayerSetting(rend,
										(non_used_inds.size() > 0) ? *(non_used_inds.begin()) : players.size(), this));
	player_ts.push_back(SDL_CreateTexture(	rend, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET,
											PLST_W, PLST_H));
	SDL_SetTextureBlendMode(player_ts.back(), SDL_BLENDMODE_BLEND);
	if(non_used_inds.size()>0) non_used_inds.erase(non_used_inds.begin());
	main->add_player();
}
void SettingMenu::update_col(int i,int col){
	players[i]->update_col(col);
}
	
MainScr* SettingMenu::get_main(){
	return main;
}

MainScr::MainScr(Main* up, Client* c) : State(up), conn(up->get_renderer(),this), play(up->get_renderer(),this), sett(up->get_renderer(),this){
	clnt = c;
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
	
	ihost = iconn = false;
	
	conn_timer = -1;
	
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
bool MainScr::get_conn_try(){
	return clnt != NULL;
}
bool MainScr::get_conn(){
	return iconn;
}
bool MainScr::get_host(){
	return ihost;
}
void MainScr::m_correct(int& x, int& y){
	if(x >= SCR_W - SETT_W) x-= SCR_W - SETT_W;
	else if(y >= SERV_H) y -= SERV_H;
}
void MainScr::connect(){
	clnt = new Client(conn.get_address());
	conn_timer = 5*60;
}
void MainScr::leave(){
	delete clnt;
	clnt = NULL;
	iconn = ihost = false;
}
	
bool MainScr::step(){
	SDL_Event e;
	
	
	
	while(SDL_PollEvent(&e)){
		switch(e.type){
		case SDL_QUIT:
			if(clnt) leave();
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
				if(clnt) leave();
				return true;
			}
		case SDL_KEYUP:
		case SDL_TEXTINPUT:
			kfocus->event(e);
		}
	}
	if(clnt){
		if(conn_timer>0) conn_timer--;
		else if(conn_timer==0){
			conn.set_err("Failed to connect to server");
			leave();
		}
		NetEvent e;
		while(true){
			if(clnt == NULL) break;
			if(clnt->is_error()){
				conn.set_err(clnt->get_error());
				leave();
				break;
			}
			e=clnt->get_event();
			char* cur;
			char h,hh, str[1000];
			int i,c,t;
			if(e.type == NetEvent::TYPE_NONE) break;
			switch(e.type){
			case NetEvent::TYPE_CONN:
				iconn = true;
				send_players();
				conn_timer = -1;
				break;
			case NetEvent::TYPE_RECV:
				cur = e.data;
				cur = decode_char(cur, h);
				switch(h){
				case '\x00':
					cur = decode_char(cur, hh);
					switch(hh){
					case '\x00':
						break;
					case '\x01':
						cur = decode_int(cur, i);
						cur = decode_int(cur, t);
						cur = decode_int(cur, c);
						play.add_player(i,c,t);
						break;
					case '\x02':
						cur = decode_int(cur, i);
						play.remove_player(i);
						break;
					case '\x03':
						cur = decode_int(cur, i);
						cur = decode_str(cur, str);
						play.update_name(i, str);
						break;
					case '\x04':
						cur = decode_int(cur, i);
						cur = decode_int(cur, c);
						play.update_col(i, c);
						break;
					case '\x05':
						cur = decode_int(cur, i);
						cur = decode_int(cur, c);
						sett.update_col(i, c);
						break;
					}
					break;
				}
				
			
				delete e.data;
				break;
			case NetEvent::TYPE_DISC:
				conn.set_err("Server disconnected");
				leave();
				break;
			}
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

void MainScr::send_players(){
	for(int i = 0; i<sett.get_player_num(); i++){
		add_player();
		update_name(i);
	}
}

void MainScr::add_player(){
	if(!iconn) return;
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x01');
	clnt->send(data, end-data, PROTO_REL);
}
void MainScr::remove_player(int ind){
	if(!iconn) return;
	
	char data[100];
	char* end;
	
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x02');
	end = encode_int(end, ind);
	clnt->send(data, end-data, PROTO_REL);
}
void MainScr::update_name(int i){
	if(!iconn) return;
	char data[100];
	char* end;
		
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x03');
	end = encode_int(end, i);
	end = encode_str(end, sett.get_name(i));
	clnt->send(data, end-data, PROTO_REL);
}
void MainScr::update_col(int i, int col){
	if(!iconn) return;
	
	char data[100];
	char* end;
				
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x04');
	end = encode_int(end, i);
	end = encode_int(end, col);
	clnt->send(data, end-data, PROTO_REL);
}