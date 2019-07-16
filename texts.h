#ifndef _TEXTS_H
#define _TEXTS_H

#include <SDL2/SDL.h>



enum FontType{
	FONT_NRM,FONT_BIG,FONT_MID,FONT_SML
};
enum Align{
	AL_LEFT,AL_RIGHT,AL_CENTER
};

bool load_fonts();

class Msg{
	SDL_Texture* img;
	SDL_Renderer* rend;
	int w,h;
public:
	Msg(const char* text, SDL_Color color, FontType type, SDL_Renderer* renderer);
	void render_centered(int x, int y, Align a);
	bool exists();
};

#endif