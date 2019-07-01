#ifndef _GAME_CONFIG_H
#define _GAME_CONFIG_H

#define UPG_MASK_GATLING 0x1
#define UPG_MASK_LASER 0x2
#define UPG_MASK_BOMB 0x4
#define UPG_MASK_ALL 0x7

struct GameSettings{
	enum ScoreMeth{
		SCR_LAST,
		SCR_ORDER,
		SCR_DEATH,
	} scr_mth;
	int allow_dif; // shoud be 0,1 or 2
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
	GameSettings set;
	GameConfig(int tank_num, int team_num, int mask);
	~GameConfig();
};

#endif