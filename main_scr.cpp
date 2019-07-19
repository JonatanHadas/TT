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

#define AR_H 20
#define UP "\xe2\x96\xb2"
#define DOWN "\xe2\x96\xbc"

#include "game_extrap.h"
#include "e_game_query.h"
#include "game_gui.h"
#include "net_ex.h"


bool in_rect(SDL_Rect r, int x, int y){
	return x>=r.x && y>=r.y && x < r.x+r.w && y < r.y+r.h;
}

void follow(double& val, double tar, double spd){
	double dv = tar - val;
	int dir = dv>0 ? 1 : -1;
	dv = std::abs(dv);
	dv = std::min(dv, spd*sqrt(dv));
	val += dv*dir;
}

SDL_Rect NumberField::u_rect(){
	SDL_Rect rr = r;
	rr.h = AR_H;
	return rr;
}
SDL_Rect NumberField::b_rect(){
	SDL_Rect rr = u_rect();
	rr.y += r.h-rr.h;
	return rr;
}
void NumberField::cut(){
	if(num<lower && use_l) num = lower;
	if(num>upper && use_u) num = upper;
}
void NumberField::upd_msg(){
	char s[10];
	sprintf(s, "%d", num);
	if(msg) delete msg;
	msg = new Msg(s, {0,0,0,active ? 255 : 128}, FONT_NRM, rend);
}
NumberField::NumberField(SDL_Renderer* ren, SDL_Rect rect, int v){
	rend = ren;
	r = rect;
	use_l = use_u = false;
	active = true;
	num = v;
	msg = NULL;
	up = new Msg(UP, {0,0,0,255}, FONT_NRM, rend);
	iup = new Msg(UP, {0,0,0,128}, FONT_NRM, rend);
	down = new Msg(DOWN, {0,0,0,255}, FONT_NRM, rend);
	idown = new Msg(DOWN, {0,0,0,128}, FONT_NRM, rend);
	upd_msg();
}
NumberField::NumberField(SDL_Renderer* ren, SDL_Rect r, int v, int lowerb, int upperb) : NumberField(ren, r,v){
	lower = lowerb; upper = upperb;
	use_l = use_u = true;
}
NumberField::NumberField(SDL_Renderer* ren, SDL_Rect r, int v, int bound, bool islower) : NumberField(ren, r,v){
	lower = upper = bound;
	use_l = islower;
	use_u = !use_l;
}
NumberField::~NumberField(){
	if(msg) delete msg;
	delete up,down, iup, idown;
}

void NumberField::draw(){
	SDL_Rect rr;
	
	if(!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))) d_prs = u_prs = false;
	
	rr = u_rect();
	SDL_SetRenderDrawColor(rend, 192,192,192,64);
	if(u_prs) SDL_RenderFillRect(rend, &rr);
	(active && (num<upper || !use_u) ? up : iup)->render_centered(rr.x + rr.w/2, rr.y + rr.h/2, AL_CENTER);

	rr = b_rect();
	if(d_prs) SDL_RenderFillRect(rend, &rr);
	(active && (num>lower || !use_l)
	? down : idown)->render_centered(rr.x + rr.w/2, rr.y + rr.h/2, AL_CENTER);

	msg->render_centered(r.x + r.w/2, r.y + r.h/2, AL_CENTER);
}
void NumberField::event(SDL_Event& e){
	switch(e.type){
	case SDL_MOUSEBUTTONDOWN:
		if(in_rect(u_rect(), e.button.x, e.button.y) && active) u_prs = true;
		if(in_rect(b_rect(), e.button.x, e.button.y) && active) d_prs = true;
		break;
	case SDL_MOUSEBUTTONUP:
		if(in_rect(u_rect(), e.button.x, e.button.y) && u_prs && active){
			u_prs = false;
			set_num(num+1);
		}
		if(in_rect(b_rect(), e.button.x, e.button.y) && d_prs && active){
			d_prs = false;
			set_num(num-1);
		}
		break;
	case SDL_MOUSEWHEEL:
		if(active) set_num(num+e.wheel.y * (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1));
		break;
	}
}

