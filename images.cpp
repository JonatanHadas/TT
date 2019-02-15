#include "images.h"
#include "SDL2/SDL_image.h"

#include <vector>

#include "math.h"

#define DIR "images/"
#define TNK "tank/"
#define UPG "upgrades/"
#define PNG ".png"


// gutling_sym, laser_sym, death_ray_sym, wifi_sym, missile_sym, bomb_sym, mine_sym, fragment
std::vector<SDL_Texture*> texts;

// body, cannon, gutling0..2, laser, ray_gun 0..3, launcher, missile, thick_cannon, mine_on, mine_off, shards, deathray, broadcast
std::vector<SDL_Surface*> surfs;
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

bool get_surf(char* name){
	SDL_Surface* surf = IMG_LOAD(name);
	
	if(surf == NULL) return false;
	
	SDL_LockSurface(surf);
	surfs.push_back(surf);
	return true;
}

bool get_tex(char* name, SDL_Renderer* r){
	SDL_Surface* surf = IMG_LOAD(name);
	
	if(surf == NULL) return false;
	
	SDL_Texture* tex = SDL_CreateTextureFromSurface(r,surf);
	SDL_FreeSurface(surf);
	
	if(tex == NULL) return false;
	texts.push_back(tex);
}
bool load_images(SDL_Renderer* rend){
	atexit(free_images);
	
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
	if(get_tex(DIR UPG "gutling" PNG))
	if(get_tex(DIR UPG "laser" PNG))
	if(get_tex(DIR UPG "deathray" PNG))
	if(get_tex(DIR UPG "wifi" PNG))
	if(get_tex(DIR UPG "missile" PNG))
	if(get_tex(DIR UPG "bomb" PNG))
	if(get_tex(DIR UPG "mine" PNG))
	if(get_tex(DIR "fragment" PNG))
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

SDL_Texture* get_img(Img i){
	return texts[get_ind(i)];
}