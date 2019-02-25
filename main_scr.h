#ifndef _MAIN_SCR_H
#define _MAIN_SCR_H

#include "gui.h"
#include "texts.h"
#include "keys.h"
#include "images.h"
#include "game_config.h"
#include <SDL2/SDL.h>
#include "network.h"
#include <string>
#include <vector>
#include <set>

class MainScr;

class SettingMenu;
class PlayerMenu;
class ConnectionMenu;

class NumberField{
	SDL_Renderer* rend;
	SDL_Rect r;
	int num;
	int lower;
	int upper;
	bool use_l, use_u;
	
	bool d_prs, u_prs;
	
	bool active, upd;
	
	SDL_Rect u_rect();
	SDL_Rect b_rect();
	
	Msg* up; Msg* down;
	Msg* iup; Msg* idown;
	Msg* msg;
	void cut();
	void upd_msg();
public:
	NumberField(SDL_Renderer* ren, SDL_Rect rect, int v);
	NumberField(SDL_Renderer* ren, SDL_Rect r, int v, int lowerb, int upperb);
	NumberField(SDL_Renderer* ren, SDL_Rect r, int v, int bound, bool islower);
	~NumberField();
	void draw();
	void event(SDL_Event& e);

	int get_num();
	void set_num(int n);
	void set_active(bool a);
	bool get_upd();
	SDL_Rect get_rect();
};


class SubMenu{
protected:
	double p;
	double pt;
	SDL_Renderer* rend;
	MainScr* main;
	
	
	int m_x, m_y;
	
	void draw_back(bool active);
public:
	SubMenu(SDL_Renderer* r, MainScr* main);
	virtual void draw() = 0;
	virtual void event(SDL_Event& e);
	virtual void lose_mfocus();
	virtual void lose_kfocus() = 0;
};

class ConnectionMenu : public SubMenu{
	bool focus_addr;
	SDL_Rect r_addr();
	std::string addr;
	Msg* addr_m;
	Msg* addr_fil;
	void reset_msg();
	
	Msg* wait;
	Msg* start;
	Msg* leave;
	Msg* nums[4];
	
	Msg* err_msg;
	double err_p;
	int num;
	
	double but_p;
	
	bool st_prs, lv_prs;
	
	unsigned int time;
	double load_p;
	Msg* loading[4];
	
	SDL_Rect r_start();
	SDL_Rect r_leave();
public:
	ConnectionMenu(SDL_Renderer* r, MainScr* main);
	~ConnectionMenu();
		
	void draw();
	void event(SDL_Event& e);
	void lose_mfocus();
	void lose_kfocus();
	
	void set_err(const char* msg);
	
	const char* get_address();
};

class CPlayerData{
	Msg* name_m;
	TankImg* img;
	int col;
	int team;
	std::string name;
	SDL_Renderer* rend;
	
	bool host;
	bool our;
	Msg* hst;
	Msg* up;
	Msg* dn;
public:
	CPlayerData(int c, int t, SDL_Renderer* r);
	~CPlayerData();
	void set_col(int i);
	int get_col();
	void set_team(int t);
	int get_team();
	void set_name(const char* name);
	std::string get_name();
	
	void draw(int y, bool use_teams);
	
	void set_host();
	void set_our();
	bool get_our();
};

class PlayerMenu : public SubMenu{
	std::map<int, CPlayerData*> players;
	std::map<int, double> ys;
	std::vector<double> tys;
	bool use_teams;
public:
	PlayerMenu(SDL_Renderer* r, MainScr* main);
	~PlayerMenu();
	
	void draw();
	void event(SDL_Event& e);
	void lose_mfocus();
	void lose_kfocus();
	
	void add_player(int id, int c, int t);
	void remove_player(int id);
	void update_name(int id, const char* name);
	void update_col(int id, int c);
	void set_host(int id);
	void set_our(int id);
	
	void set_team_num(int num);
	void set_use_teams(bool use);
	void set_team(int id, int t);
};

class PlayerSetting{
	enum Focus{
		FOCUS_NONE,
		FOCUS_FD, FOCUS_BK,
		FOCUS_LT, FOCUS_RT,
		FOCUS_SHT,
		FOCUS_NAME,
		FOCUS_COL,
	};
	int ind;
	SDL_Renderer* rend;
	
