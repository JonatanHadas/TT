#ifndef _SOUNDS_H
#define _SOUNDS_H

#define CHANNELS 4

bool load_sounds();

enum Sound{
	SND_COLLISION,
	SND_EXPLOSION,
	SND_EXPLOSION_SMALL,
	SND_BEEP,
	SND_LASER,
	SND_RAYGUN,
	SND_RAYLOAD,
	SND_ON, SND_OFF,
	SND_POPUP,
	SND_TAKE
};

void play(Sound snd);

void silence();

#endif