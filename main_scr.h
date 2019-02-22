#ifndef _MAIN_SCR_H
#define _MAIN_SCR_H

#include "gui.h"
#include "texts.h"
#include "SDL2/SDL.h"
#include <string>

class SubMenu{
protected:
	double p;
	double pt;
	SDL_Renderer* rend;
	
	bool conn;
	
	int m_x, m_y;
	
	void draw_back(bool active);
public:
	SubMenu(SDL_Renderer* r);
	void set_conn(bool c);
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
	void reset_msg();
	
	Msg* wait;
	Msg* start;
	Msg* leave;
	Msg* nums[4];
	int num;
	
	double but_p;
	
	bool st_prs, lv_prs;
	
	bool host;
	SDL_Rect r_start();
	SDL_Rect r_leave();
public:
	ConnectionMenu(SDL_Renderer* r);
	~ConnectionMenu();
	
	void set_host(bool h);
	
	void draw();
	void event(SDL_Event& e);
	void lose_mfocus();
	void lose_kfocus();
};
class PlayerMenu : public SubMenu{
public:
	PlayerMenu(SDL_Renderer* r);
	~PlayerMenu();
	
	void draw();
	void event(SDL_Event& e);
	void lose_mfocus();
	void lose_kfocus();
};
class SettingMenu : public SubMenu{
public:
	SettingMenu(SDL_Renderer* r);
	~SettingMenu();
	
	void draw();	
	void event(SDL_Event& e);
	void lose_mfocus();
	void lose_kfocus();
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
	void set_conn(bool c);
	
	void m_correct(int& x, int& y);
public:
	MainScr(Main* up);
	~MainScr();
	
	bool step();
};

#endif