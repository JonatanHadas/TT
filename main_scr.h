#ifndef _MAIN_SCR_H
#define _MAIN_SCR_H

#include "gui.h"
#include "SDL2/SDL.h"

class SubMenu{
protected:
	double p;
	double pt;
	SDL_Renderer* rend;
	
	void draw_back(bool active);
public:
	SubMenu(SDL_Renderer* r);
	virtual void draw(bool conn) = 0;
	virtual void event(SDL_Event& e);
	virtual void lose_mfocus();
	virtual void lose_kfocus() = 0;
};

class ConnectionMenu : public SubMenu{
public:
	ConnectionMenu(SDL_Renderer* r);
	~ConnectionMenu();
	
	void draw(bool conn);
	void event(SDL_Event& e);
	void lose_mfocus();
	void lose_kfocus();
};
class PlayerMenu : public SubMenu{
public:
	PlayerMenu(SDL_Renderer* r);
	~PlayerMenu();
	
	void draw(bool conn);
	void event(SDL_Event& e);
	void lose_mfocus();
	void lose_kfocus();
};
class SettingMenu : public SubMenu{
public:
	SettingMenu(SDL_Renderer* r);
	~SettingMenu();
	
	void draw(bool conn);	
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
public:
	MainScr(Main* up);
	~MainScr();
	
	bool step();
};

#endif