int NumberField::get_num(){
	return num;
}
void NumberField::set_num(int n){
	num = n;
	cut();
	upd_msg();
	upd = true;
}
void NumberField::set_active(bool a){
	active = a;
	upd_msg();
}
bool NumberField::get_upd(){
	bool ret = upd;
	upd = false;
	return ret;
}
SDL_Rect NumberField::get_rect(){
	return r;
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

#define LOAD_T 32

ConnectionMenu::ConnectionMenu(SDL_Renderer* r, MainScr* m, const char* ad) : SubMenu(r, m), addr(ad ? ad : ""){
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
			if(in_rect(r_start(), e.button.x, e.button.y) && main->get_conn()) st_prs = true;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if(e.button.button == SDL_BUTTON_LEFT){
			if(in_rect(r_leave(), e.button.x, e.button.y) && main->get_conn() && lv_prs) {main->leave(); lv_prs = false;}
			if(in_rect(r_start(), e.button.x, e.button.y) && main->get_conn() && st_prs){
				if(main->get_host() && num < 0) main->start_count();
				else main->stop_count();
				st_prs = false;
			}
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
void ConnectionMenu::set_cnt(int n){
	num = n;
}

#define PLR_X 30
#define PLR_Y 30
#define PLR_S 40
#define SEP_S 20
#define PLR_SY 900

#define PLR_TKW 60
#define PLR_TKH 33
#define PLR_TXX 70

#define HST_X  SCR_W-SETT_W-30

#define RUL_DY SEP_S-PLR_S
#define RUL_X 40
#define RUL_W SCR_W-SETT_W-2*RUL_X

#define UD_X SCR_W-SETT_W-100
#define UD_DY 10
#define UD_W 20
#define UD_H 20
	
CPlayerMData::CPlayerMData(int c, int t, SDL_Renderer* r){
	rend = r;
	img = NULL; name_m = NULL;
	set_name("");
	set_col(c);
	team = t;
	our = host = false;
	hst = new Msg("host", {0,0,0,255}, FONT_NRM, rend);
	up = new Msg(UP, {0,0,0,255}, FONT_NRM, rend);
	dn= new Msg(DOWN, {0,0,0,255}, FONT_NRM, rend);
}
CPlayerMData::~CPlayerMData(){
	if(img) delete img;
	if(name_m) delete name_m;
	delete hst;
	delete up,dn;
}
void CPlayerMData::set_col(int i){
	col = i;
	if(img) delete img;
	img = new TankImg();
	generate_tank(col, rend, img);
}
int CPlayerMData::get_col(){
	return col;
}
void CPlayerMData::set_team(int t){
	team = t;
}
int CPlayerMData::get_team(){
	return team;
}
void CPlayerMData::set_name(const char* n){
	name = n;
	if(name_m) delete name_m;
	name_m = new Msg(name.size()>0 ? name.c_str() : "---", {0,0,0,255}, FONT_NRM, rend);
}
std::string CPlayerMData::get_name(){
	return name;
}
void CPlayerMData::draw(int y, bool use_teams){
	SDL_Rect r;
	r.w = PLR_TKW; r.h = PLR_TKH;
	r.x = PLR_X;
	r.y = y - r.h/2;
	SDL_RenderCopy(rend, img->image, NULL, &r);
	
	name_m->render_centered(PLR_X + PLR_TXX, y, AL_LEFT);
	
	if(host) hst->render_centered(HST_X, y, AL_RIGHT);
	
	if(our && use_teams){
		up->render_centered(UD_X, y - UD_DY, AL_CENTER);
		dn->render_centered(UD_X, y + UD_DY, AL_CENTER);
	}
}
void CPlayerMData::set_host(){
	host = true;
}
void CPlayerMData::set_our(){
	our = true;
}
bool CPlayerMData::get_our(){
	return our;
}

PlayerMenu::PlayerMenu(SDL_Renderer* r, MainScr* m) : SubMenu(r, m){
}
PlayerMenu::~PlayerMenu(){
	for(auto it = players.begin(); it != players.end(); it++){
		delete it->second;
	}
}
void PlayerMenu::event(SDL_Event& e){
	SubMenu::event(e);
	switch(e.type){
	case SDL_MOUSEBUTTONDOWN:
		SDL_Rect r;
		r.w = UD_W;
		r.x = UD_X - r.w/2;
		r.h = UD_H;
		for(auto it = players.begin(); it != players.end(); it++){
			int id = it->first;
			r.y = ys[id] - r.h/2 - UD_DY;
			if(in_rect(r, m_x, m_y) && players[id]->get_team()>0){
				players[id]->set_team(players[id]->get_team()-1);
				main->change_team(id, players[id]->get_team());
			}
			r.y = ys[id] - r.h/2 + UD_DY;
			if(in_rect(r, m_x, m_y) && players[id]->get_our() && players[id]->get_team()<tys.size()){
				players[id]->set_team(players[id]->get_team()+1);
				main->change_team(id, players[id]->get_team());
			}
		}

	}
}
void PlayerMenu::draw(){
	draw_back(main->get_conn());
	
	if(!main->get_conn()){
		players.clear();
		ys.clear();
		return ;
	}
	
	std::vector<std::vector<int>> ids(tys.size()+1);
	for(auto it = players.begin(); it != players.end(); it++){
		ids[it->second->get_team()].push_back(it->first);
	}

	int y = PLR_Y;
	double spd = 3;
	for(int i = 0; i<ids.size(); i++){
		for(int j = 0; j<ids[i].size(); j++){
			int id = ids[i][j];
			follow(ys[id], y, spd);
			players[id]->draw(ys[id], use_teams);
			y += PLR_S;
		}
		if(i < tys.size()){
			follow(tys[i], y, spd);
			if(use_teams){
				SDL_Rect r;
				r.y = tys[i] + RUL_DY;
				r.x = RUL_X; r.w = RUL_W; r.h = 1;
				SDL_SetRenderDrawColor(rend, 0,0,0,255);
				SDL_RenderDrawRect(rend, &r);
				y += SEP_S;
			}
		}
	}
	
}
void PlayerMenu::lose_mfocus(){
	SubMenu::lose_mfocus();
}
void PlayerMenu::lose_kfocus(){
	
}
void PlayerMenu::add_player(int id, int c, int t){
	players.insert({id, new CPlayerMData(c,t,rend)});
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
void PlayerMenu::set_host(int id){
	players[id]->set_host();
}
void PlayerMenu::set_our(int id){
	players[id]->set_our();
}
void PlayerMenu::set_team_num(int num){
	while(tys.size() < num-1) tys.push_back(PLR_SY);
	while(tys.size() > num-1) tys.pop_back();
}
void PlayerMenu::set_use_teams(bool use){
	use_teams = use;
}
void PlayerMenu::set_team(int id, int t){
	players[id]->set_team(t);
}
std::map<int, CPlayerMData*>::iterator PlayerMenu::get_players(){
	return players.begin();
}
std::map<int, CPlayerMData*>::iterator PlayerMenu::end_players(){
	return players.end();
}
int PlayerMenu::get_player_num(){
	return players.size();
}
int PlayerMenu::get_team_num(){
	return tys.size()+1;
}
int PlayerMenu::get_use_teams(){
	return use_teams;
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

PlayerSetting::PlayerSetting(SDL_Renderer* rend, SettingMenu* up, PlayerMData d) : PlayerSetting(rend, d.ind, up){
	name = d.name;
	update_msg();
	update_col(d.col);
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
	if(focus == FOCUS_NAME) 
	focus = FOCUS_NONE;
	msg_upd = true;
}
	
KeySet PlayerSetting::get_keys(){
	return get_keyset(ind);
}
	
int PlayerSetting::get_ind(){
	return ind;
}
int PlayerSetting::get_col(){
	return col;
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
int SettingMenu::get_col(int i){
	return players[i]->get_col();
}

void PlayerSetting::update_col(int c){
	col = c;
}
int PlayerSetting::get_id(){
	return id;
}
void PlayerSetting::set_id(int i){
	id = i;
}

#define CROSS_S 50
#define CROSS_W 20

#define SET_H 350
#define SCR_S 50

#define MSG_X 10
#define MSG_H 40
#define MSG_XX 100
#define MSG_M 10

#define SCR_Y 30
#define SCR_LST_W 130
#define SCR_ORD_W 150
#define SCR_DTH_W 140

#define END_Y 70
#define END_NON_W 70
#define END_RND_W 120
#define END_SCORE_W 120

#define LIM_X 100
#define LIM_Y 100
#define LIM_W 50
#define LIM_H 60

#define TIE_X 300
#define TIE_Y 150
#define TIE_W 50
#define TIE_H 60

#define TEAM_X 180
#define TEAM_Y 220
#define TEAM_W 50
#define TEAM_H 60

#define TEAM_NW 80
#define TEAM_YW 80
#define TEAM_MH 40

#define UPG_X 250
#define UPG_Y 214
#define UPG_SZ 32
#define UPG_SP 8
#define UPG_LNS 2
#define UPG_PL ((UPG_NUM-1)/UPG_LNS + 1)

SettingMenu::SettingMenu(SDL_Renderer* ren, MainScr* m, MainData* data) : SubMenu(ren, m), tie_lim(ren,{TIE_X,TIE_Y,TIE_W, TIE_H},0,0,2), game_lim(ren,{LIM_X, LIM_Y, LIM_W, LIM_H},10,0,true), team_num(ren, {TEAM_X, TEAM_Y, TEAM_W, TEAM_H},2,2,true){
	players_t = SDL_CreateTexture(	rend, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, 
									PLST_W + 2*PLST_MAR, SCR_H - 2*PLST_MAR);
	game_t = SDL_CreateTexture(	rend, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, 
									PLST_W + 2*PLST_MAR, SET_H - PLST_MAR);
									
	SDL_SetTextureBlendMode(game_t, SDL_BLENDMODE_BLEND);
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
	
	scr_y = scr_t = p = 0;
	
	scr_m = new Msg("Score by:", {0,0,0,255}, FONT_NRM, rend);
	scr_lst = new Msg("Last Servivor", {0,0,0,255}, FONT_NRM, rend);
	scr_ord = new Msg("Order of Death", {0,0,0,255}, FONT_NRM, rend);
	scr_dth = new Msg("Single Deaths", {0,0,0,255}, FONT_NRM, rend);
	end_m = new Msg("Ends:", {0,0,0,255}, FONT_NRM, rend);
	end_non = new Msg("Never", {0,0,0,255}, FONT_NRM, rend);
	end_rnd = new Msg("After round", {0,0,0,255}, FONT_NRM, rend);
	end_scr = new Msg("After score", {0,0,0,255}, FONT_NRM, rend);
	tie_msg = new Msg("Tie break under difference of:", {0,0,0,255}, FONT_NRM, rend);
	teams = new Msg("Teams", {0,0,0,255}, FONT_NRM, rend);
	nteams = new Msg("Singles", {0,0,0,255}, FONT_NRM, rend);
	
	end_mth = GameSettings::END_NONE;
	scr_mth = GameSettings::SCR_LAST;
	ux = uw = sx = sw = ex = ew = 0;
	
	tie_lim.set_active(false);
	game_lim.set_active(false);
	team_num.set_active(false);
	use_teams = false;
	
	upg_mask = UPG_MASK_ALL;
	upg_prs_ind = -1;

	if(data){
		for(int i = 0; i<data->players.size(); i++){
			add_player(data->players[i]);
		}
	}
	else add_player();
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
	r.y = PLST_MAR + y;
	r.w = PLST_W + 2*PLST_MAR;
	r.h = SCR_H - 2*PLST_MAR - y;
	return r;
}
SDL_Rect SettingMenu::get_game_rect(){
	SDL_Rect r;
	r.x = (SETT_W - PLST_W - 2*PLST_MAR)/2;
	r.y = PLST_MAR + y - SET_H + PLST_MAR;
	r.w = PLST_W + 2*PLST_MAR;
	r.h = SET_H - PLST_MAR;
	return r;
}

SDL_Rect SettingMenu::scr_rect(GameSettings::ScoreMeth scr){
	SDL_Rect r;
	r.x = MSG_XX; r.y = SCR_Y - MSG_H/2; r.h = MSG_H;
	switch(scr){
		case GameSettings::SCR_DEATH:
			r.x += SCR_LST_W + SCR_ORD_W;
			r.w = SCR_DTH_W;
			break;
		case GameSettings::SCR_ORDER:
			r.x += SCR_LST_W;
			r.w = SCR_ORD_W;
			break;
		case GameSettings::SCR_LAST:
			r.w = SCR_LST_W;
			break;
	}
	return r;
}

SDL_Rect SettingMenu::end_rect(GameSettings::EndMeth mth){
	SDL_Rect r;
	r.x = MSG_XX; r.y = END_Y - MSG_H/2; r.h = MSG_H;
	switch(mth){
		case GameSettings::END_SCORE:
			r.x += END_NON_W + END_RND_W;
			r.w = END_SCORE_W;
			break;
		case GameSettings::END_ROUND:
			r.x += END_NON_W;
			r.w = END_RND_W;
			break;
		case GameSettings::END_NONE:
			r.w = END_NON_W;
			break;
	}
	return r;
}

SDL_Rect SettingMenu::use_rect(bool use){
	SDL_Rect r;
	r.x = MSG_X + (use ? TEAM_NW : 0);
	r.y = TEAM_Y + (TEAM_H - TEAM_MH)/2;
	r.w = use ? TEAM_YW : TEAM_NW;
	r.h = TEAM_MH;
	return r;
}

SDL_Rect SettingMenu::upg_rect(int ind){
	SDL_Rect r;
	r.h = r.w = UPG_SZ;
	int ri = ind/UPG_PL, ci = ind%UPG_PL;
	r.x = UPG_X + ci*(r.w + UPG_SP);
	r.y = UPG_Y + ri*(r.h + UPG_SP);;
	return r;
}


void SettingMenu::set_mfocus(){
	
	PlayerSetting* s = NULL;
	SDL_Rect pr = get_players_rect();
	if(in_rect(pr,m_x, m_y)){
		SDL_Rect r;
		r.w = PLST_W; r.h = PLST_H; r.x = PLST_MAR+pr.x; r.y = PLST_MAR+pr.y - scr_y;
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
	if(kfocus && (mfocus!=kfocus || kfocus->get_msg_upd())) main->update_name(get_ind(kfocus));
	
	
	if(kfocus && kfocus!=mfocus) kfocus->lose_kfocus();
	kfocus = mfocus;
}

Img upg_imgs[UPG_NUM] = {
	IMG_GATLING_SYM,
	IMG_LASER_SYM,
	IMG_BOMB_SYM,
	IMG_DEATH_RAY_SYM,
	IMG_WIFI_SYM,
	IMG_MISSILE_SYM,
	IMG_MINE_SYM,
	};
void SettingMenu::draw(){
	draw_back(true);
	
	tie_lim.set_active(main->get_host());
	game_lim.set_active(main->get_host() && end_mth != GameSettings::END_NONE);
	team_num.set_active(main->get_host() && use_teams);
	
	int max_scr = PLST_MAR + (PLST_MAR+PLST_H)*(players.size()+1) - get_players_rect().h;
	if(scr_t > max_scr) scr_t = max_scr;
	if(scr_t < 0) scr_t = 0;
	follow(scr_y, scr_t, 5.0);
	follow(y, main->get_conn() ? SET_H : 0, 1.5);
	
	
	follow(sx, scr_rect(scr_mth).x, 1.5);
	follow(sw, scr_rect(scr_mth).w, 1.5);
	follow(ex, end_rect(end_mth).x, 1.5);
	follow(ew, end_rect(end_mth).w, 1.5);
	follow(ux, use_rect(use_teams).x, 1.5);
	follow(uw, use_rect(use_teams).w, 1.5);
	
	if(!(SDL_GetMouseState(NULL,NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))) a_prs = false; 
	
	SDL_Texture* tar = SDL_GetRenderTarget(rend);
	SDL_SetRenderTarget(rend, players_t);
	SDL_SetRenderDrawColor(rend, 255,255,255,0);
	SDL_RenderClear(rend);
	SDL_Rect r,sr;
	r.x = PLST_MAR; r.y = PLST_MAR - scr_y; r.w = PLST_W; r.h = PLST_H;
	for(int i = 0; i<players.size(); i++, r.y += PLST_H + PLST_MAR){
		SDL_SetRenderTarget(rend, player_ts[i]);
		players[i]->draw();
		
		SDL_SetRenderTarget(rend, players_t);
		SDL_RenderCopy(rend, player_ts[i], NULL, &r);
	}
	SDL_RenderCopy(rend, playera_t[a_prs ? 1 : 0], NULL, &r);
	SDL_SetRenderDrawColor(rend, 0,0,0,255);
	SDL_Rect rr = r;
	rr.y += y;
	if(in_rect(rr, m_x, m_y)) SDL_RenderDrawRect(rend, &r);
	
	SDL_SetRenderTarget(rend, tar);
	r = get_players_rect();
	sr.x = sr.y = 0; sr.w = r.w; sr.h = r.h;
	SDL_RenderCopy(rend, players_t, &sr, &r);
	
	SDL_SetRenderTarget(rend, game_t);
	
	SDL_SetRenderDrawColor(rend, 255,255,255,0);
	SDL_RenderClear(rend);
	
	scr_m->render_centered(MSG_X, SCR_Y, AL_LEFT);
	end_m->render_centered(MSG_X, END_Y, AL_LEFT);
	tie_msg->render_centered(MSG_X, TIE_Y + TIE_H/2, AL_LEFT);
	
	SDL_SetRenderDrawColor(rend, 192,192,192,64);
	r = scr_rect(GameSettings::SCR_LAST);
	r.x = sx; r.w = sw;
	SDL_RenderFillRect(rend, &r);
	r = end_rect(GameSettings::END_NONE);
	r.x = ex; r.w = ew;
	SDL_RenderFillRect(rend, &r);
	r = use_rect(true);
	r.x = ux; r.w = uw;
	SDL_RenderFillRect(rend, &r);

	scr_lst->render_centered(MSG_M + scr_rect(GameSettings::SCR_LAST).x, SCR_Y, AL_LEFT);
	scr_ord->render_centered(MSG_M + scr_rect(GameSettings::SCR_ORDER).x, SCR_Y, AL_LEFT);
	scr_dth->render_centered(MSG_M + scr_rect(GameSettings::SCR_DEATH).x, SCR_Y, AL_LEFT);
	end_non->render_centered(MSG_M + end_rect(GameSettings::END_NONE).x, END_Y, AL_LEFT);
	end_rnd->render_centered(MSG_M + end_rect(GameSettings::END_ROUND).x, END_Y, AL_LEFT);
	end_scr->render_centered(MSG_M + end_rect(GameSettings::END_SCORE).x, END_Y, AL_LEFT);
	
	nteams->render_centered(MSG_M + use_rect(false).x, TEAM_Y + TEAM_H/2, AL_LEFT);
	teams->render_centered(MSG_M + use_rect(true).x, TEAM_Y + TEAM_H/2, AL_LEFT);

	game_lim.draw();
	tie_lim.draw();
	team_num.draw();

	SDL_SetRenderDrawColor(rend, 0,0,0,255);
	if(in_rect(get_game_rect(), m_x, m_y) && main->get_host()){
		r = scr_rect(GameSettings::SCR_LAST);
		if(in_rect(r, m_x - get_game_rect().x, m_y - get_game_rect().y)) SDL_RenderDrawRect(rend, &r);
		r = scr_rect(GameSettings::SCR_ORDER);
		if(in_rect(r, m_x - get_game_rect().x, m_y - get_game_rect().y)) SDL_RenderDrawRect(rend, &r);
		r = scr_rect(GameSettings::SCR_DEATH);
		if(in_rect(r, m_x - get_game_rect().x, m_y - get_game_rect().y)) SDL_RenderDrawRect(rend, &r);
		r = end_rect(GameSettings::END_NONE);
		if(in_rect(r, m_x - get_game_rect().x, m_y - get_game_rect().y)) SDL_RenderDrawRect(rend, &r);
		r = end_rect(GameSettings::END_ROUND);
		if(in_rect(r, m_x - get_game_rect().x, m_y - get_game_rect().y)) SDL_RenderDrawRect(rend, &r);
		r = end_rect(GameSettings::END_SCORE);
		if(in_rect(r, m_x - get_game_rect().x, m_y - get_game_rect().y)) SDL_RenderDrawRect(rend, &r);
		r = use_rect(false);
		if(in_rect(r, m_x - get_game_rect().x, m_y - get_game_rect().y)) SDL_RenderDrawRect(rend, &r);
		r = use_rect(true);
		if(in_rect(r, m_x - get_game_rect().x, m_y - get_game_rect().y)) SDL_RenderDrawRect(rend, &r);
		r = game_lim.get_rect();
		if(in_rect(r, m_x - get_game_rect().x, m_y - get_game_rect().y)) SDL_RenderDrawRect(rend, &r);
		r = tie_lim.get_rect();
		if(in_rect(r, m_x - get_game_rect().x, m_y - get_game_rect().y)) SDL_RenderDrawRect(rend, &r);
		r = team_num.get_rect();
		if(in_rect(r, m_x - get_game_rect().x, m_y - get_game_rect().y)) SDL_RenderDrawRect(rend, &r);
	}
	
	
	for(int i = 0; i<UPG_NUM; i++){
		r = upg_rect(i);
		SDL_SetTextureAlphaMod(get_img(upg_imgs[i]), (upg_mask & (1<<i)) ? 255 : 128 );
		SDL_RenderCopy(rend, get_img(upg_imgs[i]), NULL, &r);
		SDL_SetTextureAlphaMod(get_img(upg_imgs[i]), 255);
		if(in_rect(get_game_rect(), m_x, m_y) && main->get_host())
		if(in_rect(r, m_x - get_game_rect().x, m_y - get_game_rect().y)) SDL_RenderDrawRect(rend, &r);
	}
	
	SDL_SetRenderTarget(rend, tar);
	
	r = get_game_rect();
	SDL_RenderCopy(rend, game_t, NULL, &r);
}	
void SettingMenu::event(SDL_Event& e){
	SubMenu::event(e);
	SDL_Rect pr = get_players_rect();
	SDL_Rect gr = get_game_rect();
	// event propagation
	switch(e.type){
		case SDL_MOUSEMOTION:
			set_mfocus();
			if(mfocus){
				e.motion.x -= pr.x + PLST_MAR;
				e.motion.y -= pr.y + PLST_MAR + (PLST_MAR+PLST_H)*mfi - scr_y;
				mfocus->event(e);
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			set_kfocus();
		case SDL_MOUSEBUTTONUP:
			if(mfocus){
				e.button.x -= pr.x + PLST_MAR;
				e.button.y -= pr.y + PLST_MAR + (PLST_MAR+PLST_H)*mfi - scr_y;
			}
			else if(in_rect(gr, m_x, m_y)){
				e.button.x -= gr.x;
				e.button.y -= gr.y;
			}
		case SDL_MOUSEWHEEL:
			if(mfocus) {
				if(mfocus->event(e)){
					non_used_inds.insert(players[mfi]->get_ind());
					delete players[mfi];
					main->remove_player(mfi);
					if(kfocus == players[mfi]) kfocus = NULL;
					SDL_DestroyTexture(player_ts[mfi]);
					players.erase(players.begin() + mfi);
					player_ts.erase(player_ts.begin() + mfi);
					set_mfocus();
				}
			}
			else if(in_rect(gr, m_x, m_y)){
				if( in_rect(game_lim.get_rect(), m_x-gr.x, m_y-gr.y)) {
					game_lim.event(e);
					main->set_game_lim(game_lim.get_num());
				}
				if( in_rect(tie_lim.get_rect(), m_x-gr.x, m_y-gr.y)){
					tie_lim.event(e);
					main->set_tie_lim(tie_lim.get_num());
				}
				if( in_rect(team_num.get_rect(), m_x-gr.x, m_y-gr.y)){
					team_num.event(e);
					main->set_team_num(team_num.get_num());
				}
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
			if(e.button.button == SDL_BUTTON_LEFT){
				if(in_rect(ar,m_x,m_y)) a_prs = true;
				if(in_rect(gr, m_x, m_y)){
					GameSettings::ScoreMeth scr;
					scr = GameSettings::SCR_LAST;
					if( in_rect(scr_rect(scr), m_x-gr.x, m_y-gr.y)) main->set_scr_mth(scr);
					scr = GameSettings::SCR_ORDER;
					if( in_rect(scr_rect(scr), m_x-gr.x, m_y-gr.y)) main->set_scr_mth(scr);
					scr = GameSettings::SCR_DEATH;
					if( in_rect(scr_rect(scr), m_x-gr.x, m_y-gr.y)) main->set_scr_mth(scr);
					GameSettings::EndMeth end;
					end = GameSettings::END_NONE;
					if( in_rect(end_rect(end), m_x-gr.x, m_y-gr.y)) main->set_end_mth(end);
					end = GameSettings::END_ROUND;
					if( in_rect(end_rect(end), m_x-gr.x, m_y-gr.y)) main->set_end_mth(end);
					end = GameSettings::END_SCORE;
					if( in_rect(end_rect(end), m_x-gr.x, m_y-gr.y)) main->set_end_mth(end);
					bool use;
					use = false;
					if( in_rect(use_rect(use), m_x-gr.x, m_y-gr.y)) main->set_use_teams(use);
					use = true;
					if( in_rect(use_rect(use), m_x-gr.x, m_y-gr.y)) main->set_use_teams(use);
				}
				for(int i = 0; i<UPG_NUM; i++) if(in_rect(upg_rect(i),m_x-gr.x, m_y-gr.y)) upg_prs_ind = i;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if(e.button.button == SDL_BUTTON_LEFT && in_rect(ar, m_x, m_y) && a_prs){
				a_prs = false;
				add_player();
			}
			if(e.button.button == SDL_BUTTON_LEFT){
				int mask = 1 << upg_prs_ind;
				if(upg_prs_ind >= 0 && in_rect(upg_rect(upg_prs_ind), m_x-gr.x, m_y-gr.y)) main->set_upgs(mask, !(upg_mask&mask));
			}
			upg_prs_ind = -1;
			break;
		case SDL_MOUSEWHEEL:
			if(in_rect(get_players_rect(), m_x, m_y))
				scr_t += e.wheel.y * SCR_S * (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? 1 : -1);
			break;
	}
	if(kfocus && kfocus->get_msg_upd()) main->update_name(get_ind(kfocus));
}
void SettingMenu::lose_mfocus(){
	upg_prs_ind = -1;
	SubMenu::lose_mfocus();
	if(mfocus) mfocus->lose_mfocus();
	mfocus = NULL;
}
void SettingMenu::lose_kfocus(){
	if(kfocus) kfocus->lose_kfocus();
	if(kfocus) main->update_name(get_ind(kfocus));
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
void SettingMenu::add_player(PlayerMData d){
	players.push_back(new PlayerSetting(rend, this, d));
	player_ts.push_back(SDL_CreateTexture(	rend, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET,
											PLST_W, PLST_H));
	SDL_SetTextureBlendMode(player_ts.back(), SDL_BLENDMODE_BLEND);
	main->add_player();
}
void SettingMenu::update_col(int i,int col){
	players[i]->update_col(col);
}
int SettingMenu::get_ind(int ind){
	return players[ind]->get_ind();
}

MainScr* SettingMenu::get_main(){
	return main;
}

void SettingMenu::set_settings(GameSettings s){
	set_game_lim(s.lim);
	set_tie_lim(s.allow_dif);
	set_scr_mth(s.scr_mth);
	set_end_mth(s.end_mth);
}
GameSettings SettingMenu::get_settings(){
	GameSettings s;
	s.lim = game_lim.get_num();
	s.allow_dif = tie_lim.get_num();
	s.scr_mth = scr_mth;
	s.end_mth = end_mth;
	return s;
}
void SettingMenu::set_game_lim(int lim){
	game_lim.set_num(lim);
}
void SettingMenu::set_tie_lim(int lim){
	tie_lim.set_num(lim);
}
void SettingMenu::set_team_num(int num){
	team_num.set_num(num);
}
void SettingMenu::set_use_teams(bool use){
	use_teams = use;
}
void SettingMenu::set_scr_mth(GameSettings::ScoreMeth mth){
	scr_mth = mth;
}
void SettingMenu::set_end_mth(GameSettings::EndMeth mth){
	end_mth = mth;
}
void SettingMenu::set_id(int ind, int id){
	players[ind]->set_id(id);
}
int SettingMenu::get_id(int ind){
	return players[ind]->get_id();
}
void SettingMenu::set_upg_mask(int mask){
	upg_mask = mask;
}
int SettingMenu::get_upg_mask(){
	return upg_mask;
}
MainData::MainData(MainScr* scr){
	clnt = scr->clnt;
	const char* sn = scr->conn.get_address();
	serv_name = new char[1+strlen(sn)];
	strcpy(serv_name, sn);
	
	for(int i = 0; i<scr->sett.players.size(); i++){
		PlayerSetting* pl = scr->sett.players[i];
		players.push_back({pl->ind, pl->col, pl->name});
	}
}
MainData::~MainData(){
	delete[] serv_name;
}

MainScr::MainScr(Main* up, MainData* data) : State(up), conn(up->get_renderer(),this, data ? data->serv_name : NULL), play(up->get_renderer(),this), sett(up->get_renderer(),this, data){
	clnt = NULL;
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
	
	if(clnt) iconn = true;

	conn_timer = -1;
	
	if(data){
		clnt = data->clnt;
		iconn = true;
		
		char msg[3] = "\x00\x00";
		
		clnt->send(msg,2,PROTO_REL); // request update
		
		send_players();
	}
	
	delete data;
	
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
			if(mfocus) mfocus->event(e);
			break;
		case SDL_MOUSEBUTTONDOWN:
			set_kfocus();
		case SDL_MOUSEBUTTONUP:
			m_correct(e.button.x, e.button.y);
		case SDL_MOUSEWHEEL:
			if(mfocus) mfocus->event(e);
			break;
		case SDL_KEYDOWN:
			switch(e.key.keysym.sym){
			case SDLK_ESCAPE:
				if(clnt) leave();
				return true;
			}
		case SDL_KEYUP:
		case SDL_TEXTINPUT:
			if(kfocus) kfocus->event(e);
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
			int i,ind,c,t;
			bool u;
			GameSettings s;
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
						cur = decode_gamesett(cur, s);
						sett.set_settings(s);
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
					case '\x06':
						ihost = true;
						break;
					case '\x07':
						cur = decode_int(cur, i);
						play.set_host(i);
						break;
					case '\x08':
						cur = decode_int(cur, i);
						cur = decode_int(cur, t);
						play.set_team(i,t);
						break;
					case '\x09':
						cur = decode_int(cur, i);
						play.set_our(i);
						cur = decode_int(cur, ind);
						sett.set_id(ind,i);
						break;
						
					case '\x10':
						cur = decode_scr_mth(cur, s.scr_mth);
						sett.set_scr_mth(s.scr_mth);
						break;
					case '\x11':
						cur = decode_end_mth(cur, s.end_mth);
						sett.set_end_mth(s.end_mth);
						break;
					case '\x12':
						cur = decode_int(cur, s.allow_dif);
						sett.set_tie_lim(s.allow_dif);
						break;
					case '\x13':
						cur = decode_int(cur, s.lim);
						sett.set_game_lim(s.lim);
						break;
					case '\x14':
						cur = decode_int(cur, t);
						sett.set_team_num(t);
						play.set_team_num(t);
						break;
					case '\x15':
						cur = decode_bool(cur, u);
						sett.set_use_teams(u);
						play.set_use_teams(u);
						break;
					case '\x16':
						cur = decode_int(cur, i);
						sett.set_upg_mask(i);
						break;
					}
					break;
				case '\x01':
					cur = decode_char(cur, hh);
					switch(hh){
					case '\x00':
						cur = decode_int(cur, i);
						conn.set_cnt(i);
						break;
					case '\x02':
						start();
						return false;
						break;
					}
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
		add_player(sett.get_col(i));
		update_name(i);
	}
}

void MainScr::add_player(int pref_col){
	if(!iconn) return;
	char data[100];
	char* end;
		
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x01');
	end = encode_int(end, pref_col);
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
void MainScr::change_team(int id, int team){
	if(!iconn) return;
	
	char data[100];
	char* end;
				
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x08');
	end = encode_int(end, id);
	end = encode_int(end, team);
	clnt->send(data, end-data, PROTO_REL);
}
void MainScr::set_game_lim(int lim){
	if(!ihost) return;
	
	char data[100];
	char* end;
				
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x13');
	end = encode_int(end, lim);
	clnt->send(data, end-data, PROTO_REL);
}
void MainScr::set_tie_lim(int lim){
	if(!ihost) return;
	
	char data[100];
	char* end;
				
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x12');
	end = encode_int(end, lim);
	clnt->send(data, end-data, PROTO_REL);
}
void MainScr::set_scr_mth(GameSettings::ScoreMeth mth){
	if(!ihost) return;
	
	char data[100];
	char* end;
				
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x10');
	end = encode_scr_mth(end, mth);
	clnt->send(data, end-data, PROTO_REL);
}
void MainScr::set_end_mth(GameSettings::EndMeth mth){
	if(!ihost) return;
	
	char data[100];
	char* end;
				
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x11');
	end = encode_end_mth(end, mth);
	clnt->send(data, end-data, PROTO_REL);
}
void MainScr::set_team_num(int num){
	if(!ihost) return;
	
	char data[100];
	char* end;
				
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x14');
	end = encode_int(end, num);
	clnt->send(data, end-data, PROTO_REL);	
}
void MainScr::set_use_teams(bool use){
	if(!ihost) return;
	
	char data[100];
	char* end;
				
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x15');
	end = encode_bool(end, use);
	clnt->send(data, end-data, PROTO_REL);	
}
void MainScr::set_upgs(int mask, bool val){
	if(!ihost) return;
	
	char data[100];
	char* end;
				
	end = data;
	end = encode_char(end, '\x00');
	end = encode_char(end, '\x16');
	end = encode_int(end, mask);
	end = encode_bool(end, val);
	clnt->send(data, end-data, PROTO_REL);	
}

void MainScr::start_count(){
	char data[100];
	char* end;
				
	end = data;
	end = encode_char(end, '\x01');
	end = encode_char(end, '\x00');
	clnt->send(data, end-data, PROTO_REL);	
}
void MainScr::stop_count(){
	char data[100];
	char* end;
				
	end = data;
	end = encode_char(end, '\x01');
	end = encode_char(end, '\x01');
	clnt->send(data, end-data, PROTO_REL);		
}

void MainScr::start(){
	GameConfig cf(play.get_player_num(), play.get_use_teams() ? play.get_team_num() : play.get_player_num(), sett.get_upg_mask());
	
	cf.set = sett.get_settings();
	
	std::map<int,int> inds;
	
	int i = 0;
	for(auto it = play.get_players(); i<cf.tank_num; it++,i++){
		cf.names[i] = it->second->get_name();
		cf.team_inds[i] = play.get_use_teams() ? it->second->get_team() : i;
		cf.colors[i] = it->second->get_col();
		cf.keys[i] = -1;
		inds[it->first] = i;
	}
	for(int i = 0; i<sett.get_player_num(); i++){
		cf.keys[inds[sett.get_id(i)]] = sett.get_ind(i);
	}
	
	ExInEvents* in = new NetEx(clnt, cf);
	GameExtrap* ex = new GameExtrap(cf, in);
	GameQ* q = new GameEQ(ex);
	upper->set_state(new GameGui(q, upper, cf, new MainData(this)));
}