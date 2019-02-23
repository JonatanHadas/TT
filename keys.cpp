#include "keys.h"

#include <vector>
#include <stdio.h>

#define NAME "data/keysets"

std::vector<KeySet> keys;

void save_keys(){
	FILE* file = fopen(NAME, "w");
	if(file == NULL) return;
	size_t num = keys.size();
	if(fwrite(&num, sizeof(size_t), 1, file)<0) return;
	fwrite(&(keys[0]), sizeof(KeySet), num, file);
	fclose(file);
}

bool load_keys(){
	size_t num;
	
	FILE* file = fopen(NAME, "r");
	
	if(file != NULL){
	
	if(fread(&num, sizeof(size_t), 1, file)<1){
		fclose(file);
		return false;
	}
	
	KeySet* sets = new KeySet[num];
	
	if(fread(sets, sizeof(KeySet), num, file) < num){
		fclose(file);
		delete[] sets;
		return false;
	}
	
	fclose(file);
	
	for(size_t i = 0; i<num; i++) keys.push_back(sets[i]);
	
	
	delete[] sets;
	
	}
	atexit(save_keys);
	
	if(keys.size()<1){
		KeySet k;
		k.fd = SDLK_UP; k.bk = SDLK_DOWN; k.lt = SDLK_RIGHT; k.rt = SDLK_LEFT; k.sht = SDLK_SPACE;
		keys.push_back(k);
	}
	if(keys.size()<2){
		KeySet k;
		k.fd = SDL_GetKeyFromScancode(SDL_SCANCODE_W);
		k.bk = SDL_GetKeyFromScancode(SDL_SCANCODE_S);
		k.lt = SDL_GetKeyFromScancode(SDL_SCANCODE_D);
		k.rt = SDL_GetKeyFromScancode(SDL_SCANCODE_A);
		k.sht = SDL_GetKeyFromScancode(SDL_SCANCODE_GRAVE);
		keys.push_back(k);
	}
	
	return true;
}

KeySet& get_keyset(int i){
	while(keys.size()<= i) keys.push_back({SDLK_UNKNOWN,SDLK_UNKNOWN,SDLK_UNKNOWN,SDLK_UNKNOWN,SDLK_UNKNOWN});
	return keys[i];
}

const char* get_sym(SDL_Keycode k){
	switch(k){
	case SDLK_BACKSPACE:
		return "\xe2\x8c\xab";
	case SDLK_CAPSLOCK:
		return "\xe2\x87\xaa";
	case SDLK_DOWN:
		return "\xe2\x86\x93";
	case SDLK_UP:
		return "\xe2\x86\x91";
	case SDLK_LEFT:
		return "\xe2\x86\x90";
	case SDLK_RIGHT:
		return "\xe2\x86\x92";
	case SDLK_EJECT:
		return "\xe2\x8f\x8e";
	case SDLK_ESCAPE:
		return "\xe2\x90\x9b";
	case SDLK_INSERT:
		return "INS";
	case SDLK_KP_0:
		return "KP 0";
	case SDLK_KP_00:
		return "KP 00";
	case SDLK_KP_000:
		return "KP 000";
	case SDLK_KP_1:
		return "KP 1";
	case SDLK_KP_2:
		return "KP 2";
	case SDLK_KP_3:
		return "KP 3";
	case SDLK_KP_4:
		return "KP 4";
	case SDLK_KP_5:
		return "KP 5";
	case SDLK_KP_6:
		return "KP 6";
	case SDLK_KP_7:
		return "KP 7";
	case SDLK_KP_8:
		return "KP 8";
	case SDLK_KP_9:
		return "KP 9";
	case SDLK_KP_A:
		return "KP A";
	case SDLK_KP_B:
		return "KP B";
	case SDLK_KP_C:
		return "KP C";
	case SDLK_KP_D:
		return "KP D";
	case SDLK_KP_E:
		return "KP E";
	case SDLK_KP_F:
		return "KP F";
	case SDLK_KP_AMPERSAND:
		return "KP &";
	case SDLK_KP_AT:
		return "KP @";
	case SDLK_KP_BACKSPACE:
		return "KP \xe2\x8c\xab";
	case SDLK_KP_BINARY:
		return "KP Bin";
	case SDLK_KP_COLON:
		return "KP :";
	case SDLK_KP_COMMA:
		return "KP ,";
	case SDLK_KP_DBLAMPERSAND:
		return "KP &&";
	case SDLK_KP_DBLVERTICALBAR:
		return "KP \xe2\x88\xa5";
	case SDLK_KP_DECIMAL:
		return "KP Dec";
	case SDLK_KP_DIVIDE:
		return "KP /";
	case SDLK_KP_ENTER:
		return "KP \xe2\x8f\x8e";
	case SDLK_KP_EQUALS:
		return "KP =";
	case SDLK_KP_EXCLAM:
		return "KP !";
	case SDLK_KP_GREATER:
		return "KP >";
	case SDLK_KP_HASH:
		return "KP #";
	case SDLK_KP_HEXADECIMAL:
		return "KP Hex";
	case SDLK_KP_LEFTBRACE:
		return "KP {";
	case SDLK_KP_LEFTPAREN:
		return "KP (";
	case SDLK_KP_LESS:
		return "KP <";
	case SDLK_KP_MEMADD:
		return "KP M+";
	case SDLK_KP_MINUS:
		return "KP -";
	case SDLK_KP_MULTIPLY:
		return "KP *";
	case SDLK_KP_OCTAL:
		return "KP Oct";
	case SDLK_KP_PERCENT:
		return "KP %";
	case SDLK_KP_PERIOD:
		return "KP .";
	case SDLK_KP_PLUS:
		return "KP +";
	case SDLK_KP_PLUSMINUS:
		return "KP \xc2\xb1";
	case SDLK_KP_POWER:
		return "KP ^";
	case SDLK_KP_RIGHTBRACE:
		return "KP }";
	case SDLK_KP_RIGHTPAREN:
		return "KP )";
	case SDLK_KP_SPACE:
		return "KP \xe2\x90\xa3";
	case SDLK_KP_TAB:
		return "KP \xe2\x87\xa5";
	case SDLK_KP_VERTICALBAR:
		return "KP |";
	case SDLK_KP_XOR:
		return "KP Xor";
	case SDLK_PAGEDOWN:
		return "PgDn";
	case SDLK_PAGEUP:
		return "PgUp";
	case SDLK_PRINTSCREEN:
		return "PrtSc";
	case SDLK_DELETE:
		return "\xe2\x90\xa1";
	case SDLK_RETURN:
		return "\xe2\x8f\x8e";
	case SDLK_SPACE:
		return "\xe2\x90\xa3";
	case SDLK_TAB:
		return "\xe2\x90\xa3";
	case SDLK_UNKNOWN:
		return "";
	default:
		return SDL_GetKeyName(k);
	}
}