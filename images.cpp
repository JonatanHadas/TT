#include "images.h"
#include "SDL2/SDL_image.h"

#include <vector>

#include "math.h"

#define DIR "images/"
#define TNK "tank/"
#define TEX "textures/"
#define UPG "upgrades/"
#define PNG ".png"

#include <stdio.h>

TankImg::TankImg(){
	body = cannon = gutling[0] = gutling[1] = gutling[2] = laser = ray_gun0 = ray_gun1 = ray_gun2 = ray_gun3 = launcher = missile = thick_cannon = mine_off = mine_on = shards = deathray = broadcast = image = NULL;
}
TankImg::~TankImg(){
	if(body) SDL_DestroyTexture(body);
	if(cannon) SDL_DestroyTexture(cannon);
	if(gutling[0]) SDL_DestroyTexture(gutling[0]);
	if(gutling[1]) SDL_DestroyTexture(gutling[1]);
	if(gutling[2]) SDL_DestroyTexture(gutling[2]);
	if(laser) SDL_DestroyTexture(laser);
	if(ray_gun0) SDL_DestroyTexture(ray_gun0);
	if(ray_gun1) SDL_DestroyTexture(ray_gun1);
	if(ray_gun2) SDL_DestroyTexture(ray_gun2);
	if(ray_gun3) SDL_DestroyTexture(ray_gun3);
	if(launcher) SDL_DestroyTexture(launcher);
	if(missile) SDL_DestroyTexture(missile);
	if(thick_cannon) SDL_DestroyTexture(thick_cannon);
	if(mine_off) SDL_DestroyTexture(mine_off);
	if(mine_on) SDL_DestroyTexture(mine_on);
	if(shards) SDL_DestroyTexture(shards);
	if(deathray) SDL_DestroyTexture(deathray);
	if(broadcast) SDL_DestroyTexture(broadcast);
	if(image) SDL_DestroyTexture(image);
}
bool TankImg::check(){
	return body && cannon && gutling[0] && gutling[1] && gutling[2] && laser && ray_gun0 && ray_gun1 && ray_gun2 && ray_gun3 && launcher && missile && thick_cannon && mine_off && mine_on && shards && deathray && broadcast && image;
}

// gutling_sym, laser_sym, death_ray_sym, wifi_sym, missile_sym, bomb_sym, mine_sym, fragment
std::vector<SDL_Texture*> texts;

// body, cannon, gutling0..2, laser, ray_gun 0..3, launcher, missile, thick_cannon, mine_on, mine_off, shards, deathray, broadcast, image
std::vector<SDL_Surface*> surfs;

struct TankTex{
	SDL_Color col;
	SDL_Surface* surf;
	TankTex(SDL_Color c){
		col = c;
		surf = NULL;
	}
	~TankTex(){
		if(surf) SDL_FreeSurface(surf);
	}
	SDL_Color get_col(int x, int y, SDL_Color temp, bool use_tex){
		SDL_Color cur,res;
		if(use_tex && surf){
			int w = surf->w, h = surf->h, pt = surf->pitch / 4;
			unsigned int* px = (unsigned int*)surf->pixels;

			SDL_GetRGBA(px[(x%w) + (y%h)*pt],surf->format, &cur.r, &cur.g, &cur.b, &cur.a);
		}
		else cur = col;
		
		int o = temp.r, r = temp.g;
		
		res.r = (255 * r + cur.r * (o-r))/255;
		res.g = (255 * r + cur.g * (o-r))/255;
		res.b = (255 * r + cur.b * (o-r))/255;
		
		res.a = temp.a;
		
		return res;
	}
};

std::vector<TankTex*> tts;

void free_images(){
	while(texts.size()>0){
		SDL_DestroyTexture(texts.back());
		texts.pop_back();
	}
	while(surfs.size()>0){
		SDL_FreeSurface(surfs.back());
		surfs.pop_back();
	}
	while(tts.size()>0){
		delete tts.back();
		tts.pop_back();
	}
}

