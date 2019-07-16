#include "sounds.h"

#include <stdlib.h>

#include <SDL2/SDL_mixer.h>
#include <vector>

#define DIR "sounds/"
#define WAV ".wav"


// coll, explosion, beep, laser, raygun, rayload, on, off, explosion_small, popup, take
std::vector<Mix_Chunk*> sounds;

void free_sounds(){
	while(sounds.size()>0){
		Mix_FreeChunk(sounds.back());
		sounds.pop_back();
	}
}

bool load_sounds(){
	atexit(free_sounds);
	Mix_Chunk* chunk;
	
	chunk = Mix_LoadWAV(DIR "collision" WAV);
	if(!chunk) return false;
	Mix_VolumeChunk(chunk, MIX_MAX_VOLUME / 4);
	sounds.push_back(chunk);

	chunk = Mix_LoadWAV(DIR "explosion" WAV);
	if(!chunk) return false;
	Mix_VolumeChunk(chunk, MIX_MAX_VOLUME);
	sounds.push_back(chunk);

	chunk = Mix_LoadWAV(DIR "beep" WAV);
	if(!chunk) return false;
	Mix_VolumeChunk(chunk, MIX_MAX_VOLUME);
	sounds.push_back(chunk);

	chunk = Mix_LoadWAV(DIR "laser" WAV);
	if(!chunk) return false;
	Mix_VolumeChunk(chunk, MIX_MAX_VOLUME);
	sounds.push_back(chunk);

	chunk = Mix_LoadWAV(DIR "raygun" WAV);
	if(!chunk) return false;
	Mix_VolumeChunk(chunk, MIX_MAX_VOLUME);
	sounds.push_back(chunk);

	chunk = Mix_LoadWAV(DIR "rayload" WAV);
	if(!chunk) return false;
	Mix_VolumeChunk(chunk, MIX_MAX_VOLUME);
	sounds.push_back(chunk);

	chunk = Mix_LoadWAV(DIR "on" WAV);
	if(!chunk) return false;
	Mix_VolumeChunk(chunk, MIX_MAX_VOLUME);
	sounds.push_back(chunk);

	chunk = Mix_LoadWAV(DIR "off" WAV);
	if(!chunk) return false;
	Mix_VolumeChunk(chunk, MIX_MAX_VOLUME);
	sounds.push_back(chunk);

	chunk = Mix_LoadWAV(DIR "explosion_small" WAV);
	if(!chunk) return false;
	Mix_VolumeChunk(chunk, MIX_MAX_VOLUME);
	sounds.push_back(chunk);

	chunk = Mix_LoadWAV(DIR "pop_up" WAV);
	if(!chunk) return false;
	Mix_VolumeChunk(chunk, MIX_MAX_VOLUME/128);
	sounds.push_back(chunk);

	chunk = Mix_LoadWAV(DIR "take" WAV);
	if(!chunk) return false;
	Mix_VolumeChunk(chunk, MIX_MAX_VOLUME);
	sounds.push_back(chunk);

	return true;
}

int get_ind(Sound snd){
	switch(snd){
	case SND_COLLISION:
		return 0;
	case SND_EXPLOSION:
		return 1;
	case SND_BEEP:
		return 2;
	case SND_LASER:
		return 3;
	case SND_RAYGUN:
		return 4;
	case SND_RAYLOAD:
		return 5;
	case SND_ON:
		return 6;
	case SND_OFF:
		return 7;
	case SND_EXPLOSION_SMALL:
		return 8;
	case SND_POPUP:
		return 9;
	case SND_TAKE:
		return 10;
	}
	return -1;
}

void play(Sound snd){
	Mix_PlayChannel(-1,sounds[get_ind(snd)],0);
}

void silence(){
	Mix_HaltChannel(-1);
}
