#include "texts.h"
#include <SDL2/SDL_ttf.h>
#include <vector>

#define FONT_NUM 4
int sizes[FONT_NUM] = {20,72,30,14};

std::vector<TTF_Font*> fonts;

void clear_fonts(){
	while(fonts.size()>0){
		TTF_CloseFont(fonts.back());
		fonts.pop_back();
	}
}

bool load_fonts(){
	atexit(clear_fonts);
	for(int i = 0; i<FONT_NUM;i++){
		fonts.push_back(TTF_OpenFont("fonts/arial.ttf", sizes[i]));
		if(fonts.back() == NULL){
			fonts.pop_back();
			return false;
		}
	}
/*	for(; txt_num < NUM_TEXTS; txt_num++){
		def_texts[txt_num] = new Msg(txts[txt_num], cls[txt_num], szs[txt_num], renderer);
		if(!def_texts[txt_num]->exists()) return false;
	}*/
	return true;
}


//#include <stdio.h>

int get_font_ind(FontType f){
	switch(f){
	case FONT_NRM:
		return 0;
	case FONT_BIG:
		return 1;
	case FONT_MID:
		return 2;
	case FONT_SML:
		return 3;
	}
	return -1;
}

Msg::Msg(const char* text, SDL_Color color, FontType t, SDL_Renderer* renderer){
	rend = renderer;
	SDL_Surface* surf = TTF_RenderUTF8_Blended( fonts[get_font_ind(t)], (text && (*text))?text:" ", color);
	if(surf == NULL){img = NULL; w = -1; h = -1;}
	else{
		img = SDL_CreateTextureFromSurface(rend, surf);
		if(img == NULL){w = -1; h = -1;}
		else{ w = surf->w; h = surf->h;}
		SDL_SetTextureAlphaMod(img, color.a);
		SDL_FreeSurface(surf);
	}
}

void Msg::render_centered(int x, int y, Align a){
	SDL_Rect dst;
	switch(a){
	case AL_RIGHT:
		dst.x = x-w;
		break;
	case AL_LEFT:
		dst.x = x;
		break;
	case AL_CENTER:
		dst.x = x-w/2;
		break;
	}
	dst.y = y - h/2;
	dst.w = w;dst.h=h;
	SDL_RenderCopy(rend,img,NULL, &dst);
}

bool Msg::exists(){
	return img != NULL;
}