bool get_surf(const char* name){
	SDL_Surface* surf = IMG_Load(name);
	
	if(surf == NULL) return false;
	
	SDL_LockSurface(surf);
	surfs.push_back(surf);
	return true;
}

bool get_tex(const char* name, SDL_Renderer* r){
	SDL_Surface* surf = IMG_Load(name);
	
	if(surf == NULL) return false;
	
	SDL_Texture* tex = SDL_CreateTextureFromSurface(r,surf);
	SDL_FreeSurface(surf);
	
	if(tex == NULL) return false;
	texts.push_back(tex);
	return true;
}

bool load_tank_tx(const char* name){
	if(get_surf(name)){
		unsigned int r,g,b;
		unsigned char cr,cg,cb;
		
		unsigned int* pixels = (unsigned int*)(surfs.back()->pixels);
		SDL_PixelFormat* fmt = surfs.back()->format;
		int w = surfs.back()->w, h = surfs.back()->h, p = surfs.back()->pitch / 4; 
		
		for(int i = 0; i < w; i++){
			for(int j = 0; j < h; j++){
				SDL_GetRGB(pixels[i + j*p], fmt, &cr, &cg, &cb);
				r+=cr;g+=cg;b+=cb;
			}
		}
		
		SDL_Color col = {r/(w*h),g/(w*h),b/(w*h),255};
		
		tts.push_back(new TankTex(col));
		tts.back()->surf = surfs.back();
		surfs.pop_back();
		
		return true;
	}
	return false;
}

void uni_cols(){
	tts.push_back(new TankTex({255,0,0,255}));
	tts.push_back(new TankTex({0,255,0,255}));
	tts.push_back(new TankTex({0,0,255,255}));
	tts.push_back(new TankTex({255,255,0,255}));
	tts.push_back(new TankTex({0,255,255,255}));
	tts.push_back(new TankTex({255,0,255,255}));
	tts.push_back(new TankTex({128,128,128,255}));
	tts.push_back(new TankTex({255,128,0,255}));
	tts.push_back(new TankTex({128,255,0,255}));
	tts.push_back(new TankTex({128,0,255,255}));
	tts.push_back(new TankTex({0,128,0,255}));
	tts.push_back(new TankTex({0,0,0,255}));
};

bool load_images(SDL_Renderer* rend){
	atexit(free_images);
	
	uni_cols();
	
	if(get_surf(DIR TNK "body" PNG))
	if(get_surf(DIR TNK "cannon" PNG))
	if(get_surf(DIR TNK "gutling0" PNG))
	if(get_surf(DIR TNK "gutling1" PNG))
	if(get_surf(DIR TNK "gutling2" PNG))
	if(get_surf(DIR TNK "lasergun" PNG))
	if(get_surf(DIR TNK "raygun0" PNG))
	if(get_surf(DIR TNK "raygun1" PNG))
	if(get_surf(DIR TNK "raygun2" PNG))
	if(get_surf(DIR TNK "raygun3" PNG))
	if(get_surf(DIR TNK "launcher" PNG))
	if(get_surf(DIR TNK "missile" PNG))
	if(get_surf(DIR TNK "thick_cannon" PNG))
	if(get_surf(DIR TNK "mine_on" PNG))
	if(get_surf(DIR TNK "mine_off" PNG))
	if(get_surf(DIR TNK "shards" PNG))
	if(get_surf(DIR TNK "deathray" PNG))
	if(get_surf(DIR TNK "broadcast" PNG))
	if(get_surf(DIR TNK "image" PNG))
	if(get_tex(DIR UPG "gutling" PNG, rend))
	if(get_tex(DIR UPG "laser" PNG, rend))
	if(get_tex(DIR UPG "deathray" PNG, rend))
	if(get_tex(DIR UPG "wifi" PNG, rend))
	if(get_tex(DIR UPG "missile" PNG, rend))
	if(get_tex(DIR UPG "bomb" PNG, rend))
	if(get_tex(DIR UPG "mine" PNG, rend))
	if(get_tex(DIR "fragment" PNG, rend))
	if(load_tank_tx(DIR TNK TEX "army" PNG))
		return true;
	return false;
}

