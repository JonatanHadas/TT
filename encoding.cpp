#include "encoding.h"

//primitives

char* encode_char(char* buf, char c){
	*(buf++) = c;
	return buf;
}
char* decode_char(char* buf, char& c){
	c = *(buf++);
	return buf;
}

char* encode_bool(char* buf, bool i){
	bool* b = reinterpret_cast<bool*>(buf);
	*(b++) = i;
	return reinterpret_cast<char*>(b);
}
char* decode_bool(char* buf, bool& i){
	bool* b = reinterpret_cast<bool*>(buf);
	i = *(b++);
	return reinterpret_cast<char*>(b);
}

char* encode_int(char* buf, int i){
	int* b = reinterpret_cast<int*>(buf);
	*(b++) = i;
	return reinterpret_cast<char*>(b);
}
char* decode_int(char* buf, int& i){
	int* b = reinterpret_cast<int*>(buf);
	i = *(b++);
	return reinterpret_cast<char*>(b);
}

char* encode_double(char* buf, double x){
	double* b = reinterpret_cast<double*>(buf);
	*(b++) = x;
	return reinterpret_cast<char*>(b);
}
char* decode_double(char* buf, double& x){
	double* b = reinterpret_cast<double*>(buf);
	x = *(b++);
	return reinterpret_cast<char*>(b);
}

char* encode_long(char* buf, long long int i){
	long long int* b = reinterpret_cast<long long int*>(buf);
	*(b++) = i;
	return reinterpret_cast<char*>(b);
}
char* decode_long(char* buf, long long int& i){
	long long int* b = reinterpret_cast<long long int*>(buf);
	i = *(b++);
	return reinterpret_cast<char*>(b);
}
#include <string.h>
char* encode_str(char* buf, const char* s){
	strcpy(buf,s);
	return buf + 1 + strlen(buf);
}
char* decode_str(char* buf, char* s){
	strcpy(s,buf);
	return buf + 1 + strlen(buf);
}


//enums

#include <map>

#define SCR_MTH_NUM 3
std::pair<GameSettings::ScoreMeth, int> i2scr[SCR_MTH_NUM] = {
	{GameSettings::SCR_LAST,0},
	{GameSettings::SCR_ORDER,1},
	{GameSettings::SCR_DEATH,2},
};
std::map<GameSettings::ScoreMeth, int> scr2i(i2scr, i2scr+SCR_MTH_NUM);

char* encode_scr_mth(char* buf, GameSettings::ScoreMeth s){
	return encode_int(buf, scr2i[s]);
}
char* decode_scr_mth(char* buf, GameSettings::ScoreMeth& s){
	int i;
	buf = decode_int(buf, i);
	s = i2scr[i].first;
	return buf;
}

#define END_MTH_NUM 3
std::pair<GameSettings::EndMeth, int> i2end[END_MTH_NUM] = {
	{GameSettings::END_NONE,0},
	{GameSettings::END_ROUND,1},
	{GameSettings::END_SCORE,2},
};
std::map<GameSettings::EndMeth, int> end2i(i2end, i2end+END_MTH_NUM);

char* encode_end_mth(char* buf, GameSettings::EndMeth e){
	return encode_int(buf, end2i[e]);
}
char* decode_end_mth(char* buf, GameSettings::EndMeth& e){
	int i;
	buf = decode_int(buf, i);
	e = i2end[i].first;
	return buf;
}

// structs

char* encode_gamesett(char* buf, GameSettings s){
	buf = encode_scr_mth(buf, s.scr_mth);
	buf = encode_int(buf, s.allow_dif);
	buf = encode_end_mth(buf, s.end_mth);
	return encode_int(buf, s.lim);
}
char* decode_gamesett(char* buf, GameSettings& s){
	buf = decode_scr_mth(buf, s.scr_mth);
	buf = decode_int(buf, s.allow_dif);
	buf = decode_end_mth(buf, s.end_mth);
	return decode_int(buf, s.lim);
}

#define SHOT_TYPE_NUM 1
std::pair<GenShot::Type, int> i2sht[SHOT_TYPE_NUM] = {
	{GenShot::TYPE_REG,0},
};
std::map<GenShot::Type, int> sht2i(i2sht, i2sht+SHOT_TYPE_NUM);

char* encode_shot_type(char* buf, GenShot::Type t){
	return encode_int(buf, sht2i[t]);
}
char* decode_shot_type(char* buf, GenShot::Type& t){
	int i;
	buf = decode_int(buf, i);
	t = i2sht[i].first;
	return buf;
}

#define TANK_STATE_NUM 4
std::pair<Tank::State, int> i2stt[TANK_STATE_NUM] = {
	{Tank::REG,0},
	{Tank::GATLING,1},
	{Tank::GATLING_WAIT,2},
	{Tank::GATLING_SHOOT,3},
};
std::map<Tank::State, int> stt2i(i2stt, i2stt+TANK_STATE_NUM);

char* encode_tank_state(char* buf, Tank::State s){
	return encode_int(buf, stt2i[s]);
}
char* decode_tank_state(char* buf, Tank::State& s){
	int i;
	buf = decode_int(buf, i);
	s = i2stt[i].first;
	return buf;
}

std::pair<Upgrade::Type, int> i2upg[UPG_NUM] = {
	{Upgrade::GATLING,0},
};
std::map<Upgrade::Type, int> upg2i(i2upg, i2upg+UPG_NUM);

char* encode_upgrade(char* buf, Upgrade::Type u){
	return encode_int(buf, upg2i[u]);
}
char* decode_upgrade(char* buf, Upgrade::Type& u){
	int i;
	buf = decode_int(buf, i);
	u = i2upg[i].first;
	return buf;
}

