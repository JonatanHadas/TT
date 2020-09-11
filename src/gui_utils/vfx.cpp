#include "vfx.h"


EffectManager::EffectManager(SDL_Renderer* render){
	rend = render;
}
EffectManager::~EffectManager(){
	clear();
}

void EffectManager::step(){
	for(auto it = effs.begin(); it != effs.end(); it++) (*it)->step();
	for(auto it = fd.begin(); it != fd.end(); it++){
		delete (*it);
		effs.erase(*it);
	}
	fd.clear();
}
SDL_Renderer* EffectManager::get_renderer(){
	return rend;
}

void EffectManager::clear(){
	for(auto it = effs.begin(); it != effs.end(); it++) delete (*it);
	effs.clear();
	fd.clear();
}
void EffectManager::add_effect(Effect* e){
	e->man = this;
	effs.insert(e);
}
void EffectManager::remove_effect(Effect* e){
	fd.insert(e);
}

	
EffectManager* Effect::get_manager(){
	return man;
}

Effect::Effect(){
	man = NULL;
}


FadeOut::FadeOut(	SDL_Texture* texture, SDL_Rect srcr,
					double xx, double yy,
					double vvx, double vvy,
					int ww, int hh, 
					double angle,
					SDL_RendererFlip f,
					int time, double alpha) : FadeOut(texture, xx,yy,vvx,vvy,ww,hh,angle,f,time,alpha){
	usrc = true;
	
	src = srcr;
}
FadeOut::FadeOut(	SDL_Texture* texture,
					double xx, double yy,
					double vvx, double vvy,
					int ww, int hh, 
					double angle,
					SDL_RendererFlip f,
					int time, double alpha){
	usrc = false;		
					
	tex = texture;
	x=xx;y=yy;vx=vvx;vy=vvy;ang=angle;flip=f;w=ww;h=hh;
	
	
	r=g=b=255;
	damp = 0.0;
	a = alpha * 255.0;
	da = a / time;
}

void FadeOut::set_damp(double d){
	damp = d;
}
void FadeOut::set_color(unsigned char rr, unsigned char gg, unsigned char bb){
	r=rr;g=gg;b=bb;
}
	
void FadeOut::step(){
	SDL_Rect dst;
	dst.w = w; dst.h = h;
	dst.x = (int)(x-w/2);
	dst.y = (int)(y-h/2);
	
	unsigned char alpha, red, green, blue;
	SDL_GetTextureAlphaMod(tex, &alpha);
	SDL_GetTextureColorMod(tex, &red, &green, &blue);
	SDL_SetTextureAlphaMod(tex, (unsigned char)a);
	SDL_SetTextureColorMod(tex, r,g,b);
	SDL_RenderCopyEx(get_manager()->get_renderer(),  tex, usrc ? &src : NULL, &dst, ang, NULL, flip);
	SDL_SetTextureAlphaMod(tex, alpha);
	SDL_SetTextureColorMod(tex, red, green, blue);
	
	x += vx; y += vy;
	a -= da;
	
	vx *= (1-damp);
	vy *= (1-damp);
		
	if(a<=0) get_manager()->remove_effect(this);
}

FadeSize::FadeSize(SDL_Texture* texture, SDL_Rect srcr,
					double x, double y,
					double w0, double h0,
					double wf, double hf,
					double ang,
					SDL_RendererFlip flip,
					int time, double alpha) : FadeSize(texture, x,y,w0,h0,wf,hf,ang,flip,time,alpha){
						
	src = srcr;
	
	usrc = true;
}

FadeSize::FadeSize(SDL_Texture* texture,
					double xx, double yy,
					double w0, double h0,
					double wf, double hf,
					double angle,
					SDL_RendererFlip f,
					int time, double alpha){
	
	usrc = false;		
					
	tex = texture;
	x=xx;y=yy;ang=angle;flip=f;
	w=w0;h=h0;
	dw = (wf-w0)/time;
	dh = (hf-h0)/time;
	
	
	r=g=b=255;
	a = alpha * 255.0;
	da = a / time;
}
		
void FadeSize::set_color(unsigned char rr, unsigned char gg, unsigned char bb){
	r=rr;g=gg;b=bb;
}

void FadeSize::step(){
	SDL_Rect dst;
	dst.w = (int)w; dst.h = (int)h;
	dst.x = (int)(x-w/2);
	dst.y = (int)(y-h/2);
	
	unsigned char alpha, red, green, blue;
	SDL_GetTextureAlphaMod(tex, &alpha);
	SDL_GetTextureColorMod(tex, &red, &green, &blue);
	SDL_SetTextureAlphaMod(tex, (unsigned char)a);
	SDL_SetTextureColorMod(tex, r,g,b);
	SDL_RenderCopyEx(get_manager()->get_renderer(),  tex, usrc ? &src : NULL, &dst, ang, NULL, flip);
	SDL_SetTextureAlphaMod(tex, alpha);
	SDL_SetTextureColorMod(tex, red, green, blue);
	
	w += dw; h += dh;
	a -= da;
			
	if(a<=0) get_manager()->remove_effect(this);
}
