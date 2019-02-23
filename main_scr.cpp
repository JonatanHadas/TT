#include "main_scr.h"

#define SCR_W 1280
#define SCR_H 960

#define SETT_W 560
#define SERV_H 200

#define TRANS_CHS 7
#define TRANS_SPD 0.1

#include <math.h>
#include "utf8.h"

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
	num = -1;
	but_p = -1;
	st_prs = lv_prs = false;
}
ConnectionMenu::~ConnectionMenu(){
	if(addr_m) delete addr_m;
	delete addr_fil;
	delete nums[0];
	delete nums[1];
	delete nums[2];
	delete nums[3];
	delete start; delete leave; delete wait;
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
		focus_addr = in_rect(r_addr(), e.button.x, e.button.y);
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
	
	follow(but_p, main->get_conn() ? 0 : -1 ,0.3);
	
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
}
void ConnectionMenu::lose_mfocus(){
	SubMenu::lose_mfocus();
}
void ConnectionMenu::lose_kfocus(){
	focus_addr = false;
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
}
void PlayerMenu::lose_mfocus(){
	SubMenu::lose_mfocus();
}
void PlayerMenu::lose_kfocus(){
	
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
	
PlayerSetting::PlayerSetting(SDL_Renderer* rn, int i){
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
}
void PlayerSetting::update_msg(){
	if(name_m) delete name_m;
	name_m = new Msg(name.c_str(), {0,0,0,255}, FONT_NRM, rend);
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
			focus = FOCUS_NONE;
			if(in_rect(get_xr(), m_x, m_y)) x_prs = true;
			if(in_rect(get_rect(FOCUS_BK), m_x, m_y)) focus = FOCUS_BK;
			if(in_rect(get_rect(FOCUS_FD), m_x, m_y)) focus = FOCUS_FD;
			if(in_rect(get_rect(FOCUS_RT), m_x, m_y)) focus = FOCUS_RT;
			if(in_rect(get_rect(FOCUS_LT), m_x, m_y)) focus = FOCUS_LT;
			if(in_rect(get_rect(FOCUS_SHT), m_x, m_y)) focus = FOCUS_SHT;
			if(in_rect(get_rect(FOCUS_NAME), m_x, m_y)) focus = FOCUS_NAME;
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

}
	
void PlayerSetting::lose_mfocus(){
	m_x = m_y = -1;
}
void PlayerSetting::lose_kfocus(){
	focus = FOCUS_NONE;
}
	
KeySet PlayerSetting::get_keys(){
	return get_keyset(ind);
}
	
int PlayerSetting::get_ind(){
	return ind;
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

void SettingMenu::add_player(){
	players.push_back(new PlayerSetting(rend,
										(non_used_inds.size() > 0) ? *(non_used_inds.begin()) : players.size()));
	player_ts.push_back(SDL_CreateTexture(	rend, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET,
											PLST_W, PLST_H));
	SDL_SetTextureBlendMode(player_ts.back(), SDL_BLENDMODE_BLEND);
	if(non_used_inds.size()>0) non_used_inds.erase(non_used_inds.begin());
}

MainScr::MainScr(Main* up) : State(up), conn(up->get_renderer(),this), play(up->get_renderer(),this), sett(up->get_renderer(),this){
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
bool MainScr::get_conn(){
	return false;
}
bool MainScr::get_host(){
	return false;
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