int get_ind(Img i){
	switch(i){
	case IMG_GUTLING_SYM:
		return 0;
	case IMG_LASER_SYM:
		return 1;
	case IMG_DEATH_RAY_SYM:
		return 2;
	case IMG_WIFI_SYM:
		return 3;
	case IMG_MISSILE_SYM:
		return 4;
	case IMG_BOMB_SYM:
		return 5;
	case IMG_MINE_SYM:
		return 6;
	case IMG_FRAGMENT:
		return 7;
	}
	return -1;
}

SDL_Color get_tank_col(int ind){
	return tts[ind]->col;
}

SDL_Texture* tex_img(int ind, SDL_Surface* src, SDL_Renderer* rend, bool use_tex){
	TankTex* tx = tts[ind];
	int w = src->w, h = src->h;
	SDL_Surface* dst = SDL_CreateRGBSurface(0, w, h, 32, 
													0xff000000,
													0x00ff0000,
													0x0000ff00,
													0x000000ff);
	if(dst == NULL) return NULL;

	SDL_LockSurface(dst);
	
	int pts = src->pitch / 4;
	int ptd = dst->pitch / 4;
	unsigned int* pxs = (unsigned int*)src->pixels;
	unsigned int* pxd = (unsigned int*)dst->pixels;

	for(int x = 0; x<w; x++){
		for(int y = 0; y<h; y++){
			SDL_Color cur;
			SDL_GetRGBA(pxs[x + pts*y], src->format, &cur.r, &cur.g, &cur.b, &cur.a);
			cur = tx->get_col(x,y,cur,use_tex);
			pxd[x + ptd*y] = SDL_MapRGBA(dst->format, cur.r, cur.g, cur.b, cur.a);
		}
	}

	SDL_UnlockSurface(dst);
	SDL_Texture* ret = SDL_CreateTextureFromSurface(rend, dst);
	SDL_FreeSurface(dst);
	return ret;
}

void generate_tank(int ind, SDL_Renderer* rend, TankImg* img){
	img->col = tts[ind]->col;
	img->body = tex_img(ind, surfs[0], rend, true);
	img->cannon = tex_img(ind, surfs[1], rend, true);
	img->gutling[0] = tex_img(ind, surfs[2], rend, true);
	img->gutling[1] = tex_img(ind, surfs[3], rend, true);
	img->gutling[2] = tex_img(ind, surfs[4], rend, true);
	img->laser = tex_img(ind, surfs[5], rend, true);
	img->ray_gun0 = tex_img(ind, surfs[6], rend, true);
	img->ray_gun1 = tex_img(ind, surfs[7], rend, true);
	img->ray_gun2 = tex_img(ind, surfs[8], rend, true);
	img->ray_gun3 = tex_img(ind, surfs[9], rend, true);
	img->launcher = tex_img(ind, surfs[10], rend, true);
	img->missile = tex_img(ind, surfs[11], rend, true);
	img->thick_cannon = tex_img(ind, surfs[12], rend, true);
	img->mine_on = tex_img(ind, surfs[13], rend, true);
	img->mine_off = tex_img(ind, surfs[14], rend, true);
	img->shards = tex_img(ind, surfs[15], rend, true);
	img->deathray = tex_img(ind, surfs[16], rend, true);
	img->broadcast = tex_img(ind, surfs[17], rend, true);
	img->image = tex_img(ind, surfs[18], rend, true);
}


SDL_Texture* get_img(Img i){
	return texts[get_ind(i)];
}