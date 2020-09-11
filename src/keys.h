#ifndef _KEYS_H
#define _KEYS_H

#include <SDL2/SDL.h>

struct KeySet{
	SDL_Keycode fd,bk,lt,rt,sht;
};

bool load_keys();

KeySet& get_keyset(int i);

const char* get_sym(SDL_Keycode k);

#endif