	SDL_Texture* cross[2];
	bool x_prs;
	
	bool msg_upd;
	
	int m_x, m_y;
	
	SDL_Rect get_xr();
	
	KeySet p;
	Msg* k_fd;
	Msg* k_bk;
	Msg* k_lt;
	Msg* k_rt;
	Msg* k_sht;
	
	std::string name;
	Msg* name_m;
	
	Focus focus;
	void update_keys();
	void update_msg();
	SDL_Rect get_rect(Focus f);
	
	int col;
	std::vector<SDL_Texture*> cols;
	
	SettingMenu* up;
public:
	PlayerSetting(SDL_Renderer* rend, int i, SettingMenu* up);
	~PlayerSetting();
	
	bool event(SDL_Event& e); //true if destroyed
	void draw();
	
	void lose_mfocus();
	void lose_kfocus();
	
	KeySet get_keys();
	const char* get_name();
	
	bool get_msg_upd();
	void update_col(int col);
	
	int get_ind();
};

class SettingMenu : public SubMenu{
	std::vector<PlayerSetting*> players;
	std::set<int> non_used_inds;
	std::vector<SDL_Texture*> player_ts;
	SDL_Texture* players_t;
	SDL_Texture* playera_t[2];
	
	SDL_Texture* game_t;
	double y, scr_y, scr_t;
	
	bool a_prs;
	
	PlayerSetting* mfocus;
	int mfi;
	PlayerSetting* kfocus;
	void set_mfocus();
	void set_kfocus();
	
	SDL_Rect get_players_rect();
	SDL_Rect get_game_rect();
	
	NumberField tie_lim, game_lim, team_num;
	bool use_teams;
	SDL_Rect use_rect(bool use);
	double ux,uw;
	Msg* teams;
	Msg* nteams;
	GameSettings::ScoreMeth scr_mth;
	Msg* tie_msg;
	SDL_Rect scr_rect(GameSettings::ScoreMeth scr);
	double sx,sw;
	Msg* scr_m;
	Msg* scr_lst;
	Msg* scr_dth;
	Msg* scr_ord;
	GameSettings::EndMeth end_mth;
	SDL_Rect end_rect(GameSettings::EndMeth mth);
	double ex,ew;
	Msg* end_m;
	Msg* end_non;
	Msg* end_rnd;
	Msg* end_scr;
	
public:
	SettingMenu(SDL_Renderer* r, MainScr* main);
	~SettingMenu();
	
	void draw();	
	void event(SDL_Event& e);
	void lose_mfocus();
	void lose_kfocus();
	
	void add_player();
	
	int get_player_num();
	const char* get_name(int i);
	
	void update_col(int i, int col);

	int get_ind(PlayerSetting* pl);
	MainScr* get_main();
	
	std::vector<PlayerSetting*>& get_players();
	
	GameSettings get_settings();
	void set_settings(GameSettings s);
	
	void set_game_lim(int lim);
	void set_tie_lim(int lim);
	void set_team_num(int num);
	void set_use_teams(bool use);
	void set_scr_mth(GameSettings::ScoreMeth mth);
	void set_end_mth(GameSettings::EndMeth mth);
	
};

class MainScr : public State{
	SDL_Texture* conn_t;
	SDL_Texture* play_t;
	SDL_Texture* sett_t;
	
	ConnectionMenu conn;
	PlayerMenu play;
	SettingMenu sett;
	
	SubMenu* mfocus;
	SubMenu* kfocus;
	
	int m_x, m_y;
		
	void set_mfocus();
	void set_kfocus();
	
	Client* clnt;
	
	bool ihost;
	bool iconn;
	
	int conn_timer;
		
	void m_correct(int& x, int& y);
	
	void send_players();
public:
	MainScr(Main* up, Client* c);
	~MainScr();
	
	void connect();
	void leave();
	
	bool get_conn();
	bool get_conn_try();
	bool get_host();

	bool step();
	
	
	void add_player();
	void remove_player(int ind);
	void update_name(int i);
	void update_col(int i, int col);
	void change_team(int id, int team);
	void set_game_lim(int lim);
	void set_tie_lim(int lim);
	void set_scr_mth(GameSettings::ScoreMeth mth);
	void set_end_mth(GameSettings::EndMeth mth);
	
	void set_team_num(int num);
	void set_use_teams(bool use);
};

#endif