#ifndef _VFX_H
#define _VFX_H

#include "SDL2/SDL.h"
#include <set>

class Effect;
class EffectManager;

class EffectManager{
	std::set<Effect*> effs;
	std::set<Effect*> fd;
	
	SDL_Renderer* rend;
	
public:
	EffectManager(SDL_Renderer* rend);
	~EffectManager();
	
	void step();
	SDL_Renderer* get_renderer();
	
	void clear();
	void add_effect(Effect* e);
	void remove_effect(Effect* e);
};

class Effect{
	EffectManager* man;
protected:
	virtual void step() = 0;
	EffectManager* get_manager();
	friend EffectManager;
public:
	Effect();
};

class FadeOut : public Effect{
	SDL_Texture* tex;
	SDL_Rect src;
	
	double x,y, vx,vy;
	int w,h;
	double ang;
	
	bool usrc;
	
	SDL_RendererFlip flip;
	
	unsigned char r,g,b;
	
	double a,da;
	double damp;
public:
	FadeOut(SDL_Texture* texture, SDL_Rect src,
			double x, double y,
			double vx, double vy,
			int w, int h,
			double ang,
			SDL_RendererFlip flip,
			int time, double alpha = 1.0);
	FadeOut(SDL_Texture* texture,
			double x, double y,
			double vx, double vy,
			int w, int h,
			double ang,
			SDL_RendererFlip flip,
			int time, double alpha = 1.0);
	
	void set_damp(double d);
	void set_color(unsigned char r, unsigned char g, unsigned char b);
	
	void step();
};

class FadeSize : public Effect{
	SDL_Texture* tex;
	SDL_Rect src;
	
	double x,y;
	double w,h, dw,dh;
	double ang;
	
	bool usrc;
	
	SDL_RendererFlip flip;
	
	unsigned char r,g,b;
		
	double a, da;
public:
	FadeSize(SDL_Texture* texture, SDL_Rect src,
			double x, double y,
			double w0, double h0,
			double wf, double hf,
			double ang,
			SDL_RendererFlip flip,
			int time, double alpha = 1.0);
	FadeSize(SDL_Texture* texture,
			double x, double y,
			double w0, double h0,
			double wf, double hf,
			double ang,
			SDL_RendererFlip flip,
			int time, double alpha = 1.0);
			
	void set_color(unsigned char r, unsigned char g, unsigned char b);
	
	void step();
};

#endif