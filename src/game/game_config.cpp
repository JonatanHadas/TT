#include "game_config.h"

GameConfig::GameConfig(int tk_n, int tm_n, int mask){
	tank_num = tk_n;
	team_num = tm_n;
	team_inds = new int[tk_n];
	upg_mask = mask;
}
GameConfig::~GameConfig(){
	delete[] team_inds;
}