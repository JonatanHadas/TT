#ifndef _ENCODING_H
#define _ENCODING_H

// functions en/de-code write/read to/from buf and return pointer to next position

char* encode_char(char* buf, char c);
char* decode_char(char* buf, char& c);

char* encode_bool(char* buf, bool c);
char* decode_bool(char* buf, bool& c);

char* encode_int(char* buf, int i);
char* decode_int(char* buf, int& i);

char* encode_double(char* buf, double x);
char* decode_double(char* buf, double& x);

char* encode_long(char* buf, long long int i);
char* decode_long(char* buf, long long int& i);

char* encode_str(char* buf, const char* s);
char* decode_str(char* buf, char* s);

#include "game_config.h"

char* encode_scr_mth(char* buf, GameSettings::ScoreMeth s);
char* decode_scr_mth(char* buf, GameSettings::ScoreMeth& s);

char* encode_end_mth(char* buf, GameSettings::EndMeth e);
char* decode_end_mth(char* buf, GameSettings::EndMeth& e);

char* encode_gamesett(char* buf, GameSettings s);
char* decode_gamesett(char* buf, GameSettings& s);

#include "game.h"

char* encode_shot_type(char* buf, GenShot::Type t);
char* decode_shot_type(char* buf, GenShot::Type& t);

char* encode_tank_state(char* buf, Tank::State s);
char* decode_tank_state(char* buf, Tank::State& s);

char* encode_upgrade(char* buf, Upgrade::Type u);
char* decode_upgrade(char* buf, Upgrade::Type& u);


#endif