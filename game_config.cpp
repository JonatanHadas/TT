#include "game_config.h"

GameConfig::GameConfig(int tk_n, int tm_n){
	tank_num = tk_n;
	team_num = tm_n;
	team_inds = new int[tk_n];
	colors = new int[tk_n];
	keys = new int[tk_n];
	names = new std::string[tk_n];
}
GameConfig::~GameConfig(){
	delete[] team_inds;
	delete[] colors;
	delete[] keys;
	delete[] names;
}