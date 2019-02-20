#ifndef _GAME_CONFIG_H
#define _GAME_CONFIG_H

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
	GameSettings set;
	GameConfig(int tank_num, int team_num);
	~GameConfig();
};

#endif