#ifndef _GAME_CONFIG_H
#define _GAME_CONFIG_H

#include <string>

#define UPG_MASK_GATLING 0x1
#define UPG_MASK_LASER 0x2
#define UPG_MASK_BOMB 0x4
#define UPG_MASK_DEATH_RAY 0x8
#define UPG_MASK_WIFI 0x10
#define UPG_MASK_MISSILE 0x20
#define UPG_MASK_ALL 0x3f

struct GameSettings{
	enum ScoreMeth{
		SCR_LAST,
		SCR_ORDER,
		SCR_DEATH,
	} scr_mth;
	int allow_dif; // should be 0,1 or 2
	enum EndMeth{
		END_ROUND,
		END_SCORE,
		END_NONE,
	} end_mth;
	int lim;
};

struct GameConfig{
	int tank_num;
	int team_num;
	int* team_inds;
	int upg_mask;
	int* colors;
	int* keys; // keyset number 
	std::string* names;
	GameSettings set;
	GameConfig(int tank_num, int team_num, int mask);
	~GameConfig();
};

#endif